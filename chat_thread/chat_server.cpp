#include "chat.h"

#include "pthread_chat.h"
int listenfd, connfd;
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
//pthread_mutex_t     clifd_mutex = PTHREAD_MUTEX_INITIALIZER;
//pthread_cond_t clifd_cond = PTHREAD_COND_INITIALIZER;

// prethread with only main thread doing accept()
void chat_server()
{
	printf("chat server start\n");
	void sig_chld(int);
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr ;
	int clifd[MAXNCLI];

	if(sock_server_init(listenfd,(uint16_t) SERVER_PORT) < 0)
	{		
		return ;
	}
	signal(SIGCHLD, sig_chld);
	char buf[200] = {0};
	
	tptr =(Thread*) calloc(maxthreads, sizeof(Thread));

	int i;
	iget = iput = 0;
	
	for( i = 0; i < maxthreads  ; i++)
	{
		thread_make(i);
	}
	for( ;  ;) 
	{
		clilen = sizeof(cliaddr);
		connfd = accept(listenfd, (SA*)&cliaddr, &clilen);
		printf("incoming connection from IP: %s Port: %d\n", 
		   inet_ntop(AF_INET, &cliaddr.sin_addr, buf, sizeof(buf)), 
		   ntohs(cliaddr.sin_port));		

		pthread_mutex_lock(&clifd_mutex);
		clifd[iput] = connfd;
		if(++iput == MAXNCLI)
		   iput = 0;
		if(iput == iget)
		   err_quit("iput = iget  ");
		pthread_cond_signal(&clifd_cond);
		pthread_mutex_unlock(&clifd_mutex);
	   		   
	}//for
}
/*
 * select version
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
	struct timeval timeout;
	timeout.tv_sec = 3;
//	timeout.tv_usec = 500000;
	
	for( ; ; )
	{
		rset = allset ;
		nready = select( maxfd +1 , &rset , NULL, NULL, &timeout);
		if(nready < 0)
			perror("select");
		else if(nready <  0)
			printf("timeout for 3.5s \n");
		else
		{
		if(FD_ISSET(listenfd, &rset))//new incoming connection
		{
			printf("listening socket readable\n");
			int sleep_rand_sec = rand()%8;
			printf("  sleep %d  ", sleep_rand_sec);
			sleep(sleep_rand_sec);
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
			if(connfd >  maxfd)//compare to mark maxfd for select
				maxfd = connfd;

			if(i > maxi) //mark max in client[] array
				maxi = i;

			if(--nready <= 0) //no more fd to read , keep wait in accept
				continue;


		}//if FD_SSSET
		}
		for(i = 0; i <= maxi; i++)
		{
			if( (sockfd = client[i]) < 0)
				continue;//this is not a ready one 

			if(FD_ISSET(sockfd, &rset))
			{
				if(( n_have_read = p_read_from_p(sockfd ) )== 0)
				{
					//connection closed by other end
					close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i] = -1;
					continue;
				}
				printf("n_have_read = %d , client[%d] = %d \n", n_have_read, i, client[i]);
				p_write_to_p(connfd, "hi , this is server");
				if(--nready <= 0)
					break;//no more readable fd to handle keep wait in accetp
			}//FD_ISSET
		}//for

	}//for
}
*/
/* 
 * wait server version
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




