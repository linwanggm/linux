#include<sys/types.h> 
#include<sys/sysinfo.h> 
#include <sys/time.h>
#include<unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <iostream>
using namespace std;

#define ENABLE_WHCIH_CPU
#define ENABLE_SET_CPU

#define EXEC_COUNT (100 * 1000 * 1000)

struct  bits_t
{
int a;
    char            placeholder[64];
int b;
};

struct bits_t bits;

int which_cpu(const char* prefix_)
{
    #ifdef ENABLE_WHCIH_CPU
cpu_set_t cur_cpu;
CPU_ZERO(&cur_cpu);
if (sched_getaffinity(0, sizeof(cur_cpu), &cur_cpu) == -1) 
{ 
printf("warning: cound not get cpu affinity, continuing...\n"); 
return -1;
} 

int num = sysconf(_SC_NPROCESSORS_CONF);
for (int i = 0; i < num; i++) 
{ 
if (CPU_ISSET(i, &cur_cpu)) 
{ 
printf("[%s] this process %d is running processor : %d\n", prefix_, getpid(), i); 
} 
} 
    #endif

return 0;
}

int set_cpu(int cpu_id_)
{
    #ifdef ENABLE_SET_CPU
cpu_set_t mask;
CPU_ZERO(&mask); 
CPU_SET(cpu_id_, &mask); 
if (sched_setaffinity(0, sizeof(mask), &mask) == -1) 
{ 
printf("warning: could not set CPU affinity, continuing...\n"); 
return -1;
    } 
    #endif

return 0;
}

void* thd_func1(void* arg_)
{
set_cpu(0);
which_cpu("thread 1 start");
    timeval begin_tv;
    gettimeofday(&begin_tv, NULL);

    for (int i = 0; i < EXEC_COUNT; i++)
    {
        bits.a += 1;
        int a = bits.a;
    }

    timeval end_tv;
    gettimeofday(&end_tv, NULL);
    printf("thd1 perf:[%lu]us\n", (end_tv.tv_sec * 1000 * 1000 + end_tv.tv_usec) - (begin_tv.tv_sec * 1000 * 1000 + begin_tv.tv_usec));
which_cpu("thread 1 end");

    return NULL;
}

void* thd_func2(void* arg_)
{
set_cpu(1);
which_cpu("thread 2 start");
    timeval begin_tv;
    gettimeofday(&begin_tv, NULL);

    for (int i = 0; i < EXEC_COUNT; i++)
    {
        bits.b += 2;
        int b = bits.b;
    }

    timeval end_tv;
    gettimeofday(&end_tv, NULL);
    printf("thd2 perf:[%lu]us\n", (end_tv.tv_sec * 1000 * 1000 + end_tv.tv_usec) - (begin_tv.tv_sec * 1000 * 1000 + begin_tv.tv_usec));
which_cpu("thread 2 end");

    return NULL;
}


int main(int argc_, char* argv_[])
{
int num = sysconf(_SC_NPROCESSORS_CONF);
printf("system has %d processor(s).\n", num);
cpu_set_t cpu_mask;
cpu_set_t cur_cpu_info;

memset((void*)&bits, 0, sizeof(bits_t));
set_cpu(0);
which_cpu("main thread");

    pthread_t pid1;
    pthread_create(&pid1, NULL, thd_func1, NULL);

    pthread_t pid2;
    pthread_create(&pid2, NULL, thd_func2, NULL);

    pthread_join(pid1, NULL);
    pthread_join(pid2, NULL);

    return 0;
}
