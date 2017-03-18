时间间隔在linux系统函数中可以达到microseconds，不过是否可以达到微妙还需要看硬件系统是否支持。
时间间隔可以使用：
sleep(int)  --秒级
usleep(int)  --微秒级
select      --微秒级
alarm       --秒级
settimer    --微秒级

/*
*select
*/

#include <stdio.h>
#include <errno.h>
#include <sys/time.h>

void microseconds_sleep(unsigned int us)
{
    struct timeval tv;
    tv.tv_sec = us/1000000;
    tv.tv_usec = us%1000000;
    int err;
    do
    {
        err = select (0, NULL, NULL, NULL, &tv);
    }while (err<0 && err==EINTR); 
}

int main()
{
        int i = 0;

        while (i++ < 100)
        {
                printf("%d\n", i);
                microseconds_sleep(10);
        }

        return 0;
}

/*
* alarm
*/
#include <stdio.h>
#include <signal.h> 
#include <sys/time.h>

#include <errno.h>

void sigFunc()
{
   static int iCnt = 0;
   printf("The %d Times: Hello world\n", iCnt++);
}

int main(void)
{
   struct itimerval tv, otv;
   signal(SIGALRM, sigFunc);
   //how long to run the first time
   tv.it_value.tv_sec = 3;
   tv.it_value.tv_usec = 0;
   //after the first time, how long to run next time
   tv.it_interval.tv_sec = 1;
   tv.it_interval.tv_usec = 0;

   if (setitimer(ITIMER_REAL, &tv, &otv) != 0)
        printf("setitimer err %d\n", errno);

   while(1)
   {
        sleep(1);
   }
}
