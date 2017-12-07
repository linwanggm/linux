/*
* test
*/
信号的作用是什么？
：：中断正在做的事务，转而去处理信号对应的事务。
信号从产生到事务处理过程都包括哪些？
信号产生  -->
信号注册  -->
信号处理 -->
信号注销

信号处理为异步事件，都有哪些？
kill -l 列出的就是，总共62个
 1) SIGHUP       2) SIGINT       3) SIGQUIT      4) SIGILL       5) SIGTRAP
6) SIGABRT      7) SIGBUS       8) SIGFPE       9) SIGKILL     10) SIGUSR1
11) SIGSEGV     12) SIGUSR2     13) SIGPIPE     14) SIGALRM     15) SIGTERM
16) SIGSTKFLT   17) SIGCHLD     18) SIGCONT     19) SIGSTOP     20) SIGTSTP
21) SIGTTIN     22) SIGTTOU     23) SIGURG      24) SIGXCPU     25) SIGXFSZ
26) SIGVTALRM   27) SIGPROF     28) SIGWINCH    29) SIGIO       30) SIGPWR
31) SIGSYS      34) SIGRTMIN    35) SIGRTMIN+1  36) SIGRTMIN+2  37) SIGRTMIN+3
38) SIGRTMIN+4  39) SIGRTMIN+5  40) SIGRTMIN+6  41) SIGRTMIN+7  42) SIGRTMIN+8
43) SIGRTMIN+9  44) SIGRTMIN+10 45) SIGRTMIN+11 46) SIGRTMIN+12 47) SIGRTMIN+13
48) SIGRTMIN+14 49) SIGRTMIN+15 50) SIGRTMAX-14 51) SIGRTMAX-13 52) SIGRTMAX-12
53) SIGRTMAX-11 54) SIGRTMAX-10 55) SIGRTMAX-9  56) SIGRTMAX-8  57) SIGRTMAX-7
58) SIGRTMAX-6  59) SIGRTMAX-5  60) SIGRTMAX-4  61) SIGRTMAX-3  62) SIGRTMAX-2
63) SIGRTMAX-1  64) SIGRTMAX

哪些是可靠信号？哪些是不可靠信号？有什么区别？
信号1到31均为不可靠信号，之所以不可靠，是由于信号无排队机制。
信号SIGRTMIN到SIGRTMAX之间的信号均为可靠信号。

有关信号处理主要有哪些函数？作用是什么？
   int sigempty(sigset_t *set);                 // 清空信号集set
    int sigfillset(sigset_t *set);                   // 填满信号集，即让set包含所有的信号
    int sigaddset(sigset_t *set, int signo);  // 在set中增加signo信号
    int sigdelset(sigset_t *set, int signo);   // 在set中去掉signo信号
    int sigismember(sigset_t *set, int signo); // 信号signo是否在信号集set中
    int sigprocmask(int how, const sigset_t set, sigset_t oset); 
一个进程的信号屏蔽字规定了当前阻塞而不能递送给该进程的信号集。sigprocmask()可以用来检测或改变目前的信号屏蔽字，其操作依参数how来决定，如果参数oldset不是NULL指针，那么目前的信号屏蔽字会由此指针返回。如果set是一个非空指针，则参数how指示如何修改当前信号屏蔽字。每个进程都有一个用来描述哪些信号递送到进程时将被阻塞的信号集，该信号集中的所有信号在递送到进程后都将被阻塞。
参数how的取值不同，带来的操作行为也不同，该参数可选值如下：
      SIG_BLOCK
              The set of blocked signals is the union of the current set and the set argument.

       SIG_UNBLOCK
              The signals in set are removed from the current set of blocked signals.  It is permissible to attempt to unblock a signal which is not blocked.

       SIG_SETMASK
              The set of blocked signals is set to the argument set.

       If oldset is non-NULL, the previous value of the signal mask is stored in oldset.

       If set is NULL, then the signal mask is unchanged (i.e., how is ignored), but the current value of the signal mask is nevertheless returned in oldset (if it
       is not NULL).

       The use of sigprocmask() is unspecified in a multithreaded process; see pthread_sigmask(3).

    int sigpengding(sigset_t *set);  // 该函数返回信号集，该信号通过set参数返回。
   以上函数都在#include <signal.h> 头文件中。

写程度验证：
1. 对程序发送信号，输出信号对应的值及对应的信号名

root@iZ232ngsvp8Z:~/tmp# ./psignal
process id is 18723
^Csignal: 2 Interrupt  //ctrl +c 
^\signal: 3 Quit        // ctrl + \
signal: 1 Hangup      // 如下 kill -SIGHUP pid

root@iZ232ngsvp8Z:~# pidof psignal
18723
root@iZ232ngsvp8Z:~# kill -SIGHUP 18723
-------------------------start------------------------------------
root@iZ232ngsvp8Z:~/tmp# cat psignal.c
#include <stdio.h>
#include <signal.h>

int i_tmp=0;
/*
* print signal ID
*/
void sig_handle(int sig)
{ 
    i_tmp++;
    printf("signal: %d %s\n",sig,sys_siglist[sig]);

}


int main()
{
//print process pid
printf("process id is %d\n",getpid());
fflush(stdout);

signal(SIGHUP,sig_handle);
signal(SIGHUP,sig_handle);
signal(SIGINT,sig_handle);
signal(SIGQUIT,sig_handle);
while(i_tmp!=3); 
return 0;
}
--------------------------end-----------------------------------
printf("%s\n", strsignal(SIGUSR1));      //函数
printf("%s\n", sys_siglist[SIGUSR1]);  //数组

还有类似上面的函数


2.信号的屏蔽
sigaddset/sigprocmask(SIG_BLOCK/SIG_UNBLOCK)

root@iZ232ngsvp8Z:~/tmp# ./signal2
SIGINT was masked, SIGQUIT was registered!
^C^C^C^C^C^C^C
^C^C^C^C
^\in quit handler
int handler 2
SIGINT was unmasked, pauseing .........
^\in quit handler
-------------------------start------------------------------------
root@iZ232ngsvp8Z:~/tmp# cat signal2.c 
/*
* mask the SIGINT,until snd signal SIGQUIT  
*/
#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <stdbool.h>  
#include <signal.h>  
#include <sys/types.h>  
#include <errno.h>  
#include <string.h>  
 
int  flag =  true ; 
 
void  int_handler(  int  signum ) 
{ 
   printf("int handler %d\n" ,signum); 
} 
 
void  quit_handler(  int  signum ) 
{ 
   flag = false ; 
   printf("in quit handler\n" ); 
} 
 
int  main( int  argc,  char  **argv) 
{ 
   char  choice; 
   sigset_t mask; 
 
 
   if  ( signal(SIGINT, int_handler) == SIG_ERR ){ 
      printf("sigaction failed!\n" ); 
      return  -1; 
   } 
 
   if  ( signal(SIGQUIT, quit_handler) == SIG_ERR ){ 
      printf("sigaction failed!\n" ); 
      return  -1; 
   } 
 
 
   sigemptyset(&mask); 
   if  ( -1 == sigaddset(&mask,SIGINT)){ 
      printf("add SIGINT to mask failed!\n" ); 
      return  -1; 
   } 
 
   if  ( -1 == sigprocmask(SIG_BLOCK,&mask,NULL) ){ 
      printf("sigprocmask failed!\n" ); 
      return  -1; 
   } 
 
   printf("SIGINT was masked, SIGQUIT was registered!\n" ); 
   while (flag){ 
      pause(); 
   } 
 
   if  ( -1 == sigprocmask(SIG_UNBLOCK,&mask,NULL) ){ 
      printf("sigprocmask failed!\n" ); 
      return  -1; 
   } 
 
   printf("SIGINT was unmasked, pauseing .........\n" ); 
   pause(); 
 
   return  0; 
}

 -------------------------end------------------------------------

会发现一直按CTRL+C都不会起作用，直到发送信号SIGQUIT ，后面调用SIG_UNBLOCK解除信号屏蔽


3. 在指定时间内对信号屏蔽，如果该信号来则随后对信号进行处理
-------------------------start------------------------------------
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void handler(int sig)

{
   printf("Deal SIGINT\n");  //SIGINT信号处理函数
}
int main()
{
sigset_t newmask;
sigset_t oldmask;
sigset_t pendmask;

struct sigaction act;
act.sa_handler = handler;  //handler为信号处理函数首地址
sigemptyset(&act.sa_mask);
act.sa_flags = 0;
sigaction(SIGINT, &act, 0);  //信号捕捉函数，捕捉Ctrl+C
sigemptyset(&newmask);//初始化信号量集
sigaddset(&newmask, SIGINT);//将SIGINT添加到信号量集中

sigprocmask(SIG_BLOCK, &newmask, &oldmask);//将newmask中的SIGINT阻塞掉，并保存当前信号屏蔽字到Oldmask
sleep(5);//休眠5秒钟，说明:在5s休眠期间，任何SIGINT信号都会被阻塞，如果在5s内收到任何键盘的Ctrl+C信号，则此时会把这些信息存在内核的队列中，等待5s结束后，可能要处理此信号。
sigpending(&pendmask);//检查信号是悬而未决的,

if (sigismember(&pendmask, SIGINT))//SIGINT是悬而未决的。所谓悬而未决，是指SIGINT被阻塞还没有被处理
{
printf("/nSIGINT pending\n");
}
sigprocmask(SIG_SETMASK, &oldmask, NULL);//恢复被屏蔽的信号SIGINT

//此处开始处理信号，调用信号处理函数
printf("SIGINT unblocked\n");

return (0);

}
-------------------------end------------------------------------
http://blog.sina.com.cn/s/blog_6af9566301013w66.html


-------------------------start------------------------------------
root@iZ232ngsvp8Z:~/tmp# cat signal8.c
#include <unistd.h>

#include <signal.h>

#include <stdio.h>



void handler(int sig)    //信号处理函数的实现

{

   printf("SIGINT sig\n");

}

int main()

{

    sigset_t new,old;

    struct sigaction act;

    act.sa_handler = handler;  //信号处理函数handler

    sigemptyset(&act.sa_mask);

    act.sa_flags = 0;

    sigaction(SIGINT, &act, 0);  //准备捕捉SIGINT信号

    sigemptyset(&new);

    sigaddset(&new, SIGINT);

    sigprocmask(SIG_BLOCK, &new, &old);  //将SIGINT信号阻塞，同时保存当前信号集

    printf("SIGINT Blocked\n");
    sleep(5);
    sigprocmask(SIG_SETMASK, &old, NULL);  //取消阻塞
    pause();

    return 0;

}
-------------------------end------------------------------------

root@iZ232ngsvp8Z:~/tmp# ./signal8
SIGINT Blocked
^C^C^C^C^C^C^C^C^C^C
SIGINT sig
^CSIGINT sig

http://blog.sina.com.cn/s/blog_6af9566301013xp4.html


-------------------------start------------------------------------
#include <unistd.h>

#include <signal.h>

#include <stdio.h>

void handler(int sig)   //信号处理程序

{

   if(sig == SIGINT)

      printf("SIGINT sig");

   else if(sig == SIGQUIT)

      printf("SIGQUIT sig");

   else

      printf("SIGUSR1 sig");

}



int main()

{

    sigset_t new,old,wait;   //三个信号集

    struct sigaction act;

    act.sa_handler = handler;

    sigemptyset(&act.sa_mask);

    act.sa_flags = 0;

    sigaction(SIGINT, &act, 0);    //可以捕捉以下三个信号：SIGINT/SIGQUIT/SIGUSR1

    sigaction(SIGQUIT, &act, 0);

    sigaction(SIGUSR1, &act, 0);

  

    sigemptyset(&new);

    sigaddset(&new, SIGINT);  //SIGINT信号加入到new信号集中

    sigemptyset(&wait);

    sigaddset(&wait, SIGUSR1);  //SIGUSR1信号加入wait

    sigprocmask(SIG_BLOCK, &new, &old);       //将SIGINT阻塞，保存当前信号集到old中

  

    //临界区代码执行   

 

    if(sigsuspend(&wait) != -1)  //程序在此处挂起；用wait信号集替换new信号集。即：过来SIGUSR1信  号，阻塞掉，程序继续挂起；过来其他信号，例如SIGINT，则会唤醒程序。执行sigsuspend的原子操作。注意：如果“sigaddset(&wait, SIGUSR1);”这句没有，则此处不会阻塞任何信号，即过来任何信号均会唤醒程序。

        printf("sigsuspend error");

    printf("After sigsuspend");

    sigprocmask(SIG_SETMASK, &old, NULL);

    return 0;

}

-------------------------end------------------------------------
http://blog.sina.com.cn/s/blog_6af9566301013xp4.html

















