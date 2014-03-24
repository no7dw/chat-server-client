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
void chat_server()
{

	int listenfd, connfd;
	printf("chat server start\n");
	void sig_chld(int);
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr ;
	
	if(sock_server_init(listenfd,(uint16_t) SERVER_PORT) < 0)
	{		
		return ;
	}
	
	signal(SIGCHLD, sig_chld);
	
	char buf[200] = {0};
	fd_set rset, allset;
	int maxfd, maxi, i, nready;
	int client[FD_SETSIZE], sockfd;
	ssize_t n_have_read;
	maxfd = listenfd;	
	maxi = -1;
	for(i = 0; i < FD_SETSIZE; i++)
		client[i] = -1;
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);
#ifdef WAIT_TIME_OUT
	struct timeval timeout;
	timeout.tv_sec = 2;
	timeout.tv_usec = 500000;
#endif
	for( ; ; )
	{
		rset = allset ;
#ifdef WAIT_TIME_OUT
		nready = select( maxfd +1 , &rset , NULL, NULL, &timeout);
#else
		nready = select( maxfd +1 , &rset , NULL, NULL,  NULL);
#endif		
		if(nready == 0)
		{
			printf("select timeout\n");
		}
		else if(nready <  0)
		{
			perror("select");
		}	
		else
		{
			if(FD_ISSET(listenfd, &rset))//new incoming connection
			{
			//	int sleep_rand_sec = rand()%4;
			//	printf("  sleep %d  ", sleep_rand_sec);
			//	sleep(sleep_rand_sec);
				clilen = sizeof(cliaddr);
				connfd = accept(listenfd, (SA*)&cliaddr, &clilen);
				printf("incoming connection from IP: %s Port: %d\n", 
				   inet_ntop(AF_INET, &cliaddr.sin_addr, buf, sizeof(buf)), 
				   ntohs(cliaddr.sin_port));		

				for(i= 0; i< FD_SETSIZE; i++)
				{
					if(client[i] < 0)
					{
						client[i] = connfd;//save fd to empty pos
						break;
					}
				}//for
				if(i == FD_SETSIZE)
					err_quit("too many clients");
				
				FD_SET(connfd, &allset);//add new fd to set

				//a few trick to avoid read all fdset
				if(connfd >  maxfd)//compare to mark maxfd for select
					maxfd = connfd;

				if(i > maxi) //mark max in client[] array
					maxi = i;

				if(--nready <= 0) //no more fd to read , keep wait in accept
					continue;


			}//if FD_SSSET
		}
		//usage of trick to avoid read all fdset
		for(i = 0; i <= maxi; i++)
		{
			if( (sockfd = client[i]) < 0)
				continue;//this is not a ready one 

			if(FD_ISSET(sockfd, &rset))
			{
				if(( n_have_read = p_read_from_p(sockfd ) ) == 0)
				{
					//connection closed by other end
					printf("close sock %d\n", sockfd);
					close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i] = -1;
					//continue;
				}
				else
				{
					//printf("n_have_read = %d , client[%d] = %d \n", n_have_read, i, client[i]);
					p_write_to_p(sockfd, "hi , this is server");
				}
				if(--nready <= 0)
					break;//no more readable fd to handle keep wait in accetp
			}//if FD_ISSET
		}//for

	}//for
}
/*
void chat_server()
{
	printf("chat server start\n");
	void sig_chld(int);
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr ;
	
	if(sock_server_init(listenfd,(uint16_t) SERVER_PORT) < 0)
	{		
		return ;
	}
	
	signal(SIGCHLD, sig_chld);
	
	char buf[200] = {0};
	
	for(;;)
	{
		clilen = sizeof(cliaddr);
		if((connfd = accept(listenfd, (SA*)&cliaddr, &clilen )) < 0)
		{
			if(errno == EINTR)
				continue;
			else
				err_sys("accept error");
		}
		printf("incoming connection from IP: %s Port: %d\n", 
			   inet_ntop(AF_INET, &cliaddr.sin_addr, buf, sizeof(buf)), 
			   ntohs(cliaddr.sin_port));		
		p_read_from_p(connfd);
		p_write_to_p(connfd, "hi , this is server");
		close(connfd);
	}//for
}
*/




