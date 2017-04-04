/*
 ****************************************************************************** 
 *
 *       Filename:   thread_local_storage.c
 *       Author:     Ram Polisetty (Ram P), polisetty_ram@yahoo.com
 *       Created:    05/07/2016 05:06:04 PM
 *       Description:  
 *
 *       Version:    1.0
 *       Revision:   none
 *       Company:    Self
 *
 ****************************************************************************** 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/* Remove __thread attribute here and test */
static __thread const char *g_name;

void * 
routine(void *args __attribute__((unused)))
{
    printf("\nReached thread %lu and value of global variable [%s]\n", 
           pthread_self(), g_name ? : "null");
    fflush(stdout);

    g_name = "ramas";

    printf("\nThread %lu modified value of global variable to [%s]\n", 
           pthread_self(), g_name);
    fflush(stdout);

    while (1) {
        continue;
    }
}

int 
main(int argc, char **argv)
{
    int idx = 0;
    pthread_t tid;

    if (argc != 2) {
        fprintf(stderr, "\nInvalid arguments\n");
    }
    else {
        for (idx = 0; idx < atoi(argv[1]); idx++) {
            fprintf(stdout, "\nCreating thread num %d\n", idx + 1);
            fflush(stdout);
            pthread_create(&tid, NULL, routine, NULL);
            sleep(3);
        }
    }

    fprintf(stdout, "\nMain thread waiting in inifinite loop after creating "
            "threads. Press Ctrl + C to exit\n");
    fflush(stdout);

    /* No Join etc, this is just a sample program */
    while (1) {
        continue;
    }
    
    return 0;
}
