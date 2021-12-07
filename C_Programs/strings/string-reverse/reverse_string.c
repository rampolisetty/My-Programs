/*
 * Author: Ram Polisetty
 *
 * reverse_string.c - Implements the string reversal function in O(n/2).
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define DEBUG 1

/*
 * reverse_string () -- Reverses the given string. The caller has to release
 * the char pointer returned by this function. The returned char buf will have
 * the reversed string.
 */
char *
reverse_string (const char *input)
{
    uint32_t idx = 0;
    size_t len = 0;
    char tmp = 0;
    char *start = NULL;
    char *end = NULL;
    char *rev_str = NULL;

    if (!input)
        return NULL;

    len = strlen(input);
    if (len == 0) {
        return NULL;
    }

    /* Duplicate the input string and reverse it */
    rev_str = strdup(input);

    /*
     * O(n/2) logic. Perform the swapping.
     */
    start = rev_str;
    end = rev_str + len - 1;
    while (idx < len/2) {
#ifdef DEBUG
        fprintf(stdout, "\nDEBUG: Swapping %c and %c\n",
                start[idx], *end);
#endif
        tmp = start[idx];
        start[idx] = *end;
        *end = tmp;
        idx++;
        end--;
    }

#ifdef DEBUG
    fprintf(stdout, "\nDEBUG: Reversed string '%s'\n",
            rev_str);
#endif

    return rev_str;
}

int
main (int argc, char *argv[])
{
    char *rev_str = NULL;

    if (argc != 2) {
        fprintf(stderr, "\nUsage: ./a.out <string>\n");
        exit(EXIT_FAILURE);
    }

    /* Call helper */
    rev_str = reverse_string(argv[1]);
    if (rev_str) {
        fprintf(stdout, "\nThe reversed string is '%s'\n", rev_str);
        free(rev_str);
    } else {
        fprintf(stderr, "\nInvalid input received\n");
    }

    return 0;
}

