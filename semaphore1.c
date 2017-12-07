/* test1 */
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <pthread.h>  
#include <errno.h> 
#include <sys/ipc.h> 
#include <semaphore.h>  
int lock_var; 
time_t end_time; 
sem_t sem1,sem2;  
 
void pthread1(void *arg)  
{ 
    int i; 
    while(time(NULL) < end_time){ 
        sem_wait(&sem2); //线程阻塞一直等到sem2信号量大于0，执行后将sem2减1，代表资源已经被使用
        for(i=0;i<2;i++){ 
            sleep(1); 
            lock_var++; 
            printf("lock_var=%d\n",lock_var); 
        } 
        printf("pthread1:lock_var=%d\n",lock_var); 
        sem_post(&sem1); //将信号量sem1的值加1，代表资源增加
        sleep(1); 
    } 
} 
 
void pthread2(void *arg) 
{ 
    int nolock=0; 
    int ret; 
    while(time(NULL) < end_time){ 
    sem_wait(&sem1); 
    printf("pthread2:got lock;lock_var2=%d\n",lock_var); 
    sem_post(&sem2); 
    sleep(3); 
    } 
} 

 
int main(int argc, char *argv[]) 
{ 
    pthread_t id1,id2;  
    pthread_t mon_th_id; 
    int ret; 
    end_time = time(NULL)+8; 
    ret=sem_init(&sem1,0,1); //对信号量进行初始化，第一个0表示此信号量子整个进程中共享，第二个1表示信号量初始值
    ret=sem_init(&sem2,0,0); 
    if(ret!=0) 
    { 
        perror("sem_init"); 
    } 
    ret=pthread_create(&id1,NULL,(void *)pthread1, NULL);  
    if(ret!=0) 
        perror("pthread cread1"); 
    ret=pthread_create(&id2,NULL,(void *)pthread2, NULL); 
    if(ret!=0) 
        perror("pthread cread2"); 
    pthread_join(id1,NULL); //用来等待线程1的结束
    pthread_join(id2,NULL); //用来等待线程2的结束
    exit(0); 
} 
 
