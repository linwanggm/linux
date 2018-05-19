#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

static int check_connect_port(const int startPort, int totalNum, char *address);

void showUsage(char *progname)
{
	printf("usage:\n"
		"    -p port    the start port number\n"
		"    -h host    the address\n"
		"    -totalNum number  the port from port to port+totalNum\n");
}

typedef struct fdCtl
{
	int fd;
	int connectStatus;
	int connected;
	int bchecked;
	int err;
} fdCtl;

int main(int argc, char *argv[])
{
	int totalNum = 50;
	int startPort = 1000;
	char address[32] = "127.0.0.1";


	

	if (argc < 2)
	{
			showUsage(argv[0]);
			exit(1);
	}

	char ch;
	while ((ch = getopt(argc, argv, "p:h:N:")) != -1)
	{
		switch (ch)
		{
			case 'p':
					startPort = atoi(optarg);
					break;
			case 'h':
					strncpy(address, optarg, 31);
					address[strlen(optarg)] = '\0';
					break;
			case 'N':
					totalNum = atoi(optarg);
					break;
			default:
					showUsage(argv[0]);
					exit(1);
		}
	}

	int k = 0;
	const int unit = 1000;
	for (k = 0; k < totalNum/unit; k++)
	{
		//printf("start = %d, end = %d\n", startPort+k*unit, startPort+k*unit+unit);
		check_connect_port(startPort + k * unit, unit, address);
	}
	//printf("start = %d, end = %d\n", startPort+k*unit, startPort+k*unit+totalNum % unit);
	check_connect_port(startPort + k * unit, totalNum % unit, address);
}

/*
* check the port form startPort to startPort + totalNum -1
*/
static int check_connect_port(const int startPort, int totalNum, char *address)
{
	fdCtl *fdHandle;
	int ret = 0;
	struct sockaddr_in *serv_addr;
	int i = 0;
	int j = 0;
	int maxfd = 0;
	int *fd;
	int *connected;

	fdHandle = (fdCtl *)malloc(sizeof(fdCtl) * totalNum);
	if (!fdHandle)
	{
		fprintf(stderr, "malloc %d byte fail: %s", sizeof(fdCtl) * totalNum, strerror(errno));
		exit(1);
	}

	serv_addr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in) * totalNum);
	if (!serv_addr)
	{
		fprintf(stderr, "malloc %d byte fail: %s", sizeof(struct sockaddr_in) * totalNum, strerror(errno));
		exit(1);
	}

	/* create totalNum socket fd */
	while (i < totalNum)
	{
		fdHandle[i].fd = socket(AF_INET, SOCK_STREAM, 0);
		if (fdHandle[i].fd == -1)
		{
			while (j < i)
					close(fdHandle[i].fd);

			free(fdHandle);
			free(serv_addr);
			fprintf(stderr, "create socket fail : %s", strerror(errno));
			exit(1);
		}

		fdHandle[i].bchecked = 0;
		/* set noblock */
		int flags = fcntl(fdHandle[i].fd, F_GETFL, 0);
		fcntl(fdHandle[i].fd, F_SETFL, flags | O_NONBLOCK);
		if (fdHandle[i].fd > maxfd)
				maxfd = fdHandle[i].fd;
		i++;
	}

	i = 0;
	/* init */
	while (i < totalNum)
	{
		serv_addr[i].sin_family = AF_INET;
		serv_addr[i].sin_port = htons(startPort + i);
		serv_addr[i].sin_addr.s_addr = inet_addr(address);
		i++;
	}

	i = 0;
	while (i < totalNum)
	{
		fdHandle[i].connected = connect(fdHandle[i].fd, (struct sockaddr *)&serv_addr[i], sizeof(struct sockaddr_in));
		fdHandle[i].err = errno;
		i++;
	}

	/*check the connect status */
	fd_set cset, rset, wset;
	struct timeval tval;

	int m = 0;
	int s = 0;
	while (m++ < totalNum)
	{
		tval.tv_sec = 0;
		tval.tv_usec = 300000;
		FD_ZERO(&cset);
		FD_ZERO(&rset);
		FD_ZERO(&wset);
		i = 0;
		maxfd = 0;
		s = 0;

		/* fd set */
		while (i < totalNum)
		{
			if (fdHandle[i].connected == 0)
			{
				fdHandle[i].bchecked = 1;
				printf("connect ok, fd = %d, port= %d\n", fdHandle[i].fd, startPort + i);
			}
			else if (fdHandle[i].connected == -1 && fdHandle[i].err == EINPROGRESS)
			{
				if (!fdHandle[i].bchecked)
				{
					FD_SET(fdHandle[i].fd, &rset);
					FD_SET(fdHandle[i].fd, &wset);
					s++;
				}
				if (fdHandle[i].fd > maxfd)
					maxfd = fdHandle[i].fd;
			}
			else
			{
				fdHandle[i].bchecked = 1;
				printf("connect error : %s, fd = %d, port= %d\n", strerror(errno), fdHandle[i].fd, startPort+i);
			}

			i++;
		}

		if (!s)
			break;

		int res = select(maxfd + 1, &rset, &wset, NULL, &tval);
		if (res < 0)
		{
			printf("network error in connect : %s\n", strerror(errno));
		}
		else if (res == 0)
		{
			printf("connect time out\n");
		}
		else
		{
			/* check connect status */
			int k = 0;
			while (k < totalNum)
			{
				int r1 = FD_ISSET(fdHandle[k].fd, &rset);
				int w1 = FD_ISSET(fdHandle[k].fd, &wset);
				if ( r1 || w1 )
				{
					int err = 0;
					int len = sizeof(err);
					int rt = getsockopt(fdHandle[k].fd, SOL_SOCKET, SO_ERROR,&err,&len);
					if (!err)
						printf("get the connect ok : fd = %d, port = %d\n", fdHandle[k].fd, startPort + k);
					fdHandle[k].bchecked = 1;
				}
				k++;
			}
		}
	}

	/* close fd */
	i = 0;
	while (i < totalNum)
	{
		close(fdHandle[i].fd);
		i++;
	}

	free(serv_addr);
	free(fdHandle);
}
