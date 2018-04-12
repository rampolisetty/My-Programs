/*
 * .a library 1 
 */

#include <stdio.h>
#include <unistd.h>

/* Constructor function */
static void initialize_hooks_4 (void) __attribute__((constructor));

extern int static_data;

/*
 * initialize_hooks_4 () -- Constructor function
 */
static void
initialize_hooks_4 (void)
{
    static_data += 4;
    fprintf(stdout, "\nInitialize hooks (for .a) - 4, called from pid %d\n",
            getpid());
}

/*
 * test_fun_4 () -- Dummy function
 */
void
test_fun_4 (void)
{
    fprintf(stdout, "\nCalled %s and static data %d\n", 
            __FUNCTION__, static_data);
    fflush(stdout);
}

