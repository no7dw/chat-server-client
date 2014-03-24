#include "chat.h"

extern int sock_client_init(const char*ipaddress, uint16_t server_port);

extern int p_write_to_p(int clientfd, const char *msg);
extern void p_read_from_p(int clientfd);

void chat_client(const char* linkin_ip)
{
	printf("chat client start %s \n", linkin_ip);
	int clientfd = sock_client_init(linkin_ip, SERVER_PORT);	
	if(clientfd < 0 )
	{		
		return ;
	}
	p_write_to_p(clientfd, "hello, this is client");
	p_read_from_p(clientfd);
}

