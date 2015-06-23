#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h> //包含信号量相关头文件
using namespace std;

sem_t sem1,sem2; //声明两个信号量

struct
{
    pthread_rwlock_t rwlock;
    int product;
}sharedData = {PTHREAD_RWLOCK_INITIALIZER, 9};

void * produce(void *ptr)
{
    for (int i = 0; i < 5; ++i)
    {
        pthread_rwlock_wrlock(&sharedData.rwlock);
        sharedData.product = i;
        cout<<"produce:" << i<<endl;
        pthread_rwlock_unlock(&sharedData.rwlock);
        sleep(1);
    }
        sem_post(&sem1); //将信号量sem1的值加1，代表资源增加
}

void * consume1(void *ptr)
{
    sem_wait(&sem1);
    for (int i = 0; i < 5;)
    {
        pthread_rwlock_rdlock(&sharedData.rwlock);
        cout<<"consume1:"<<sharedData.product<<endl;
        pthread_rwlock_unlock(&sharedData.rwlock);
       ++i;
        sleep(1);
    }
        sem_post(&sem2); //将信号量sem2的值加1，代表资源增加
}

void * consume2(void *ptr)
{
    sem_wait(&sem2);
    for (int i = 0; i < 5;)
    {
        pthread_rwlock_rdlock(&sharedData.rwlock);
        cout<<"consume2:"<<sharedData.product<<endl;
        pthread_rwlock_unlock(&sharedData.rwlock);
        ++i;
        sleep(1);
    }
}

int main()
{
    pthread_t tid1, tid2, tid3;
    int ret1,ret2;
    ret1=sem_init(&sem1,0,0); //对信号量进行初始化，第一个0表示此信号量子整个进程中共享，第二个0表示信号量初始值
    ret2=sem_init(&sem2,0,0); 
    if(ret1!=0 || ret2 !=0 ) 
    { 
        perror("sem_init"); 
    } 
    pthread_create(&tid1, NULL, produce, NULL);
    pthread_create(&tid2, NULL, consume1, NULL);
    pthread_create(&tid3, NULL, consume2, NULL);

    void *retVal;

    pthread_join(tid1, &retVal);
    pthread_join(tid2, &retVal);
    pthread_join(tid3, &retVal);

    sem_destroy(&sem1);
    sem_destroy(&sem2);
    return 0;
}

[wln@localhost file]$ ./pthread2
produce:0
produce:1
produce:2
produce:3
produce:4
consume1:4
consume1:4
consume1:4
consume1:4
consume1:4
consume2:4
consume2:4
consume2:4
consume2:4
consume2:4
