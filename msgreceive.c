//msgreceive.c
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/msg.h>

struct msg_st
{
        long int msg_type;
        char text[BUFSIZ];
};

int main()
{
        int running = 1;
        int msgid = -1;
        struct msg_st data;
        long int msgtype = 0; //注意1

        //建立消息队列
        msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
        if(msgid == -1)
        {
                fprintf(stderr, "msgget failed with error: %d\n", errno);
                exit(EXIT_FAILURE);
        }
        //从队列中获取消息，直到遇到end消息为止
        while(running)
        {
                if(msgrcv(msgid, (void*)&data, BUFSIZ, msgtype, 0) == -1)
                {
                        fprintf(stderr, "msgrcv failed with errno: %d\n", errno);
                        exit(EXIT_FAILURE);
                }
                printf("You wrote: %s\n",data.text);
                //遇到end结束
                if(strncmp(data.text, "end", 3) == 0)
                        running = 0;
        }
        //删除消息队列
        if(msgctl(msgid, IPC_RMID, 0) == -1)
        {
                fprintf(stderr, "msgctl(IPC_RMID) failed\n");
                exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
}


---------------------------------------------------------
//msgsend.c
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/msg.h>
#include <errno.h>

#define MAX_TEXT 512
struct msg_st
{
        long int msg_type;
        char text[MAX_TEXT];
};

int main()
{
        int running = 1;
        struct msg_st data;
        char buffer[BUFSIZ];
        int msgid = -1;

        //建立消息队列
        msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
        if(msgid == -1)
        {
                fprintf(stderr, "msgget failed with error: %d\n", errno);
                exit(EXIT_FAILURE);
        }

        //向消息队列中写消息，直到写入end
        while(running)
        {
                //输入数据
                printf("Enter some text: ");
                fgets(buffer, BUFSIZ, stdin);
                data.msg_type = 1;    //注意2
                strcpy(data.text, buffer);
                //向队列发送数据
                if(msgsnd(msgid, (void*)&data, MAX_TEXT, 0) == -1)
                {
                        fprintf(stderr, "msgsnd failed\n");
                        exit(EXIT_FAILURE);
                }
                //输入end结束输入
                if(strncmp(buffer, "end", 3) == 0)
                        running = 0;
                sleep(1);
        }
        exit(EXIT_SUCCESS);
}
