//server.c
/* net_select.c */

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netinet/in.h>

#define PORT				4321
#define MAX_QUE_CONN_NM			5
#define MAX_SOCK_FD			FD_SETSIZE
#define BUFFER_SIZE			1024

int main()
{
	struct sockaddr_in server_sockaddr, client_sockaddr;
	int sin_size, count;
	fd_set inset, tmp_inset;
	int sockfd, client_fd, fd;
	char buf[BUFFER_SIZE];
	
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		exit(1);
	}
	
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_port = htons(PORT);
	server_sockaddr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(server_sockaddr.sin_zero), 8);
	
	int i = 1;/* 使得重复使用本地地址与套接字进行绑定 */
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));
	if (bind(sockfd, (struct sockaddr *)&server_sockaddr, sizeof(struct sockaddr)) == -1)
	{
		perror("bind");
		exit(1);
	}
	
	if(listen(sockfd, MAX_QUE_CONN_NM) == -1)
	{
		perror("listen");
		exit(1);
	}
	printf("listening....\n");
	
	/*将调用socket函数的描述符作为文件描述符*/
	FD_ZERO(&inset); //将inset清零使集合中不含任何fd
	FD_SET(sockfd, &inset); //将fd加入inset集合
	
	while(1)
	{
		tmp_inset = inset;
		sin_size=sizeof(struct sockaddr_in);
		memset(buf, 0, sizeof(buf));
		
		/*调用select函数*/
		if (!(select(MAX_SOCK_FD, &tmp_inset, NULL, NULL, NULL) > 0))
		{
			perror("select");
			close(sockfd);
			exit(1);
		}
		
		for (fd = 0; fd < MAX_SOCK_FD; fd++)
		{
			if (FD_ISSET(fd, &tmp_inset) > 0) //在调用select()函数后，用FD_ISSET来检测fd在tmp_inset集合中的状态是否变化返回整型，当检测到fd状态发生变化时返回真，否则，返回假（0）
			{
				if (fd == sockfd)
				{ /* 服务端接收客户端的连接请求 */
					if ((client_fd = accept(sockfd, (struct sockaddr *)&client_sockaddr, &sin_size))== -1)
					{
						perror("accept");
						exit(1);
					}
					FD_SET(client_fd, &inset);
					printf("New connection from %d(socket)\n", client_fd);
				}
				else /* 处理从客户端发来的消息 */
				{
					if ((count = recv(fd, buf, BUFFER_SIZE, 0)) > 0)
					{
						printf("Received a message from %d: %s\n", fd, buf);
					}
					else		
					{
						close(fd);
						FD_CLR(fd, &inset);
						printf("Client %d(socket) has left\n", fd);
					}						
				}		
			} /* end of if FD_ISSET*/ 
		} /* end of for fd*/
	} /* end if while while*/
	
	close(sockfd);
	exit(0);
}



/*client.c*/

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>

#define PORT	4321
#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
	int sockfd, sendbytes;
	char buf[BUFFER_SIZE];
	struct hostent *host;
	struct sockaddr_in serv_addr;
	
	if(argc < 3)
	{
		fprintf(stderr,"USAGE: ./client Hostname(or ip address) Text\n");
		exit(1);
	}
	
	/*地址解析函数*/
	if ((host = gethostbyname(argv[1])) == NULL)
	{
		perror("gethostbyname");
		exit(1);
	}
	
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%s", argv[2]);
	
	/*创建socket*/
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
	{
		perror("socket");
		exit(1);
	}
	
	/*设置sockaddr_in 结构体中相关参数*/
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr = *((struct in_addr *)host->h_addr);
	bzero(&(serv_addr.sin_zero), 8);
	
	/*调用connect函数主动发起对服务器端的连接*/
	if(connect(sockfd,(struct sockaddr *)&serv_addr, sizeof(struct sockaddr))== -1)
	{
		perror("connect");
		exit(1);
	}
	
	/*发送消息给服务器端*/
	if ((sendbytes = send(sockfd, buf, strlen(buf), 0)) == -1)
	{
		perror("send");
		exit(1);
	}
	close(sockfd);
	exit(0);
}
