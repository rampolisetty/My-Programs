/*
 * start-end-string-check.c
 *
 * Author: Ram Polisetty
 *
 * Given a string str and a corner string cs, we need to find out whether the
 * string str starts and ends with the corner string cs or not.
 *
 * Input : str = "geeksmanishgeeks", cs = "geeks"
 *                     ^    ^
 * Output : Yes
 *
 * Input : str = "shreya dhatwalia", cs = "abc"
 * Output : No
 *
 * Input: str = "geeks", cs = "geeks"
 * Output : Yes
 *
 * How to compile?
 *
 * gcc start-end-string-check.c -g -Wall -Werror
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*
 * do_check_string_start_end () -- Performs the required logic and returns
 * true for Success case or false otherwise.
 */
static bool
do_check_string_start_end (const char *master_str,
                           const char *match_str)
{
    uint32_t pos = 0;
    size_t match_str_len = 0;
    size_t master_str_len = 0;
    const char *ptr = NULL;

    /* Sanity checks */
    if (!master_str || !match_str)
        return false;

    /* Get match_str length */
    match_str_len = strlen(match_str);
    /* Get master_str length */
    master_str_len = strlen(master_str);

    /* Few checks for optimization */
    if (match_str_len == 0 || master_str_len == 0
        || master_str_len < match_str_len)
        return false;

    /* Identical strings, perfect match!!! */
    if ((master_str_len == match_str_len)
        && (strncmp(master_str, match_str, match_str_len) == 0))
        return true;

    /* Perform match at start */
    if (strncmp(master_str, match_str, match_str_len)) {
        return false; /* No match */
    }

    /* Perform match at end */
    pos = master_str_len - match_str_len;
    ptr = master_str + pos;
    if (strncmp(ptr, match_str, match_str_len)) {
        return false; /* No match */
    }

    /* We are here, perfect match!!!! */
    return true;
}

/*
 * main () -- main function
 */
int
main (int argc, const char **argv)
{
    const char *master_string = NULL;
    const char *match_string = NULL;

    /* Usage check */
    if (argc != 3) {
        fprintf(stderr, "\nUsage: ./a.out <master-string> <match-string>\n");
        return EXIT_FAILURE;
    }

    /* Get input */
    master_string = argv[1];
    match_string = argv[2];

    /* Call the helper */
    fprintf(stdout, "\n\"%s\" %s start and end with \"%s\"\n",
            master_string,
            do_check_string_start_end(master_string,
                                      match_string) ? "DOES" : "DOES NOT",
            match_string);

    return EXIT_SUCCESS;
}

