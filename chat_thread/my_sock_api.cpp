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
