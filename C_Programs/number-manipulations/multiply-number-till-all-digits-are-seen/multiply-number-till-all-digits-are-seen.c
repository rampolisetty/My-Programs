/*
 * multiply-number-till-all-digits-are-seen.c
 *
 * Program which finds the maximum required multiplication of a given number,
 * so that all the digits from 0 - 9 are seen.
 *
 * Example
 * ======
 * Input number : 123
 *
 * Program needs to multiply the number from 1 till X so that all digits from
 * 0 .. 9 are seen. Program needs to print the value of X.
 *
 * 123 when multipled by 1 ; 123 * 1 = 1 2 3 -----> Here digits 1, 2, 3 are seen
 *
 * Next
 *
 * 123 when multipled by 2 ; 123 * 2 = 2 4 6 -----> Here digits 2, 4, 6 are
 * seen and above we have seen 1, 2, 3 already and so we have seen 1, 2, 4, 6.
 *
 * This should continue till all digits 0 .. 9 are seen.
 *
 * Assumption: Asuuming a +ve number as input, so user input will be converted
 * to +ve always.
 *
 * Author: Ram Polisetty
 *
 * How to compile?
 *
 * gcc -g -Wall -Werror multiply-number-till-all-digits-are-seen.c
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

/* 0 .. 9 */
#define MAX_DIGITS 10

static bool
all_digits_seen (uint8_t *num_map, uint8_t num_map_sz)
{
    uint8_t idx = 0;

    /* Sanity checks */
    if (!num_map || num_map_sz <= 0)
        return false;

    /* Traverse the map and find out */
    for (idx = 0; idx < num_map_sz; idx++) {
        if (num_map[idx] == 0)
            return false;
    }

    return true;
}

/*
 * print_num_map () -- A debug function which just prints the num_map. 
 */
static void
print_num_map (uint64_t input_num, uint32_t count,
               uint8_t *num_map, uint8_t num_map_sz)
{
    uint8_t idx = 0;

    /* Sanity checks */
    if (!num_map || num_map_sz <= 0)
        return;

    /* Print map */
    fprintf(stdout, "\nAfter multiplying by %d, the input num becomes %lu and "
            "the map looks like .....\n", count, input_num);
    for (idx = 0; idx < num_map_sz; idx++) {
        fprintf(stdout, "%5d", num_map[idx]);
    }
    fprintf(stdout, "\n");
}

/*
 * extract_digits_and_populate_map () -- Extracs digits from a given numbers
 * and sets their corresponding position in the map.
 *
 * Returns: NONE
 */
static void
extract_digits_and_populate_map (uint64_t input_num,
                                 uint8_t *num_map,
                                 uint8_t num_map_sz)
{
    uint8_t idx = 0;

    /* Sanity checks */
    if (!num_map || num_map_sz <= 0)
        return;

    while (input_num) {
        idx = input_num % 10;
        if (idx < num_map_sz) {
            num_map[idx]++;
        }
        input_num = input_num/10;
    }
}

/*
 * find_req_multiple () -- Function which does the required Job of this
 * Program.
 */
static uint32_t
find_req_multiple (uint64_t input_num)
{
    uint32_t count = 1;
    /* Map (or counter) to remember if digits 0 .. 9 are seen */
    uint8_t num_map[MAX_DIGITS] = {0};

    /* No need to check further if input num is 0 */
    if (input_num == 0)
        return 0;

    /* Do the job, count should be 1 here before loop start */
    do {
        extract_digits_and_populate_map(input_num * count, num_map, MAX_DIGITS);
        /* Debug */
        print_num_map(input_num * count, count, num_map, MAX_DIGITS);

        /* Loop break check, condition met? */
        if (all_digits_seen(num_map, MAX_DIGITS)) {
            break;
        }

        /* Repeat */
        count = count + 1;
    } while (true);

    /* Done */
    return count;
}

/*
 * main () -- main() function
 */
int32_t
main (int32_t argc, const char **argv)
{
    uint64_t input_num = 0;
    uint32_t required_multiple = 0;

    /* Number of args check */
    if (argc != 2) {
        fprintf(stderr,
                "\nInvalid usage: ./a.out <input-positive-integer-num>\n");
        return EXIT_FAILURE;
    }

    /* Reset errno */
    errno = 0;

    /* Convert the input string to input_num */
    input_num = strtoul(argv[1], NULL, 0);
    /* Sanity check */
    if (errno == ERANGE || input_num == 0) {
        fprintf(stderr, "\nInvalid input [%s]\n", argv[1]);
        return EXIT_FAILURE;
    }

    /* Find the required multiple */
    required_multiple = find_req_multiple(input_num);
    fprintf(stdout, "\nThe required multiple is %u by when all "
            "digits are seen!!!!\n", required_multiple);

    /* Done */
    return EXIT_SUCCESS;
}

