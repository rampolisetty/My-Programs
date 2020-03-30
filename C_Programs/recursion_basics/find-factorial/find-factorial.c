/*
 * Author: Rama Polisetty 
 *
 * find-factorial.c 
 *
 * A simple C program that demonstrates the usage of Recursion.
 *
 * This program finds the factorial of a given number (user input number).
 *
 * Factorial of n is : n! = n * (n - 1)!
 *
 * Example
 * =======
 *
 * 4! = 4 * (4 - 1)!
 *      4 * 3!
 *      4 * 3 * (3 - 1)!
 *      4 * 3 * 2!
 *      4 * 3 * 2 * (2 - 1)!
 *      4 * 3 * 2 * 1!
 *      4 * 3 * 2 * 1
 *   = 24
 */

#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>

/*
 * find_factorial () -- The recursive function, which computes and returns the
 * factorial of the given number.
 */
static unsigned long long
find_factorial (unsigned long long input_num)
{
    /* Recursion base case */
    if (input_num == 0 || input_num == 1)
        return 1;

    /* Recursive case; smaller tasks */
    return input_num * find_factorial(input_num - 1);

}

/*
 * main () -- main function.
 */
int
main (int32_t argc, const char **argv)
{
    unsigned long long input_num = 0;
    unsigned long long factorial = 0;

    if (argc != 2) {
        errx(EXIT_FAILURE, "Usage: ./a.out <input-any-format-number>\n");
    }

    /* Perform the conversion */
    input_num = strtoull(argv[1], NULL, 0);

    /* Perform range check/error check */
    if (input_num == ULLONG_MAX && errno == ERANGE) {
        errx(EXIT_FAILURE, "Abort: Input number is Out Of Range\n");
    } else {
        /* A debug msg is helpful */
        fprintf(stdout, "\nDebug: The given number is (converted to decimal): %llu\n", input_num);

        /* Do the work */
        factorial = find_factorial(input_num);
        if (factorial == 0) {
            fprintf(stdout, "\nInput number is too big to determine a factorial\n");
        } else {
            fprintf(stdout, "\n%llu! is: %llu\n", input_num, factorial);
        }
    }

    return EXIT_SUCCESS;
}

