//sem_write.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h>

#define READ 0
#define WRITE 1

#define N 2

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
}mysemun;

//初始化信号灯集中的信号灯的值
int my_sem_init(int semid)
{
    int i = 0;

    for(i = 0;i < N;i ++)
    {
        mysemun.val = i;
        if(semctl(semid,i,SETVAL,mysemun) < 0)
        {
            perror("Fail to semctl");
            return -1;
        }
    }

    return 0;
}

//信号灯集中的信号灯释放资源
int my_sem_post(int semid,int sem_num)
{
    struct sembuf op;

    op.sem_num = sem_num;
    op.sem_op = 1;
    op.sem_flg = 0;

    if(semop(semid,&op,1) < 0)
    {
        perror("Fail to semop");
        return -1;
    }

    return 0;
}

//信号灯集中的信号灯申请资源
int my_sem_wait(int semid,int sem_num)
{
    struct sembuf op;

    op.sem_num = sem_num;
    op.sem_op = -1;
    op.sem_flg = 0;

    if(semop(semid,&op,1) < 0)
    {
        perror("Fail to semop");
        return -1;
    }

    return 0;
}

//读共享内存
int read_share_memory(int semid,char *addr)
{
    int n;

    while(1)
    {
        my_sem_wait(semid,WRITE);

        printf(">");
        fgets(addr,1024,stdin);
        addr[strlen(addr)-1] = '\0';

        my_sem_post(semid,READ);

        if(strncmp(addr,"quit",4) == 0)
        {
            if(shmdt((void *)addr) < 0)
            {
                perror("Fail to semdt");
                return -1;
            }
            
            break;
        }

    }
    
    return 0;
}

int main(int argc,char *argv[])
{
    key_t key;
    void *shmaddr;
    int shmid,semid;
    
    if(argc < 2)
    {
        fprintf(stderr,"usage : %s argv[1].\n",argv[0]);
        exit(EXIT_FAILURE);
    }
    
    //获取键值
    if((key = ftok(argv[1],'a')) < 0)
    {
        perror("Fail to ftok");
        exit(EXIT_FAILURE);
    }
    
    //创建共享内存
    if((shmid = shmget(key,1024,IPC_CREAT | 0666)) < 0)
    {
        perror("Fail to shmget");
        exit(EXIT_FAILURE);
    }

    //映射共享内存到进程地址空间
    if((shmaddr = shmat(shmid,NULL,0)) == (void *)-1)
    {
        perror("Fail to shmat");
        exit(EXIT_FAILURE);
    }
    
    //创建含有2个的信号灯的信号灯集
    if((semid = semget(key,N,IPC_CREAT | 0666)) < 0)
    {
        perror("Fail to shmget");
        exit(EXIT_FAILURE);
    }

    //初始化信号灯集中的信号灯
    my_sem_init(semid);
    
    read_share_memory(semid,(char *)shmaddr);

    if(shmctl(shmid,IPC_RMID,NULL) < 0)
    {
        perror("Fail to shmctl");
        return -1;
    }

    if(semctl(semid,0,IPC_RMID,0) < 0)
    {
        perror("Fail to semctl WRITE");
        return -1;
    }

    exit(EXIT_SUCCESS);
    
    
    //------------------
    //sem_read.c
    
        #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <errno.h>
    #include <unistd.h>
    #include <sys/ipc.h>
    #include <sys/shm.h>
    #include <sys/types.h>
    #include <sys/sem.h>

    #define READ 0
    #define WRITE 1

    #define N 2

    union semun{
        int val;
        struct semid_ds *buf;
        unsigned short *array;
        struct seminfo *__buf;
    };

    //初始化信号灯集中的信号灯的值
    int my_sem_init(int semid)
    {
        int i = 0;
        union semun mysemun;

        for(i = 0;i < N;i ++)
        {
            mysemun.val = i;
            if(semctl(semid,i,SETVAL,mysemun) < 0)
            {
                perror("Fail to semctl");
                return -1;
            }
        }

        return 0;
    }

    //信号灯集中的信号灯释放资源
    int my_sem_post(int semid,int sem_num)
    {
        struct sembuf op;

        op.sem_num = sem_num;
        op.sem_op = 1;
        op.sem_flg = 0;

        if(semop(semid,&op,1) < 0)
        {
            perror("Fail to semop");
            return -1;
        }

        return 0;
    }

    //信号灯集中的信号灯申请资源
    int my_sem_wait(int semid,int sem_num)
    {
        struct sembuf op;

        op.sem_num = sem_num;
        op.sem_op = -1;
        op.sem_flg = 0;

        if(semop(semid,&op,1) < 0)
        {
            perror("Fail to semop");
            return -1;
        }

        return 0;
    }

    //读共享内存
    int read_share_memory(int semid,char *addr)
    {
        int n;

        while(1)
        {
            my_sem_wait(semid,READ);
            
            printf("Read : %s.\n",addr);

            my_sem_post(semid,WRITE);

            if(strncmp(addr,"quit",4) == 0)
            {
                if(shmdt((void *)addr) < 0)
                {
                    perror("Fail to semdt");
                    return -1;
                }
                
                break;
            }

        }
        
        return 0;
    }

    int main(int argc,char *argv[])
    {
        key_t key;
        void *shmaddr;
        int shmid,semid;
        
        if(argc < 2)
        {
            fprintf(stderr,"usage : %s argv[1].\n",argv[0]);
            exit(EXIT_FAILURE);
        }
        
        //获取键值
        if((key = ftok(argv[1],'a')) < 0)
        {
            perror("Fail to ftok");
            exit(EXIT_FAILURE);
        }
        
        //创建共享内存
        if((shmid = shmget(key,1024,IPC_CREAT | 0666)) < 0)
        {
            perror("Fail to shmget");
            exit(EXIT_FAILURE);
        }

        //映射共享内存到进程地址空间
        if((shmaddr = shmat(shmid,NULL,0)) == (void *)-1)
        {
            perror("Fail to shmat");
            exit(EXIT_FAILURE);
        }
        
        //创建含有2个的信号灯的信号灯集
        if((semid = semget(key,N,IPC_CREAT | 0666)) < 0)
        {
            perror("Fail to shmget");
            exit(EXIT_FAILURE);
        }

        //初始化信号灯集中的信号灯
        my_sem_init(semid);
        
        read_share_memory(semid,(char *)shmaddr);

        exit(EXIT_SUCCESS);
    }
    
    
//the process from : http://blog.chinaunix.net/uid-26833883-id-3230813.html
