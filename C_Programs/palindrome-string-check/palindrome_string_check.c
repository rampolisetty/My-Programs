#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include <err.h>

/*
 * palindrome_check () -- Returns TRUE if the given input string is
 * palindrome.
 */
bool
palindrome_check (const char *str)
{
    int idx = 0, lidx = 0;

    if (!str || !*str)
        return false;

    idx = 0;
    lidx = strlen(str) - 1;

    /* 
     * A string with one character is also treated palindrome by default.
     *
     * Example: "a"
     */
    while (idx < lidx) {
        if (str[idx] != str[lidx]) {
            return false;
        }
        idx++;
        lidx--;
    }

    /* We are here, means proper palindrome */
    return true;
}

#ifndef UNIT_TEST
/*
 * Proper main function
 */
int
main (int argc, char **argv)
{
    if (argc != 2) {
        errx(EXIT_FAILURE, "Please input a string\n");
    }

    if (palindrome_check(argv[1])) {
        fprintf(stdout, "\nThe input string '%s' is palindrome\n", argv[1]);
    } else {
        fprintf(stdout, "\nThe input string '%s' is not palindrome\n", argv[1]);
    }

    return 0;
}
#else
/*
 * Driver program
 */
int
main (int argc, char **argv)
{
    assert(palindrome_check("abc") == 0);
    assert(palindrome_check("abccba") == 1);
    assert(palindrome_check("abcdcba") == 1);
    assert(palindrome_check("racecar") == 1);
}
#endif
