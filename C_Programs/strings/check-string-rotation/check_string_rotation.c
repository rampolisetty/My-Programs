/*
 * Author: Ram Polisetty
 *
 * A program to check if given two strings are rotation of each other.
 *
 * Ex: given s1 = ABCD and s2 = CDAB, return true, given s1 = ABCD, 
 * and s2 =  ACBD , return false
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <alloca.h>

/*
 * check_string_rotation () -- Helper function to check if given two strings
 * are rotation of each other. 
 *
 * Returns :
 *         TRUE if so
 *         FALSE if not.
 */
static bool
check_string_rotation (const char *red, const char *blue)
{
    size_t red_len = 0;
    size_t blue_len = 0;
    size_t buf_sz = 0;
    char *buf = NULL;

    /* Sanity check */
    if (!red || !blue)
        return false;

    /* Determine length of input strings */
    red_len = strlen(red);
    blue_len = strlen(blue);

    /* Skip empty strings */
    if (!red_len || !blue_len)
        return false;

    /* They are not rotate strings, if the length is not equal */
    if (red_len != blue_len)
        return false;

    /*
     * Now perform the logic to check for rotation.
     */
    buf_sz = (2 * red_len) + 1;
    buf = alloca(buf_sz);
    assert(buf != NULL);

    /* Concatenate the first string with itself*/
    snprintf(buf, buf_sz, "%s%s", red, red);

    /* Perform the required check */
    return (strstr(buf, blue));
}

/*
 * main () function
 */
int
main (int argc, char **argv)
{
    /* We need two arguments to program */
    if (argc < 3) {
        fprintf(stdout, "\nUsage: ./check_string_rotation "
                "<string-1> <string-2> \n");
    } else {
        if (check_string_rotation(argv[1], argv[2])) {
            fprintf(stdout, "\n'%s' and '%s' are rotation of each other\n",
                    argv[1], argv[2]);
        } else {
            fprintf(stdout, "\n'%s' and '%s' are not rotation of each other\n",
                    argv[1], argv[2]);
        }
    }

    return EXIT_SUCCESS;
}

