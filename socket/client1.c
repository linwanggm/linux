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

static char *server_adress;
static char *send_msg;
static int sockfd;
static int serverPort;

static void parse_options(const int argc, char **argv);
static void send_message(char *data, int sockfd);
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
				server_adress = optarg;
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
	servaddr.sin_port=htons(SERVER_PORT);
	inet_pton(AF_INET, server_adress, &servaddr.sin_addr);

	fprintf(stdout, "client try to connect %s\n", server_adress);
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
	send_message(send_msg,sockfd);
	exit(EXIT_SUCCESS);
}


static void send_message(char *data, int sockfd)
{
	char recv[512];

	int wc=write(sockfd,data,strlen(data));

	exit(EXIT_SUCCESS);
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
