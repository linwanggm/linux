/*
client1.c

*/
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdbool.h>
#include <getopt.h>
#include <signal.h>

#define SERVER_PORT  4322
#define CLIENT_VERSION "client 1.0"
#define SERVER_ADDRESS "127.0.0.1"

#define min(x,y) ({ \
 typeof(x) _x = (x); \
 typeof(y) _y = (y); \
 (void) (&_x == &_y);  \
 _x < _y ? _x : _y; })

#define max(x,y) ({ \
 typeof(x) _x = (x); \
 typeof(y) _y = (y); \
 (void) (&_x == &_y);  \
 _x > _y ? _x : _y; })

static char *server_address;
static char *send_msg;
static int sockfd;
static int serverPort;

static void parse_options(const int argc, char **argv);
static void client_communicate(char *data, int sockfd);
static void start_connect(void);
static void	begin_server(void);
static void client_run(void);
static void show_help(void);

int main(int argc, char **argv)
{
	parse_options(argc, argv);

	start_connect();

	begin_server();

	client_run();
}
	
static void
parse_options(const int argc, char **argv)
{
	serverPort = SERVER_PORT;
	server_address = SERVER_ADDRESS;

	static struct option long_options[] =
	{
		{"help", no_argument, NULL, '?'},
		{"version", no_argument, NULL, 'V'},
		{"port", required_argument, NULL, 'p'},
		{"address", required_argument, NULL, 'h'},
		{"string", required_argument, NULL, 's'}
	};

	int c;
	int option_index;
	while((c=getopt_long(argc, argv, "p:h:s:V", long_options, &option_index)) != -1)
	{
		switch(c)
		{
			case 'V':
				fprintf(stdout, CLIENT_VERSION);
				exit(EXIT_SUCCESS);
				break;
			case 'p':
				serverPort = atoi(optarg);
				if (serverPort < 0 || serverPort > 65535)
				{
					fprintf(stderr, "Invalid port number \"%s\"", optarg);
					exit(EXIT_FAILURE);
				}
				fprintf(stdout, "server_port = %d\n", serverPort);
				break;
			case 'h':
				server_address = optarg;
				break;
			case 's':
				send_msg = optarg;
				break;
			case '?':
				show_help();
				break;
			default:
				show_help();
				break;
		}
	}

	if (optind < argc)
		show_help();
}

static void
start_connect(void)
{
	struct sockaddr_in servaddr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1)
	{
		printf("socket established error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(serverPort);
	inet_pton(AF_INET, server_address, &servaddr.sin_addr);

	fprintf(stdout, "client try to connect %s\n", server_address);
	int conRes=connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if(conRes==-1)
	{    
		fprintf(stderr, "connect error: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}

}

static void
begin_server(void)
{
	/* signal handle */
	signal(SIGINT, SIG_DFL);
	signal(SIGTERM, SIG_DFL);

}

	
static void
client_run(void)
{
	client_communicate(send_msg,sockfd);
	exit(EXIT_SUCCESS);
}


static void client_communicate(char *data, int sockfd)
{
	char recvstr[512];
	char sendstr[512];
	int wc;
	int maxfd;
	int ret;
	fd_set readSet;
	FILE *fp = stdin;
	
	FD_ZERO(&readSet);
	
	for(;;)
	{
		FD_SET(fileno(fp), &readSet);
		FD_SET(sockfd, &readSet);
		maxfd = max(fileno(fp), sockfd) + 1;
		select(maxfd, &readSet, NULL, NULL, NULL);
		
		if( FD_ISSET(sockfd, &readSet))
		{
			while(ret = read(sockfd, recvstr, sizeof(recvstr)-1))
			{
				if (ret == 0)
				{
					fprintf(stdout, "receive from server end\n");
					break;
				}
				else if (ret < 0)
				{
					fprintf(stderr, "receive from server error: %s\n", strerror(errno));
					exit(EXIT_FAILURE);
				}
				else
				{
					recvstr[ret] = '\0';
					fprintf(stdout, "receive from server: %s\n", recvstr);
					break;
				}
				
				memset(recvstr, 0, sizeof(recvstr));
			}
		}
		
		if( FD_ISSET(fileno(fp),&readSet))
		{
			fgets(sendstr, sizeof(sendstr)-1, fp);
			ret = write(sockfd, sendstr, strlen(sendstr)-1);
			memset(sendstr, 0, sizeof(sendstr));
		}
		
		
	}
	
}

static void
show_help(void)
{
	fprintf(stderr, "client command\n");
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "options\n");
	fprintf(stderr, "       -h --address=serverAddress    server address\n");
	fprintf(stderr, "       -p --port=serverPort    			server port\n");
	fprintf(stderr, "       -s --string=sendString    		send string\n");
	fprintf(stderr, "       -p --port=serverPort    server port\n");
	fprintf(stderr, "       -V --version            show server version, then exit\n");
	fprintf(stderr, "       -? --help               show help, then exit\n");
	exit(EXIT_FAILURE);
}
