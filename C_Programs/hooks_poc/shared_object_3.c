/*
 * Shared library - 3
 */

#include <stdio.h>
#include <unistd.h>

/* Constructor function */
static void initialize_hooks_3 (void) __attribute__((constructor));

extern int static_data;

/*
 * initialize_hooks_3 () -- Constructor function
 */
static void
initialize_hooks_3 (void)
{
    static_data += 3;
    fprintf(stdout, "\nInitialize hooks - 3, called from pid %d\n",
            getpid());
}

/*
 * test_fun_3 () -- Dummy function
 */
void
test_fun_3 (void)
{
    fprintf(stdout, "\nCalled %s and static data %d\n", 
            __FUNCTION__, static_data);
    fflush(stdout);
}
