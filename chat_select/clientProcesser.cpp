#include "chat.h"

ssize_t p_read_from_p(int clientfd)
{
	if(0 == clientfd)
		return -1;
		
	Message msg_receive;
	ssize_t read_size = 0;//fd read_size	
	if(( read_size = read(clientfd, &msg_receive.msg_head, sizeof(msg_receive.msg_head))) <= 0)
	{
		perror("read header");
		return read_size;//connection closed by other end
	}

	if(( read_size += read(clientfd, &msg_receive.msg_content, msg_receive.msg_head.msg_length)) <= 0)
	{
		perror("read msg");
		return read_size;//connection closed by other end
	}

	printf(" ----> receive msg_content:\t %s \n", msg_receive.msg_content);
	fflush(stdout);
	return read_size;
}
ssize_t p_write_to_p(int clientfd, const char *msg)
{
	size_t slen = strlen(msg);
	if(slen > MAX_MSG_LENGTH || 0 == slen)
		return -1;
		
	Message msg_send;
	msg_send.msg_head.msg_length = slen;
	strncpy(msg_send.msg_content , msg, slen);
			
	ssize_t wn = write(clientfd, &msg_send ,sizeof(msg_send.msg_head)+slen );
	printf(" write finished msg:\t %s  \n",msg );
	return wn;
}
