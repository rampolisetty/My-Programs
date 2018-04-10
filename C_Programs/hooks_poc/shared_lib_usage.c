/*
 * Program to demonstrate usage of gcc's __attribute__((constructor))
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

void test_fun_1 (void);
void test_fun_2 (void);
void test_fun_3 (void);

/* 
 *+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=
 *  Name:   main()
 *  Params: input: 
 *               argc: argument count, 
 *               argv: argument vector
 *          output:
 *                exit status
 *          inout: NONE
 *  Description: main() function, starting point of program
 *+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=
 */
int main (int argc, char *argv[])
{
    fprintf(stdout, "\nInside main and my pid: %d\n", getpid());
    fflush(stdout);

    test_fun_1();
    test_fun_2();
    test_fun_3();

    fprintf(stdout, "\nExiting from pid: %d . . .\n", getpid());
    return EXIT_SUCCESS;
} /* ----------  end of function main()  ---------- */

