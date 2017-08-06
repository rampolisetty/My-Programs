/*
 * mailto-sendmail.c
 *
 * Given a mailto URI (which is of one of the below formats) parses the same
 * and sends a mail with given data. This logic can be reused in our CLI. 
 *
 * Examples: 
 *     mailto:name@domain.net
 *     mailto:obama@whitehouse.gov,michelle@whitehouse.gov
 *     mailto:obama@whitehouse.gov?cc=michelle@whitehouse.gov&bcc=joe@whitehouse.gov
 *     mailto:obama@whitehouse.gov?subject=Congrats%20Obama
 *     mailto:name@domain.net,name@domain.net?subject=the sample subject&cc=name@domain.net,name@domain.net&body=this is the body
 *     mailto:?to=name@domain.net&subject=the sample subject&cc=name@domain.net,name@domain.net&body=this is the body
 *     mailto:?to=name@domain.net&subject=the sample subject&cc=name@domain.net,name@domain.net&body=this is the body&server=smtp.domain.net:8000
 *     mailto:?to=name@domain.net&subject=the sample subject&cc=name@domain.net,name@domain.net&body=this is the body&server=smtp.domain.net:8000&user=abc&password=1234
 *
 *
 * We use libcurl in backend to send mail.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <sys/queue.h>
#include <sys/stat.h>

#include <curl/curl.h>


/* Our defines */

#define TRUE 1
#define FALSE 0

#define TMP_FILE_EMAIL "email-tmp.txt"

#define MAILTO_URI_SCHEME "mailto:"
#define MAILTO_URI_SCHEME_LENGTH (sizeof(MAILTO_URI_SCHEME) - 1)

#define MAILTO_FIRST_DELIMITER_CHAR '?'
#define MAILTO_KEY_DELIM_CHAR '&'
#define MAILTO_VALUE_DELIM_CHAR ','

/* List of allowed keys/case sensitive */
#define KEY_FROM "from"
#define KEY_SUBJECT "subject"
#define KEY_SMTP_SERVER "smtp-server"
#define KEY_USER_NAME "user"
#define KEY_PASSWORD "password"
#define KEY_TO "to"
#define KEY_CC "cc"
#define KEY_BCC "bcc"

/* These strings are for header information */
#define HEADER_STR_FROM "From"
#define HEADER_STR_SUBJECT "Subject"
#define HEADER_STR_TO "To"
#define HEADER_STR_CC "Cc"
#define HEADER_STR_BCC "Bcc"
#define HEADER_ALIAS_SEPARATOR ", "

/*
 * A TAILQ to hold a list of aliases (email addresses).
 */
typedef struct cli_mailto_alias_s {
    char *cma_value;                                  /* Value */
    STAILQ_ENTRY(cli_mailto_alias_s) cma_sibling;     /* Sibling */
} cli_mailto_alias_t;

/* Head of TAILQ */
typedef
STAILQ_HEAD(cli_mailto_alias_list_s, 
            cli_mailto_alias_s) cli_mailto_alias_list_t;

/*
 * A struct to hold parsed mailto URI Data.
 */
typedef struct cli_mailto_uri_data_s {
    char *cmud_from;                      /* From address */
    char *cmud_subject;                   /* Subject */
    char *cmud_smtp_server;               /* Smtp server */
    char *cmud_user_name;                 /* User name for SMTP login */
    char *cmud_password;                  /* Password (plain text) for SMTP login */
    char *cmud_body;                      /* Body of the mail. TODO */
    cli_mailto_alias_list_t *cmud_to_list;/* TO List */
    cli_mailto_alias_list_t *cmud_cc_list;/* CC List */
    cli_mailto_alias_list_t *cmud_bcc_list;/* BCC List */
} cli_mailto_uri_data_t;


/*  -----------   HELPER FUNCTIONS --------- */

static inline char *
allocadupx (char *to, const char *from)
{
    if (to) strcpy(to, from);
    return to;
}
#define ALLOCADUP(s) allocadupx((char *) alloca(strlen(s) + 1), s)

static inline int
streq (const char *red, const char *blue) 
{
    return *red == *blue && (*red == '\0' || strcmp(red + 1, blue + 1) == 0);
}


/*  -----------   IMPLEMENTATION FUNCTIONS --------- */


/*
 * cli_mailto_alias_node_new () -- Allocates memory for storing a single alias
 * node. 
 */
static inline cli_mailto_alias_t *
cli_mailto_alias_node_new (char *to_address)
{
    cli_mailto_alias_t *ptr = NULL;

    ptr = calloc(1, sizeof(cli_mailto_alias_t));
    /* TODO INSIST */

    ptr->cma_value = strdup(to_address);
    /* TODO INSIST */

    return ptr;
}

/*
 * cli_mailto_alias_list_new () -- Helper function to allocate memory to hold
 * an alias list
 */
static inline cli_mailto_alias_list_t *
cli_mailto_alias_list_new (void)
{
    void *ptr = NULL;

    ptr = calloc(1, sizeof(cli_mailto_alias_list_t));
    /* TODO INSIST */

    return ptr;
}

/*
 * cli_mailto_uri_data_node_new () -- Allocates memory to hold
 * cli_mailto_uri_data_t. Initializes the allocated memory chunk to zero.
 */
static inline cli_mailto_uri_data_t *
cli_mailto_uri_data_node_new (void)
{
    cli_mailto_uri_data_t *ptr = NULL;

    /* Zeroize */
    ptr = calloc(1, sizeof(cli_mailto_uri_data_t));
    /* TODO replace with INIST */

    /* Allocate memory for alias lists and initialize them */
    ptr->cmud_to_list = cli_mailto_alias_list_new();
    ptr->cmud_cc_list = cli_mailto_alias_list_new();
    ptr->cmud_bcc_list = cli_mailto_alias_list_new();

    /*  Initialize the lists */
    STAILQ_INIT(ptr->cmud_to_list);
    STAILQ_INIT(ptr->cmud_cc_list);
    STAILQ_INIT(ptr->cmud_bcc_list);

    return ptr; /* Done */
}

/*
 * print_alias_list () -- Helper function to print aliases from the given list
 */
static void
print_alias_list (cli_mailto_alias_list_t *to_listp, 
                  char *sep_str, FILE *fp)
{
    cli_mailto_alias_t *nodep = NULL;

    STAILQ_FOREACH(nodep, to_listp, cma_sibling) {
        fprintf(fp ? : stdout, "%s%s", 
                nodep->cma_value,
                STAILQ_NEXT(nodep, cma_sibling) ? sep_str : "");
    }
}

/*
 * cli_construct_alias_list () -- Parses the given alias list and prepares a
 * list of aliases of type cli_mailto_alias_list_t.
 */
void
cli_construct_alias_list (char *alias_list_buf, 
                        cli_mailto_alias_list_t *in_alias_list)
{
    cli_mailto_alias_t *nodep = NULL;
    char *tok = NULL;
    char *save_ptr = NULL;

    if (!alias_list_buf || *alias_list_buf == '\0')
        return;

    /*
     * We can support appending the list too. It may happen in below example
     * cases:
     *
     * Ex:
     * mailto://abc@domain.com?to=def@domain.com&cc=ghi@domain.net&cc=xyz@domain.com,root@domain.net
     *
     * Now scan through the alias list and add them to our list given.
     */
    tok = strtok_r(alias_list_buf, ",", &save_ptr);
    while (tok) {
        nodep = cli_mailto_alias_node_new(tok);
        STAILQ_INSERT_TAIL(in_alias_list, nodep, cma_sibling);
        tok = strtok_r(NULL, ",", &save_ptr);
    }
}


/*
 * cli_validate_mailto_data () -- Validates the given mailto uri data.
 *
 * Returns:
 *        TRUE on Success (validation is good).
 *        FALSE on failure.
 */
static bool
cli_validate_mailto_data (cli_mailto_uri_data_t *mailto_data) 
{
    bool rc = TRUE;

    /* From address is mandatory */
    if (mailto_data->cmud_from == NULL) {
        rc = FALSE;
        fprintf(stderr, "\nFrom address is missing\n");
    }

    /* To address is mandatory */
    if (STAILQ_EMPTY(mailto_data->cmud_to_list)) {
        rc = FALSE;
        fprintf(stderr, "\nTo address is missing\n");
    }

    /* Smtp Server is mandatory */
    if (mailto_data->cmud_smtp_server == NULL) {
        rc = FALSE;
        fprintf(stderr, "\nSmtp server details are missing\n");
    }

    /*
     * Print the email data for debug purpose.
     */
#ifdef DEBUG
    fprintf(stdout, "\n======= Here is the complete data parsed ====== \n");
    fprintf(stdout, "\nFrom address [%s]\n", mailto_data->cmud_from);
    fprintf(stdout, "\nSubject [%s]\n", mailto_data->cmud_subject);
    fprintf(stdout, "\nSMTP Server [%s]\n", mailto_data->cmud_smtp_server);
    fprintf(stdout, "\nUser name [%s]\n", mailto_data->cmud_user_name);
    fprintf(stdout, "\nPassword [%s]\n", mailto_data->cmud_password);
    fprintf(stdout, "\nThe TO list....\n");
    print_alias_list(mailto_data->cmud_to_list, "\n", NULL);
    fprintf(stdout, "\n\nThe CC list....\n");
    print_alias_list(mailto_data->cmud_cc_list, "\n", NULL);
    fprintf(stdout, "\n\nThe BCC list....\n");
    print_alias_list(mailto_data->cmud_bcc_list, "\n", NULL);
    fprintf(stdout, "\n=============================================== \n");
#endif

    return rc; /* Done */
}

/*
 * cli_add_key_value_to_maildata () -- Adds the given value to the given key
 * in corresponding mailto uri data given.
 */
static void
cli_add_key_value_to_maildata (char *key, char *value,
                               cli_mailto_uri_data_t *mailto_uri_data)
{
    /* Make sure the data is sane. */
    if (!key || !value || !mailto_uri_data)
        return;

    /*
     * Now match the key and add the value. Memory allocated here gets free'd
     * later. 
     */
    if (streq(key, KEY_SUBJECT)) {
        /* Copy subject */
        if (!mailto_uri_data->cmud_subject)
            mailto_uri_data->cmud_subject = strdup(value);
        else
            fprintf(stderr, "\nIgnoring duplicate %s=%s\n", key, value);
    } else if (streq(key, KEY_SMTP_SERVER)) {
        /* Copy smtp server details */
        if (!mailto_uri_data->cmud_smtp_server)
            mailto_uri_data->cmud_smtp_server = strdup(value);
        else
            fprintf(stderr, "\nIgnoring duplicate %s=%s\n", key, value);
    } else if (streq(key, KEY_FROM)) {
        /* Copy From address */
        if (!mailto_uri_data->cmud_from)
            mailto_uri_data->cmud_from = strdup(value);
        else
            fprintf(stderr, "\nIgnoring duplicate %s=%s\n", key, value);
    } else if (streq(key, KEY_USER_NAME)) {
        /* Copy user name */
        if (!mailto_uri_data->cmud_user_name)
            mailto_uri_data->cmud_user_name = strdup(value);
        else
            fprintf(stderr, "\nIgnoring duplicate %s=%s\n", key, value);
    } else if (streq(key, KEY_PASSWORD)) {
        /* Copy password */
        if (!mailto_uri_data->cmud_password)
            mailto_uri_data->cmud_password = strdup(value);
        else
            fprintf(stderr, "\nIgnoring duplicate %s=%s\n", key, value);
    } else if (streq(key, KEY_TO)) {
        /* Copy to list */
        if (!STAILQ_EMPTY(mailto_uri_data->cmud_to_list))
            fprintf(stdout, "\nAppening the new To list [%s]\n", value);

        /* Copy/Append to list */
        cli_construct_alias_list(value, mailto_uri_data->cmud_to_list);
    } else if (streq(key, KEY_CC)) {
        /* Copy cc address */
        if (!STAILQ_EMPTY(mailto_uri_data->cmud_cc_list))
            fprintf(stdout, "\nAppening the new CC list [%s]\n", value);

        /* Copy/Append cc address */
        cli_construct_alias_list(value, mailto_uri_data->cmud_cc_list);
    } else if (streq(key, KEY_BCC)) {
        /* Copy bcc list */
        if (!STAILQ_EMPTY(mailto_uri_data->cmud_bcc_list))
            fprintf(stdout, "\nAppening the new BCC list [%s]\n", value);

        /* Copy/append bcc list */
        cli_construct_alias_list(value, mailto_uri_data->cmud_bcc_list);
    } else {
#ifdef DEBUG
        fprintf(stdout, "\nUnsupported key [%s]\n", key);
#endif
    }
}

/*
 * cli_add_key_value () -- From the given key=value pair identifies the key
 * and value and adds the same to mailto_uri_data given.
 */
static void
cli_add_key_value (const char *key_value_pair, 
                   cli_mailto_uri_data_t *mailto_uri_data)
{
    char *ptr = NULL;
    char *key = NULL;
    char *value = NULL;

    /* Make sure the data is sane. */
    if (!mailto_uri_data || !key_value_pair)
        return;

    /* Get our own memory */
    ptr = strdup(key_value_pair); 
    /* TODO INSIST */

    /*
     * Example: 
     *
     * subject=sample
     */
    key = ptr;
    value = strchr(key, '=');
    if (value) {
        *value = '\0';
        value = value + 1;
    }
    /* Add this */
    cli_add_key_value_to_maildata(key, value, mailto_uri_data);
    free(ptr);
}

/* 
 * cli_mailto_get_first_delim_ptr() -- Helper function which passes the
 * pointer to first delimiter char i.e. '?' in given URI. While doing so it
 * performs some basic validations too.
 */
static char *
cli_mailto_get_first_delim_ptr (char *mailto_uri, bool log_error)
{
    char *first_delim_ptr = NULL;
    char *ptr = NULL;

    /* Make sure the data is sane. */
    if (!mailto_uri)
        return NULL;

    /* Fetch pointer to '?' */
    first_delim_ptr = strchr(mailto_uri, MAILTO_FIRST_DELIMITER_CHAR);
    if (!first_delim_ptr) {
        if (log_error)
            fprintf(stderr, "\nInvalid mailto URI, missing '?'\n");
        return NULL;
    }

    /* Make sure only one '?' is specified in given URI */
    if (first_delim_ptr != strrchr(mailto_uri, MAILTO_FIRST_DELIMITER_CHAR)) {
        if (log_error)
            fprintf(stderr, "\nInvalid mailto URI, duplicate '?'\n");
        return NULL;
    }

    /* 
     * We have valid '?' specified, make sure '?' is specified first before
     * '&' delimiter. 
     */
    ptr = strchr(mailto_uri, MAILTO_KEY_DELIM_CHAR);
    if (ptr && (ptr < first_delim_ptr)) { /* Endianness issues? */
        if (log_error)
            fprintf(stderr, "\nInvalid mailto URI, invalid '&'\n");
        return NULL;
    }

    return first_delim_ptr; /* Done */
}

/*
 * cli_construct_mailto_uri_data () -- Parses and constructs the given mailto
 * uri data. Helper for cli_parse_mailto_uri().
 */
static cli_mailto_uri_data_t *
cli_construct_mailto_uri_data (const char *mailto_uri,
                               bool parse_to_list)
{
    size_t to_list_buf_sz = 0;
    char *tok = NULL;
    char *ptr = NULL;
    char *save_ptr = NULL;
    char *to_list_buf = NULL;
    char *mailto_uri_cache = NULL;
    cli_mailto_uri_data_t *mailto_uri_data = NULL;

    /* Sanity */
    if (!mailto_uri || *mailto_uri == '\0')
        return mailto_uri_data;

    /* Cache the data */
    mailto_uri_cache = ALLOCADUP(mailto_uri);

    /* Allocate mail to uri data */
    mailto_uri_data = cli_mailto_uri_data_node_new();

    /* Construct TO list if asked. */
    if (parse_to_list) {
        /*
         * The input URI could be:
         *
         * name@domain.net?cc=name@domain.net
         * name@domain.net?cc=name@domain.net&subject=sub
         */
        ptr = cli_mailto_get_first_delim_ptr(mailto_uri_cache, TRUE);
        if (!ptr) {
            return mailto_uri_data;
        }

        /* Prepare TO List buffer */
        to_list_buf_sz = (ptr - mailto_uri_cache + 1);
        to_list_buf = alloca(to_list_buf_sz);
        /* TODO have INSIST */
        bzero(to_list_buf, to_list_buf_sz);
        strncpy(to_list_buf, mailto_uri_cache, (to_list_buf_sz - 1));
        /* Now preapre the list containing "TO" mail addresses */
        cli_construct_alias_list(to_list_buf, mailto_uri_data->cmud_to_list);

        /* Done with TO list, move to next */
        mailto_uri_cache = ptr + 1;
    }

    /*
     * Now parse each "hname = hvalue" pairs like:
     * 
     * Example: cc=name@domain.net&subject=sub
     */
    if (*mailto_uri_cache) {
        /*
         * Here each key = value (or hname = hvalue) is separated by '&'
         */
        tok = strtok_r(mailto_uri_cache, "&", &save_ptr);
        while (tok) {
            /* Make sure we have a valid key=value pair. */
            if (strchr(tok, '=') == NULL) {
                fprintf(stderr, "\nInvalid key=value pair [%s], ignoring\n", tok);
            } else {
                cli_add_key_value(tok, mailto_uri_data);
            }
            /* Get next token */
            tok = strtok_r(NULL, "&", &save_ptr);
        }

        /* Add the last missing token */
        if (tok)
            cli_add_key_value(tok, mailto_uri_data);


        /* 
         * We now have mailto_data successfully constructed. Validate the
         * same. 
         */
        if (!cli_validate_mailto_data(mailto_uri_data)) {
            fprintf(stderr, "\nIgnoring this mailto request\n");
            /* TODO free the data */
            return NULL;
        }
    }

    return mailto_uri_data; /* Done */
}

/*
 * cli_parse_mailto_uri () -- Parses the given mailto_uri and
 * constructs and returns cli_mailto_uri_data_t *. 
 *
 * User/Caller has to free the memory returned. 
 * All required error messages will be printed by this function.
 *
 * Examples: 
 *     mailto:name@domain.net
 *     mailto:obama@whitehouse.gov,michelle@whitehouse.gov
 *     mailto:obama@whitehouse.gov?cc=michelle@whitehouse.gov&bcc=joe@whitehouse.gov
 *     mailto:obama@whitehouse.gov?subject=Congrats Obama
 *     mailto:name@domain.net,name@domain.net?subject=the sample subject&cc=name@domain.net,name@domain.net&body=this is the body
 *     mailto:?to=name@domain.net&subject=the sample subject&cc=name@domain.net,name@domain.net&body=this is the body
 *     mailto:?to=name@domain.net&subject=the sample subject&cc=name@domain.net,name@domain.net&body=this is the body&server=smtp.domain.net:8000
 *     mailto:?to=name@domain.net&subject=the sample subject&cc=name@domain.net&bcc=name@domain.net&body=this is the body&server=smtp.domain.net:8000&user=abc&password=1234
 *
 */
static cli_mailto_uri_data_t *
cli_parse_mailto_uri (const char *mailto_uri)
{
    uint32_t idx = 0;
    size_t uri_len = 0;
    cli_mailto_uri_data_t *mailto_uri_data = NULL;

    /* Make sure of data */
    if (!mailto_uri)
        return mailto_uri_data;

    /* Fetch the input uri length */
    uri_len = strlen(mailto_uri);

    /* Validate format */
    if (strncmp(mailto_uri, MAILTO_URI_SCHEME, MAILTO_URI_SCHEME_LENGTH)) {
        fprintf(stderr, "Error: Invalid mailto format in [%s], "
                "Please refer documentation for more details\n",
                mailto_uri);
        return mailto_uri_data; /* Done with error  */
    }

#ifdef DEBUG
    fprintf(stdout, "Received a valid URI scheme [mailto:]\n");
#endif

    /* Few bytes have been validated already */
    idx = idx + MAILTO_URI_SCHEME_LENGTH;

    /*
     * Check if the next character is '?'
     *
     * Example: mailto:?to=abc@domain.net
     */
    if (idx < uri_len &&
        mailto_uri[idx] == MAILTO_FIRST_DELIMITER_CHAR) {
        /* 
         * Fetch all key:value pairs 
         */
        mailto_uri_data = cli_construct_mailto_uri_data(mailto_uri + idx + 1, 
                                                        FALSE);
    } else {
        /*
         * Example:
         *
         * mailto:abc@domain.net,def@domain.net?cc=ghi@domain.net
         */
        mailto_uri_data = cli_construct_mailto_uri_data(mailto_uri + idx, TRUE);
    }

    return mailto_uri_data; /* Done */
}

/*
 * cli_print_email_header () -- Decorates the email by printing the header
 * information. No Validations inside this function.
 *
 * Basically prints:
 *
 * From: user@domain.net
 * To: user@domain.net
 * Cc: user@domain.net
 * Bcc: user@domain.net
 * Subject: Sub
 */
static void
cli_print_email_header (FILE *fp, 
                        const cli_mailto_uri_data_t *mailto_uri_data)
{
    if (!fp || !mailto_uri_data)
        return;

    /* Print from address */
    fprintf(fp, "%s: %s\n", HEADER_STR_FROM, mailto_uri_data->cmud_from);

    /* Print TO List */
    fprintf(fp, "%s: ", HEADER_STR_TO);
    print_alias_list(mailto_uri_data->cmud_to_list, 
                     HEADER_ALIAS_SEPARATOR, fp);
    fprintf(fp, "%s", "\n");

    /* Print CC List */
    if (!STAILQ_EMPTY(mailto_uri_data->cmud_cc_list)) {
        fprintf(fp, "%s: ", HEADER_STR_CC);
        print_alias_list(mailto_uri_data->cmud_cc_list, 
                         HEADER_ALIAS_SEPARATOR, fp);
        fprintf(fp, "%s", "\n");
    }

    /* Print BCC List */
    if (!STAILQ_EMPTY(mailto_uri_data->cmud_bcc_list)) {
        fprintf(fp, "%s: ", HEADER_STR_BCC);
        print_alias_list(mailto_uri_data->cmud_bcc_list, 
                         HEADER_ALIAS_SEPARATOR, fp);
        fprintf(fp, "%s", "\n");
    }

    /* Print Subject */
    fprintf(fp, "%s: %s\n", HEADER_STR_SUBJECT, mailto_uri_data->cmud_subject);

    /* Separate header from body */
    fprintf(fp, "%s", "\n\n");
}

/*
 * cli_prepare_curl_recipient_list () --
 */
static void
cli_prepare_curl_recipient_list (const cli_mailto_uri_data_t *mailto_uri_data,
                                 struct curl_slist **recipients)
{
    cli_mailto_alias_t *nodep = NULL;

    if (!mailto_uri_data || !recipients)
        return;

    /*
     * Append
     *      -> To List
     *      -> CC List
     *      -> BCC List
     *
     * As Mail recipients
     */

    STAILQ_FOREACH(nodep, mailto_uri_data->cmud_to_list, cma_sibling) {
        /* Append */
        *recipients = curl_slist_append(*recipients, nodep->cma_value);
    }

    STAILQ_FOREACH(nodep, mailto_uri_data->cmud_cc_list, cma_sibling) {
        /* Append */
        *recipients = curl_slist_append(*recipients, nodep->cma_value);
    }

    STAILQ_FOREACH(nodep, mailto_uri_data->cmud_bcc_list, cma_sibling) {
        /* Append */
        *recipients = curl_slist_append(*recipients, nodep->cma_value);
    }
}

/*
 * cli_send_mail_with_libcurl () -- The ultimate function for sending mail
 * with libcurl APIs using the given mailto_uri_data.
 *
 * Returns: FALSE on Success
 *          TRUE on failure.
 */
static bool
cli_send_mail_with_libcurl (const cli_mailto_uri_data_t *mailto_uri_data,
                            const char *email_tmp_file)
{
    FILE *fp = NULL;
    CURL *curl = NULL;
    CURLcode res;
    struct curl_slist *recipients = NULL;
    struct stat file_info;

    if (!email_tmp_file || !mailto_uri_data)
        return TRUE;

    /* Open the email file */
    fp = fopen(email_tmp_file, "rb");
    if (!fp) {
        fprintf(stderr, "\nUnable to open email file\n");
        return TRUE;
    }

    /* to get the file size */ 
    if(fstat(fileno(fp), &file_info) != 0) {
        fprintf(stderr, "\nCannot stat file\n");
        return TRUE; /* can't continue */ 
    }

    /* Initialize libcurl */
    curl = curl_easy_init();
    if (curl) {

        /* This is the URL for your mailserver */
        curl_easy_setopt(curl, CURLOPT_URL, mailto_uri_data->cmud_smtp_server);

        /* Set mandatory FROM address */
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, mailto_uri_data->cmud_from);

        /* Set Recipients (includes all TO/CC/BCC) */
        cli_prepare_curl_recipient_list(mailto_uri_data, &recipients);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        /* Set source data to upload */
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(curl, CURLOPT_READDATA, fp);
        /* and give the size of the upload (optional) */
        curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,
                         (curl_off_t)file_info.st_size);

        /* Send the message */
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            /* TODO check for errors and print speed etc */
        }

        /* Free the list of recipients */
        curl_slist_free_all(recipients);

        curl_easy_cleanup(curl);
    }

    fclose(fp);

    return FALSE;
}

/*
 * cli_do_send_mail () -- Parses and validates the given mail to URI and
 * performs the necessary action, i.e. to send mail using libcurl APIs
 */
static void
cli_do_send_mail (const char *mailto_uri)
{
    FILE *fp = NULL;
    cli_mailto_uri_data_t *mailto_uri_data = NULL;

    /* Make sure the data is sane. */
    if (!mailto_uri) {
        fprintf(stderr, "\nNo URI is given\n");
        return;
    }

    /* Parse and fetch mail data */
    mailto_uri_data = cli_parse_mailto_uri(mailto_uri);
    if (!mailto_uri_data) {
        fprintf(stderr, "\nUnable to parse mail data, please refer "
                "documentation for more details\n");
        return;
    }

    /*
     * Open a file which actually contains the email data.
     */
    fp = fopen(TMP_FILE_EMAIL, "w");
    if (!fp) {
        fprintf(stderr, "\nUnable to open temp file, error [%s],"
                " cannot send mail", strerror(errno));
        return;
    }

    /* Print email header. */
    cli_print_email_header(fp, mailto_uri_data);
    fclose(fp); /* Done printing header. */

    /* Now send mail using libcurl helpers TODO */
    cli_send_mail_with_libcurl(mailto_uri_data, TMP_FILE_EMAIL);

#ifndef DEBUG
    unlink(TMP_FILE_EMAIL);
#endif
}

/*
 * cli_print_mailto_usage () -- Helper function which prints usage of this
 * program.
 */
static void
cli_print_mailto_usage (void)
{
    fprintf(stderr, "\nPlease find usage examples below. Please note "
            "From address, SMTP Server details and To addresses are "
            "mandatory\n");

    fprintf(stderr, "\n./mailto \"mailto:abc@domain.com?from=def@domain.com&"
            "smtp-server=192.168.1.1:25\"\n");
    fprintf(stderr, "\n./mailto \"mailto:abc@domain.com?from=def@domain.com&"
            "smtp-server=192.168.1.1:25&subject=sub\"\n");
    fprintf(stderr, "\n./mailto \"mailto:abc@domain.com?from=def@domain.com&"
            "smtp-server=192.168.1.1:25&subject=sub&cc=user@domain.net\"\n");
    fprintf(stderr, "\n./mailto \"mailto:abc@domain.com?from=def@domain.com&"
            "smtp-server=192.168.1.1:25&subject=sub&cc=user@domain.net&"
            "bcc=user@domain.net\"\n");
}

/*
 * main () function
 */
int
main (int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "\nInvalid arguments\n");
        cli_print_mailto_usage();
        errx(EXIT_FAILURE, "\nPlease refer documentation for more details\n");
    }

    /*
     * Perform send mail action
     */
    cli_do_send_mail(argv[1]);

    fprintf(stdout, "\n\n");

    return EXIT_SUCCESS;
}

