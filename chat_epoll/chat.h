#ifndef __CHAT_H
#define __CHAT_H

#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>  
#include <sys/socket.h>  
#include <sys/types.h>  
#include <sys/socket.h>

#include <stdio.h>  
#include <time.h> 
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

//for inet_pton
#include <arpa/inet.h> 

//for waitpid
#include <sys/types.h>
#include <sys/wait.h>	

#define SA struct sockaddr

#define SERVER_PORT 54321

#define PROTOL_TYPE  0xE3
#define	OP_MESSAGE 0x4E

const short MAX_MSG_LENGTH = 512;

struct Message_Head
{
	char protol_type;		
	char msg_type ;	
	size_t msg_length;
	int msg_size;
	//this sort will make the msg lager than it should
	Message_Head()
	{
		protol_type = PROTOL_TYPE;
		msg_type = OP_MESSAGE;		
		msg_length = 0;
		msg_size = 3;//?
	};
		
};


struct Message
{	
	Message_Head msg_head;
	char msg_content[MAX_MSG_LENGTH];
	
	Message()
	{		
		memset(msg_content, 0, sizeof(msg_content) );
	};
	
};
struct myevent_s
{
        int fd; 
        int (*call_back)(int fd, int events, void *arg);
        int events;
        void *arg;
        int status;//// 1: in epoll wait list, 0 not in
        Message msg;
        int len;
        long last_active;
};
#define MAX_EVENTS 5000
int g_epollfd;
myevent_s g_Events[MAX_EVENTS+1];

#endif
