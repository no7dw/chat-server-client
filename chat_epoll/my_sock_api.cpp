#include "chat.h"

#define LISTENQ 10

int sock_init(int &sockfd ,struct sockaddr_in &servaddr, uint16_t server_port)
{
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(server_port);
	return sockfd;
}
int sock_server_init(int &listenfd , uint16_t server_port)
{
	struct sockaddr_in servaddr;
	sock_init(listenfd, servaddr , server_port);		

	const int on = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));	
	if(bind(listenfd, (SA*)&servaddr, sizeof(servaddr)) < 0)
	{
		perror("bind");
		return -1;
	}
	listen(listenfd, LISTENQ);
	return listenfd;
}

int sock_client_init(const char*ipaddress, uint16_t server_port)
{
	int sockfd ;
	struct sockaddr_in servaddr;
	sock_init(sockfd, servaddr, server_port);	
	
	inet_pton(AF_INET, ipaddress, &servaddr.sin_addr);

	if(0 != connect(sockfd, (SA *) &servaddr, sizeof(servaddr)))
	{
		perror("connect");
		return -1;
	}
	return sockfd;	
}
extern ssize_t p_read_from_p(int fd);
extern ssize_t p_write_to_p(int fd,const char *msg );
#include <sys/epoll.h>
#include <time.h>
#include <errno.h>
int EventSet(myevent_s *ev, int fd, int(*call_back)(int, int, void*), void *arg)
{
	ev->fd = fd;
	ev->call_back = call_back;
	ev->events = 0;
	ev->arg = arg;
	ev->status = 0;
	ev->last_active = time(NULL);
	return 0;
}
int EventAdd(int epollfd, int events, myevent_s *ev)
{
	struct epoll_event epv = {0, {0} };
	int operation;
	epv.data.ptr = ev;
	epv.events = ev->events = events;
	if( 1 == ev->status)
	{
		operation = EPOLL_CTL_MOD;
	}
	else
	{
		operation = EPOLL_CTL_ADD;
		ev->status = 1;
	}
	if(epoll_ctl(epollfd, operation, ev->fd, &epv) < 0)
	{
		perror("epoll_ctl ADD or MOD");
		return -1;
	}
	return 0;
}
int EventDel(int epollfd, myevent_s *ev)
{
	struct epoll_event epv = {0 , {0} };
	if(ev->status != 1)
		return -1;
	epv.data.ptr = ev;
	ev->status = 0;
	if( epoll_ctl(epollfd, EPOLL_CTL_DEL, ev->fd, &epv)  < 0)
	{
		perror("epoll_ctl DEL");
		return -1;
	}
	return 0;
}

int RecvData(int fd, int events, void *arg);
int SendData(int fd, int events, void *arg);

int RecvData(int fd, int events, void *arg)
{
        struct myevent_s *ev = (struct myevent_s *)arg;
        int readback = p_read_from_p(fd);
        EventDel(g_epollfd, ev);
	if(readback > 0 )
	{
		ev->len = readback;
		EventSet(ev, fd, SendData, ev);
		EventAdd(g_epollfd, EPOLLOUT|EPOLLET, ev);
	}
	else if(readback == 0)
	{
		close(ev->fd);
		printf("close fd %d gracefully\n", fd);
	}
	else
	{
		close(ev->fd);
		printf("recv error %s  fd %d \n", strerror(errno), fd);
	}
	return readback;

}

int SendData(int fd, int events, void *arg)
{
	struct myevent_s *ev = (struct myevent_s *)arg;
	int writeback = p_write_to_p(fd, "Hi this is server");
	ev->len = 0;
	EventDel(g_epollfd, ev);
	if(writeback > 0)
	{
		EventSet(ev, fd, RecvData, ev);
		EventAdd(g_epollfd, EPOLLIN|EPOLLET, ev);
	}
	else
	{
		close(ev->fd);
		printf("send error %s , fd %d\n", strerror(errno), fd );
	}
}

int AcceptConn(int fd, int evetns, void *arg)
{
	struct sockaddr_in sin;
	socklen_t len = sizeof(struct sockaddr_in);	
	int clientfd, index;
	if((clientfd = accept( fd, (struct sockaddr * )&sin, &len)) == -1 )
	{
		perror("accept");
		return -1;
	}
	do{
		for(index = 0; index < MAX_EVENTS; index++)
		{
			if(g_Events[index].status == 0)//not in list
			{
				break;
			}
			if(index == MAX_EVENTS)
			{
				perror("achieve MAX_EVENTS");
				break;
			}
			if(fcntl(clientfd, F_SETFL, O_NONBLOCK) < 0)
				break;

			EventSet(&g_Events[index] , clientfd, RecvData, &g_Events[index]);
			EventAdd(g_epollfd, EPOLLIN|EPOLLET, &g_Events[index]);
			printf("new connection[%s:%d] at [time:%d]\n", inet_ntoa(sin.sin_addr),
				ntohs(sin.sin_port),(int) g_Events[index].last_active);

		}//for
	}while(0);
}

int sock_server_init_epoll(int &listenfd , uint16_t server_port)
{
	listenfd = sock_server_init(listenfd, server_port);
	EventSet(&g_Events[MAX_EVENTS], listenfd, AcceptConn, &g_Events[MAX_EVENTS]);
	EventAdd(listenfd, EPOLLIN|EPOLLET, &g_Events[MAX_EVENTS]);
	return listenfd;
}

