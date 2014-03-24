//#include "chat.h"
#include <stdio.h>

void usage()
{
	printf("./chat& \t first chat login , in order to receive other client's message \n\
	./chat endpoint_ip_address \t start a client to chat with endpoint_ip_address \n");
}

extern void chat_server();
extern void chat_client(const char *linkin_ip);

int main(int argc, char **argv)
{
	if(argc == 1)
	{
		chat_server();
	}
	else if(argc == 2)
	{		
		chat_client(argv[1]);
	}	
	else
	{
		usage();
	}
	return 0;
}
	
