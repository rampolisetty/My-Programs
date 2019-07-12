/*
 * common-char-in-n-strings.c
 *
 * Author: Ram Polisetty
 *
 * Common characters in n strings
 * ==============================
 * Given n strings, find the common characters in all the strings. In simple
 * words, find characters that appear in all the strings and display them in
 * alphabetical order or lexicographical order.
 *
 *  Input :  geeksforgeeks
 *           gemkstones
 *           acknowledges
 *           aguelikes
 *
 *  Output : e g k s
 *
 *  Input :  apple 
 *           orange
 *
 *  Output : a e
 *
 * How to compile?
 *
 * gcc -g -Wall -Werror common-char-in-n-strings.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define NUM_ASCII_CHAR 256

/*
 * display_common_char_in_strings () -- Function which does the required Job
 * of this program i.e. to display common characters in given strings.
 *
 * Returns: NONE
 */
static size_t
display_common_char_in_strings (const char **input_strings)
{
    int32_t idx = 0;
    size_t num_repeat = 0;
    bool result_array[NUM_ASCII_CHAR];
    bool mark_array[NUM_ASCII_CHAR];
    unsigned char c = '\0';
    const char *ptr = NULL;

    /* Safe NULL check */
    if (!input_strings)
        return num_repeat;

    /* Boundary checks */
    if (input_strings[0] == NULL || input_strings[1] == NULL) {
        fprintf(stderr, "\nNumber of input strings should be at least 2\n");
        return num_repeat;
    }

    /* Initialize result_array with all 'true's */
    memset(result_array, true, sizeof(result_array));

    /*
     * Do the job
     */
    for (idx = 0; input_strings[idx]; idx++) {

        /* Get a temporary ptr/accessor to current string */
        ptr = input_strings[idx];
        /* print the same */
        fprintf(stdout, "Input string \"%s\"\n", ptr);

        /* Initialize presence array with all 'false's */
        memset(mark_array, false, sizeof(mark_array));

        /* Travese each char in ptr and mark its corresponding pos as true */
        while (*ptr) {
            c = *ptr;
            if(result_array[c]) {
                mark_array[c] = true;
            }
            ptr++;
        }

        /* Copy mark array to result array */
        memcpy(result_array, mark_array, sizeof(result_array));
    }

    /*
     * Traverse result_array
     */
    for (idx = 0; idx < NUM_ASCII_CHAR; idx++) {
        if (result_array[idx]) {
            fprintf(stdout, "\nChar %c is repeated\n", idx);
            num_repeat++;
        }
    }

    return num_repeat;
}

/*
 * main () -- main function
 */
int32_t
main (int32_t argc, char **argv)
{
    const char *input_strings[] = {"geeksforgeeks", "gemkstones",
                                   "acknowledges", "aguelikes",
                                   NULL
                                  };
    /* Call the helper */
    fprintf(stdout, "\nNum repeat char: %ld\n",
            display_common_char_in_strings(input_strings));

    return EXIT_SUCCESS;
}
