/*
 * .a library 2 
 */

#include <stdio.h>
#include <unistd.h>

/* Constructor function */
static void initialize_hooks_5 (void) __attribute__((constructor));

int static_data;

/*
 * initialize_hooks_5 () -- Constructor function
 */
static void
initialize_hooks_5 (void)
{
    static_data += 5;
    fprintf(stdout, "\nInitialize hooks (for .a) - 5, called from pid %d\n",
            getpid());
}

/*
 * test_fun_5 () -- Dummy function
 */
void
test_fun_5 (void)
{
    fprintf(stdout, "\nCalled %s and static data %d\n", 
            __FUNCTION__, static_data);
    fflush(stdout);
}

