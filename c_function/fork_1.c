/*
 *  sleep given time, then start up given process to execute job
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/prctl.h>

#define bool int

static bool do_parent_job();
static bool do_child_job();

int main(int argc, char *argv[])
{
    pid_t pid;
    pid_t waitPid;
    int exitStatus;
    int i=0;

    while (i++ < 10)
    {
        pid = fork();
        if (pid > 0)
        {
            /* parent process */
            fprintf(stdout, "this is parent process, pid = %d\n", getpid());
            //(void)do_parent_job();

            /* kill child */
            //kill(pid, SIGINT);

            /* wait child process exit */
            //waitPid = wait(&exitStatus);
            //fprintf(stdout, "exit child pid = %d, status = %d\n", waitPid, exitStatus);        
        }
        else if (pid == 0)
        {
            /* this is child process */
            fprintf(stdout, "this is child process, pid = %d\n", getpid());
            (void)do_child_job();
        }
        else
        {
            fprintf(stderr, "fork error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

    }
    wait();

    return 1;
}

/*
 *  do the parent job
 * 
 */
static bool
do_parent_job()
{
    usleep(13 * 1000000L);

    return 1;
}

/*
 * do the child job
 * 
 */
static bool
do_child_job()
{
    usleep(17 * 1000000L);
    exit(EXIT_SUCCESS);

    return 1;
}
