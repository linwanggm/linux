#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

#define PROCNUM        20    
#define FNAME        "/tmp/out"
#define BUFSIZE        1024

// 多个函数都要使用这个信号量 ID，所以定义为全局变量
static int semid;

static void P(void)
{
    struct sembuf op;

    op.sem_num = 0; // 只有一个资源，所以数组下标是 0
    op.sem_op = -1; // 取一个资源就减1
    op.sem_flg = 0; // 没有特殊要求
	//nsops=1,信号操作结构的数量，恒大于或等于1
    while(semop(semid,&op,1) < 0)
    {
        // 出现假错就重试
        if(errno != EINTR && errno != EAGAIN)
        {
            perror("semop()");
            exit(1);
        }
    }

}

static void V(void)
{
    struct sembuf op;

    op.sem_num = 0;
    op.sem_op = 1; // 归还一个资源
    op.sem_flg = 0;
	
    while(semop(semid,&op,1) < 0)
    {
        if(errno != EINTR && errno != EAGAIN)
        {
            perror("semop()");
            exit(1);
        }
    }
}

static void func_add()
{
    FILE *fp;
    char buf[BUFSIZE];

    fp = fopen(FNAME,"r+");    
    if(fp == NULL)
    {
        perror("fopen()");
        exit(1);
    }

    // 先取得信号量在操作文件，取不到就阻塞等待，避免发生竞争
    P();
    fgets(buf,BUFSIZE,fp);
    rewind(fp);    
    sleep(1); // 调试用，为了放大竞争，更容易看出来互斥量发挥了作用
    fprintf(fp,"%d\n",atoi(buf)+1);
    fflush(fp);
    // 操作结束之后归还信号量，让其它进程可以取得信号量
    V();
    fclose(fp);

    return ;
}

int main()
{
    int i;
    pid_t pid;

    // 在具有亲缘关系的进程之间使用，所以设置为 IPC_PRIVATE 即可。
    // 另外想要实现互斥量的效果，所以信号量数量设置为 1 个即可。
    semid = semget(IPC_PRIVATE,1,0600);
    if(semid < 0)
    {
        perror("semget()");
        exit(1);
    }

    // 将 union semun.val 的值设置为 1
    if(semctl(semid,0,SETVAL,1) < 0)
    {
        perror("semctl()");
        exit(1);
    }


    // 创建 20 个子进程
    for(i = 0 ; i < PROCNUM ; i++)
    {
        pid = fork();        
        if(pid < 0)
        {
            perror("fork()");
            exit(1);
        }
        if(pid == 0)    // child
        {
            func_add();
            exit(0);
        }
    }

    for(i = 0 ; i < PROCNUM ; i++)
        wait(NULL);

    // 销毁信号量
    semctl(semid,0,IPC_RMID);

    exit(0);
}

//the process from :http://www.cnblogs.com/chuyuhuashi/p/4475904.html
