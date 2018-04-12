/*
 * Shared library - 2
 */

#include <stdio.h>
#include <unistd.h>

/* Constructor function */
static void initialize_hooks_2 (void) __attribute__((constructor));

extern int static_data;

/*
 * initialize_hooks_2 () -- Constructor function
 */
static void
initialize_hooks_2 (void)
{
    static_data += 2;
    fprintf(stdout, "\nInitialize hooks - 2, called from pid %d\n",
            getpid());
}

/*
 * test_fun_2 () -- Dummy function
 */
void
test_fun_2 (void)
{
    fprintf(stdout, "\nCalled %s and static_data %d\n", 
            __FUNCTION__, static_data);
    fflush(stdout);
}
