/*
 * Shared library - 1
 */

#include <stdio.h>
#include <unistd.h>

/* Constructor function */
static void initialize_hooks_1 (void) __attribute__((constructor));

/*
 * initialize_hooks_1 () -- Constructor function
 */
static void
initialize_hooks_1 (void)
{
    fprintf(stdout, "\nInitialize hooks - 1, called from pid %d\n",
            getpid());
}

/*
 * test_fun_1 () -- Dummy function
 */
void
test_fun_1 (void)
{
    fprintf(stdout, "\nCalled %s\n", __FUNCTION__);
    fflush(stdout);
}
