/*
 * Program to demonstrate usage of gcc's __attribute__((constructor))
 *
 * Reference:
 * https://www.cprogramming.com/tutorial/shared-libraries-linux-gcc.html
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

void test_fun_1 (void);
void test_fun_2 (void);
void test_fun_3 (void);
void test_fun_4 (void);
void test_fun_5 (void);

/*
 * main() binary constructor
 */
static void initialize_hooks_main (void) __attribute__((constructor));

/*
 * initialize_hooks_main () --  main() binary constructor
 */
static void
initialize_hooks_main (void)
{
    fprintf(stdout, "\nInitialize hooks - main, called from pid %d\n",
            getpid());
}


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

    /* Call shared lib functions */
    test_fun_1();
    test_fun_2();
    test_fun_3();

    /* Call static lib functions */
    test_fun_4();
    test_fun_5();

    fprintf(stdout, "\nExiting from pid: %d . . .\n", getpid());

    return EXIT_SUCCESS;
} /* ----------  end of function main()  ---------- */

