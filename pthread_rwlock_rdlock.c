#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <pthread.h> 
#include <errno.h> 

#define MAXDATA 1024 
#define MAXREDER 100 
#define MAXWRITER 100 
struct 
{ 
  pthread_rwlock_t rwlock; //读写锁 
  char datas[MAXDATA]; //共享数据域 
}shared = { PTHREAD_RWLOCK_INITIALIZER }; 
  
void *reader(void *arg); 
void *writer(void *arg); 

int main(int argc,char *argv[]) 
{ 
	int i,readercount,writercount; 
	pthread_t tid_reader[MAXREDER],tid_writer[MAXWRITER]; 
	if(argc != 3) 
	{ 
	   printf("usage :%s #<readercount> #<writercount>\n",argv[0]); 
	   exit(0); 
	} 

	readercount = atoi(argv[1]); //读者个数 
	writercount = atoi(argv[2]); //写者个数 
	pthread_setconcurrency(readercount+writercount); 

	for(i=0;i<writercount;++i) 
	  pthread_create(&tid_writer[i],NULL,writer,NULL); 
	  sleep(1); //等待写者先执行 
	  
	  for(i=0;i<readercount;++i) 
	  pthread_create(&tid_reader[i],NULL,reader,NULL); //等待线程终止 
	  
	  for(i=0;i<writercount;++i) 
	  pthread_join(tid_writer[i],NULL); 
	  
	  for(i=0;i<readercount;++i) 
	  pthread_join(tid_reader[i],NULL); 
	  return 0; 
} 

void *reader(void *arg) 
{ 
	if( pthread_rwlock_rdlock(&shared.rwlock) ==0 ) //获取读出锁 
	   printf("pthread_rwlock_rdlock OK\n"); 
	   
	printf("Reader begins read message.\n"); 
	printf("Read message is: %s\n",shared.datas); 

	if( pthread_rwlock_unlock(&shared.rwlock) != 0 ) 
	  printf("pthread_rwlock_unlock read fail\n");
	 else
	  printf("pthread rwlock unlock read ok\n"); 
	return NULL; 
} 


void *writer(void *arg) 
{ 
	 char datas[MAXDATA]; 
	 //pthread_rwlock_wrlock(&shared.rwlock); //获取写锁 
	 
	 if( pthread_rwlock_wrlock(&shared.rwlock) != 0)//再次获取写锁 
	   perror("pthread_rwlock_wrlock\n"); 
			 
	printf("Writers begings write message.\n"); 
	//sleep(1); 

	printf("Enter the write message: \n"); 

	scanf("%s",datas); //写入数据 
	strcat(shared.datas,datas); 

	if(pthread_rwlock_unlock(&shared.rwlock)!=0) //释放锁 
	  printf("pthread_rwlock_unlock write fail\n");
	 else
	  printf("pthread rwlock unlock write ok\n");
	return NULL; 
}
