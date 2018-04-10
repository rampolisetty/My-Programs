/*
 * Shared library - 3
 */

#include <stdio.h>
#include <unistd.h>

/* Constructor function */
static void initialize_hooks_3 (void) __attribute__((constructor));

/*
 * initialize_hooks_3 () -- Constructor function
 */
static void
initialize_hooks_3 (void)
{
    fprintf(stdout, "\nInitialize hooks - 3, called from pid %d\n",
            getpid());
}

/*
 * test_fun_3 () -- Dummy function
 */
void
test_fun_3 (void)
{
    fprintf(stdout, "\nCalled %s\n", __FUNCTION__);
    fflush(stdout);
}
