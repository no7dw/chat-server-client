#include "chat.h"

extern int sock_server_init_epoll(int &listenfd , uint16_t server_port);
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


#include <sys/epoll.h>
extern void EventDel(int epollfd, myevent_s *ev);
void chat_server()
{

	int listenfd, connfd;
	printf("chat server start\n");
	void sig_chld(int);
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr ;
	
	if(sock_server_init_epoll(listenfd,(uint16_t) SERVER_PORT) < 0)
	{		
		return ;
	}
	
	signal(SIGCHLD, sig_chld);
	
	g_epollfd = epoll_create(MAX_EVENTS);
	if( g_epollfd < 0 )
	{
		perror("epoll_create");
		return ;
	}
	struct epoll_event events[MAX_EVENTS];
	int check_pos = 0;
	while(1)
	{
		long lnow = time(NULL);
		for(int index = 0; index < 100; index++, check_pos++)
		{
			if(check_pos == MAX_EVENTS )
				check_pos = 0;
			if(g_Events[check_pos].status != 1)
				continue;

			long lduration = lnow - g_Events[check_pos].last_active;
			if(lduration >= 60)
			{
				close(g_Events[check_pos].fd);
				printf("time out fd %d\n", g_Events[check_pos].fd);
				EventDel(g_epollfd, &g_Events[check_pos]);
			}
		}//for
		int fds = epoll_wait(g_epollfd, events, MAX_EVENTS, 1000);
		if(fds < 0)
		{
			perror("epoll_wait");
			break;
		}
		if(fds != 0)
		{
			for(int eindex = 0; eindex < fds; eindex++)
			{
				myevent_s *ev = (struct myevent_s *)events[eindex].data.ptr;
				if((events[eindex].events & EPOLLIN ) && (ev->events & EPOLLIN))
				{
					ev->call_back(ev->fd, events[eindex].events, ev->arg);
				}

				if((events[eindex].events & EPOLLOUT ) && (ev->events & EPOLLOUT))
				{
					ev->call_back(ev->fd, events[eindex].events, ev->arg);
				}
			}//for
		}//if
	}//while

	return ;
}



