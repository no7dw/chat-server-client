#include "chat.h"


extern int sock_server_init(int &listenfd , uint16_t server_port);

void err_quit(const char *error_string)
{
	printf("%s\n", error_string);	
	exit(1);
}
void err_sys(const char *error_string)
{
	printf("%s\n", error_string);	
}

extern ssize_t p_read_from_p(int clientfd);
extern ssize_t p_write_to_p(int clientfd, const char *msg);

int do_use_fd(int fd)
{
	p_read_from_p(fd);
// 	sleep(2);
	p_write_to_p(fd, "Hi this is server");
	
	
}
//#define WAIT_TIME_OUT
void sig_chld(int signo)
{
	pid_t pid;
	int stat;
	while( (pid = waitpid(-1, &stat, WNOHANG)) > 0 )
	{
		printf("client %d terminated \n", pid);
	}
	return;
}
int setnonblocking(int fd)
{
	int flag;
	if( (flag =  fcntl(fd, F_GETFL, 0) ) < 0 )
		perror("F_GETFL");
	flag |= O_NONBLOCK;
	
	if( (flag =  fcntl(fd, F_SETFL, flag) ) < 0 )
	{
		perror("F_SETFL");
		return -1;
	}
	else
		return flag ;		
}

#include <sys/epoll.h>

#define MAX_EVENTS 1000
void chat_server()
{
	int listenfd, connfd, nfds, epollfd;
	struct epoll_event ev, events[MAX_EVENTS];
	
	struct sockaddr_in cliaddr;
	socklen_t clilen;
	char buf[200] = {0};
	
	sock_server_init(listenfd, SERVER_PORT);
	
	epollfd = epoll_create(MAX_EVENTS);
	if(epollfd == -1)
	{
		perror("epoll_create");
		exit(EXIT_FAILURE);		
	}
	ev.events = EPOLLIN;
	ev.data.fd = listenfd;
	if(epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev) == -1)
	{
		perror("epoll_ctl listenfd");
		exit(EXIT_FAILURE);
	}
// 	sleep(10);
	int i_have_server_client_counter = 0;
	for(;;)
	{
		nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
		if(nfds == -1)
		{
			perror("epoll_wait");
			exit(EXIT_FAILURE);
		}
		
		for(int n = 0; n < nfds; ++n)
		{
			printf("got client counts: %d \t handling fd: %d\n", nfds, events[n].data.fd );
			if(events[n].data.fd == listenfd)
			{
				clilen = sizeof(cliaddr);
				connfd = accept(listenfd, (struct sockaddr *)&cliaddr , &clilen);
				if(connfd == -1)
				{
					perror("accept");
					exit(EXIT_FAILURE);
				}
				printf("incoming connection from IP: %s Port: %d\n",
                            inet_ntop(AF_INET, &cliaddr.sin_addr, buf, sizeof(buf)),
                             ntohs(cliaddr.sin_port));
				
				setnonblocking(connfd);
				ev.events = EPOLLIN|EPOLLET;
				ev.data.fd = connfd;
				if(epoll_ctl(epollfd,EPOLL_CTL_ADD, connfd, &ev) == -1)
				{
					perror("epoll_ctl: connfd");
					exit(EXIT_FAILURE);
				}				
			}//if
			else
			{
				do_use_fd(events[n].data.fd);
				i_have_server_client_counter++;
				printf("server count %d\n", i_have_server_client_counter);
				if(epoll_ctl(epollfd,EPOLL_CTL_DEL, events[n].data.fd, &ev) == -1)
				{
					perror("epoll_ctl: EPOLL_CTL_DEL events[n].data.fd");
					exit(EXIT_FAILURE);
				}	
				close(events[n].data.fd);
			}
		}//for
		printf("next round\n");
	}//for
}



