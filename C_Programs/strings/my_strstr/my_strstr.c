/*
 * my_strstr.c -- My own implementation of strstr() libc function.
 *
 * Author: Ram Polisetty
 *
 * How to run the program?
 *
 * gcc -Wall -Werror my_strstr.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

/*
 * my_strstr () -- Helper function which implements strstr() equivalent.
 *
 * Returns: Pointer to sub_string if found;
 *          NULL otherwise.
 */
const char *
my_strstr (const char *main_str, const char *sub_str)
{
    const char *main_str_cur_pos = NULL;
    const char *sub_str_ptr = NULL;

    if (!main_str || !sub_str)
        return NULL;

    while (*main_str) {

        /* Cache current main_str pos */
        main_str_cur_pos = main_str;
        /* Cache the sub_str */
        sub_str_ptr = sub_str;

        /*
         * Perform match
         */
        while (*main_str && *sub_str_ptr && (*main_str == *sub_str_ptr)) {
            sub_str_ptr++;
            main_str++;
        }

        /* Check if we matched */
        if (*sub_str_ptr == '\0') {
            return main_str_cur_pos;
        }

        /* Move to next */
        main_str = main_str_cur_pos + 1;
    }

    /* We reach here, no match!!! */
    return NULL;
}

/*
 * main () -- main () function.
 */
int32_t
main (int32_t argc, const char **argv)
{
    const char *sub_str_ptr = NULL;

    if (argc != 3) {
        fprintf(stderr, "\nUsage: ./a.out \"main-string\" \"sub-string\"\n");
        return EXIT_FAILURE;
    }

    /* Call the helper */
    sub_str_ptr = my_strstr(argv[1], argv[2]);
    fprintf(stdout,
            "\nIs the sub-string [%s] found in main-string [%s]? - [%s]\n\n",
            argv[2], argv[1], sub_str_ptr ? "YES" : "NO");

    /* Done */
    return EXIT_SUCCESS;
}

