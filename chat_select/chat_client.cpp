#include "chat.h"

extern int sock_client_init(const char*ipaddress, uint16_t server_port);

extern ssize_t p_write_to_p(int clientfd, const char *msg);
extern ssize_t p_read_from_p(int clientfd);
void chat_client(const char* linkin_ip)
{
	printf("chat client start %s \n", linkin_ip);
	int clientfd = sock_client_init(linkin_ip, SERVER_PORT);	
	if(clientfd < 0 )
	{		
		return ;
	}
	ssize_t iw = p_write_to_p(clientfd, "hello, this is client");
	printf(" iwrite =  %d \n", iw);
	ssize_t ir = p_read_from_p(clientfd);
	printf(" ireadback =  %d \n", ir);
	if( ir == 0)
		printf(" connection close by server\n");
}

