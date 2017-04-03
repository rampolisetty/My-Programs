/*
 * POLLHUP.c - An example program which shows how to handle POLLHUP.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <poll.h>
#include <sys/wait.h>

#define PIPE_READ_IDX 0
#define PIPE_WRITE_IDX 1

/*
 * 
 */
static void
clean_zombie (pid_t pid)
{
    int rc = 0;
    int status = 0;
    unsigned short count = 0;

    /* We are interested only in our children */
    if (pid > 0) {
        /* Send SIGTERM if it is still running */
        kill(pid, SIGTERM);

        /* Wait for child */
        do {
            if (count > 2) {
                /*
                 * We had given 3 seconds to child to terminate. If it did not
                 * terminate kill it.
                 */
                kill(pid, SIGKILL);
            }
            count++;
            /* Do not hang */
            rc = waitpid(pid, &status, WNOHANG);
            sleep(1);
            /* No need to log child status. */
        } while (rc < 0 && errno == EINTR); /* We need to continue */
    }
}

int main (int argc, char **argv)
{
    int pipe_fd[2] = {-1, -1};
    struct pollfd poll_fd = {0};
    pid_t child_pid = -1;

    /* Open a pipe */
    if (pipe(pipe_fd) < 0) {
        perror("\nUnable to open pipe:");
        fprintf(stderr, "\n");
        exit(EXIT_FAILURE);
    }

    switch (child_pid = fork()) {

        /* Child */
        case 0:
            fprintf(stdout, "\nChild pid: %d\n", getpid());
            fflush(stdout);
            /* Child writes to parent something. So close read end */
            if (close(pipe_fd[PIPE_READ_IDX]) < 0) {
                perror("\nUnable to close pipe read end in child");
            } else {
                fprintf(stdout, "\nChild closed read end successfully\n");
                fflush(stdout);
            }

            /* 
             * Child will sleep for 10 secs after that it will close its write
             * end and sleep for few more secs and exits
             */
            fprintf(stdout, "\nChild going to sleep\n");
            fflush(stdout);
            sleep(10);

            fprintf(stdout, "\nChild after going to close write end\n");
            fflush(stdout);
            close(pipe_fd[PIPE_WRITE_IDX]);

            sleep(10);
            fprintf(stdout, "\nChild exiting\n");
            fflush(stdout);

            break;

        /* Error */
        case -1:
            perror("\nfork:");
            fprintf(stderr, "\n");
            exit(EXIT_FAILURE);
            break;

        /* Parent */
        default:
            fprintf(stdout, "\nParent pid: %d\n", getpid());
            fflush(stdout);
            /* Parent will read, so it will close its write end */
            if (close(pipe_fd[PIPE_WRITE_IDX]) < 0) {
                perror("\nUnable to close parent pipe write end");
                fprintf(stderr, "\n");
            } else {
                fprintf(stdout, "\nParent closed write end successfully\n");
                fflush(stdout);
            }
            /* Poll for data from child */
            bzero(&poll_fd, sizeof(struct pollfd));

            poll_fd.fd = pipe_fd[PIPE_READ_IDX];
            poll_fd.events = POLLIN; /* Interested for input only */

            /* Now poll() for events no timeout */
            if (poll(&poll_fd, 1, -1) < 0) {
                perror("\nUnable to poll:");
                fprintf(stderr, "\n");
                _exit(EXIT_FAILURE);
            }

            /*
             * Check the returned events
             */
            switch (poll_fd.revents) {
                case POLLIN:
                    fprintf(stdout, "\nParent received POLLIN\n");
                    fflush(stdout);
                    break;
                case POLLHUP:
                    fprintf(stdout, "\nParent received POLLHUP\n");
                    fflush(stdout);
                    clean_zombie(child_pid);
                    break;
                default:
                    clean_zombie(child_pid);
                    break;
            }
            break;
            
    }

    return 0;
}
