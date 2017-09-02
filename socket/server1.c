/*
  server1.c 
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <getopt.h>

#define LISTEN_PORT  4322
#define SERVER_VERSION "server 1.0"

static int listenFd;
static int listenPort = LISTEN_PORT;
static bool runAsBackend = false;


static void parse_options(const int argc, char **argv);
static void show_help(void);
static void start_listen(void);
static void begin_server(void);
static void server_run(void);
static void signal_child(int sig);
static void show_client_message(int sockfd);
static void run_backend(void);

int
main(int argc, char **argv)
{
	parse_options(argc, argv);

	start_listen();

	begin_server();

	server_run();


}

static void
parse_options(const int argc, char **argv)
{
	listenPort = LISTEN_PORT;

	static struct option long_options[] =
	{
		{"help", no_argument, NULL, '?'},
		{"version", no_argument, NULL, 'V'},
		{"port", required_argument, NULL, 'p'},
		{"backend", no_argument, NULL, 'b'}
	};

	int c;
	int option_index;
	while((c=getopt_long(argc, argv, "bp:V", long_options, &option_index)) != -1)
	{
		switch(c)
		{
			case 'V':
				fprintf(stdout, SERVER_VERSION);
				exit(EXIT_SUCCESS);
				break;
			case 'p':
				listenPort = atoi(optarg);
				if (listenPort < 0 || listenPort > 65535)
				{
								fprintf(stderr, "Invalid port number \"%s\"", optarg);
								exit(EXIT_FAILURE);
				}
				fprintf(stdout, "listen_port = %d\n", listenPort);
				break;
			case 'b':
				runAsBackend = true;
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
show_help(void)
{
	fprintf(stderr, "server command\n");
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "options\n");
	fprintf(stderr, "       -p --port=listenPort    listen port\n");
	fprintf(stderr, "       -b --backend            run as backend\n");
	fprintf(stderr, "       -V --version            show server version, then exit\n");
	fprintf(stderr, "       -? --help               show help, then exit\n");
	exit(EXIT_FAILURE);
}


static void
start_listen(void)
{
	int connfd;
	pid_t childpid;

	struct sockaddr_in servaddr;

	listenFd=socket(AF_INET, SOCK_STREAM, 0);
	if(listenFd==-1)
	{
		fprintf(stderr, "socket established error: %s\n",strerror(errno));
						exit(EXIT_FAILURE);
	}

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(listenPort);

	int bindc = bind(listenFd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if(bindc == -1)
	{
		printf("bind error: %s\n",strerror(errno));
		return;
	}
	listen(listenFd,5);
}


static void
begin_server(void)
{
	/* signal handle */
	signal(SIGINT, SIG_DFL);
	signal(SIGTERM, SIG_DFL);
	signal(SIGCHLD, signal_child);

	/*if run as backend*/
	if (runAsBackend)
		run_backend();
}

static void
server_run(void)
{
	struct sockaddr_in clientAddr;
	socklen_t clientLen;
	int clientFd;
	pid_t clientPid;
	while(1)
	{
		clientLen = sizeof(clientAddr);
		clientFd = accept(listenFd, (struct sockaddr *)&clientAddr, &clientLen);
		if (clientFd == -1)
		{
			fprintf(stderr, "accept client error: %s\n", strerror(errno));
			return;
		}
		else
		{
			fprintf(stdout, "client connected\n");
		}

		if (clientPid = fork() == 0)
		{
			close(listenFd);
			printf("client from %s\n", inet_ntoa(clientAddr.sin_addr));
			/*show client sends message*/
			show_client_message(clientFd);
			exit(0);
		}
		else if(clientFd < 0)
		{
			fprintf(stderr, "fork error: %s\n", strerror(errno));
		}

		close(clientFd);
	}

	close(listenFd);
}


/* wait until all child processes finish*/
static void
signal_child(int sig)
{
	pid_t pid;
	int exitStaus;
	while(pid = waitpid(-1, &exitStaus, WNOHANG) >0);
}

static void
show_client_message(int sockfd)
{
	ssize_t n;
	char line[512];

	fprintf(stdout, "ready to read\n");
	while((n = read(sockfd, line, 512))> 0)
	{
		line[n] = 0;
		fprintf(stdout, "cient send: %s\n", line);
		bzero(&line,sizeof(line));
	}

	fprintf(stdout, "end client\n");
}


static void
run_backend(void)
{
   pid_t pid = fork();

  if (pid == 0)
  {
	FILE *fd = fopen("/dev/null", "r");
	if (fd < 0)
	{
		perror("open file fail:");
		exit(EXIT_FAILURE);
	}

	if (fd != NULL)
	{
		dup2(fileno(fd), 0);
		fclose(fd);
	}

	fd = fopen("/dev/null", "w");
	if (fd != NULL)
	{
		dup2(fileno(fd), 1);
		dup2(fileno(fd), 2);
		fclose(fd);
	}
	setsid();

    }
	else if (pid > 0)
	{
		fprintf(stdout, "%d\n", (int)pid);
		exit(EXIT_SUCCESS);
	}
	else
	{
		fprintf(stderr, "could not fork new process:%m\n");
		exit(EXIT_FAILURE);

	 }
}

