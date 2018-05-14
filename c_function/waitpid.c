#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int main( void )
{
    pid_t childpid, pr;
    int status;

    childpid = fork();

    if ( -1 == childpid )
    {
        perror( "fork()" );
        exit( EXIT_FAILURE );
    }
    else if ( 0 == childpid )
    {
        puts( "In child process" );
        sleep( 1 );           //sleep 3
        printf("\tchild pid = %d\n", getpid());
        printf("\tchild ppid = %d\n", getppid());
        exit(EXIT_SUCCESS);
    }
    else
    {
        //wait(&status);
        pr=waitpid( childpid, &status, 0 );
        if(WIFEXITED(status))     //if WIFEXITED returns non-0
        {
          printf("the child process %d exit normally.\n",pr);
          printf("the return code is %d.\n",WEXITSTATUS(status));
        }
        else
        {
          printf("the child process %d exit abnormally.\n",pr);
        }
        puts( "in parent" );
        printf( "\tparent pid = %d\n", getpid() );
        printf( "\tparent ppid = %d\n", getppid()  );
        printf( "\tchild process exited with status %d \n", status );
    }
    exit(EXIT_SUCCESS);
}

----
wln@iZ232ngsvp8Z:~/2016/c> ./waitpid
In child process
        child pid = 2464
        child ppid = 2463
the child process 2464 exit normally.
the return code is 0.
in parent
        parent pid = 2463
        parent ppid = 2158
        child process exited with status 0
        
