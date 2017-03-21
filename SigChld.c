/*
 ****************************************************************************** 
 *
 *       Filename:   SigChld.c
 *       Author:     Ram Polisetty (Ram P), polisetty_ram@yahoo.com
 *       Created:    20/Mar/2017
 *       Description: This program intentionally creates a Zombie without
 *       waiting for Child (or not calling wait()) in SIGCHLD handler.
 *
 *       Version:    1.0
 *       Revision:   none
 *       Company:    Self
 *
 ****************************************************************************** 
 */
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

extern const char * const sys_siglist[];

/*
 * handler () -- Signal handler
 */
void handler(int sig_num)
{
    fprintf(stdout, "\nInside signal handler in pid %d "
            "and received signal [%s] i.e. [%s]\n",
            getpid(), strsignal(sig_num), sys_siglist[sig_num]);
    fflush(stdout);
}

/*
 * main() -- main function.
 */
int main()
{
    pid_t pid = 0;

    /* Register for SIGCHLD handler */
    signal(SIGCHLD, handler);

    switch(pid = fork())
    {
        /* Child */
        case 0:
            fprintf(stdout, "\nI am child %d, sleeping for sometime \n", 
                    getpid());
            sleep(10);
            fprintf(stdout, "\nI am child %d, exiting\n", getpid());
            exit(0);

        /* Error */
        case -1:
            perror("\nfork():");
            break; /* This will exit */

        /* Parent */
        default:
            fprintf(stdout, "\nI am parent %d in infinite while loop\n", 
                    getpid());
            fflush(stdout);
            while (1) { 
                continue; /*  :) */
            };
    }

    /* Done ? */
    _exit(EXIT_SUCCESS);
}
