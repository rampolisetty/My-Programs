/*
 * Shared library - 1
 */

#include <stdio.h>
#include <unistd.h>

/* Constructor function */
static void initialize_hooks_1 (void) __attribute__((constructor));

extern int static_data;

/*
 * initialize_hooks_1 () -- Constructor function
 */
static void
initialize_hooks_1 (void)
{
    static_data += 1;
    fprintf(stdout, "\nInitialize hooks - 1, called from pid %d\n",
            getpid());
}

/*
 * test_fun_1 () -- Dummy function
 */
void
test_fun_1 (void)
{
    fprintf(stdout, "\nCalled %s and static data %d\n", 
            __FUNCTION__, static_data);
    fflush(stdout);
}
