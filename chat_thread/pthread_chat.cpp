#include "pthread_chat.h"

extern ssize_t p_read_from_p(int clientfd);
extern ssize_t p_write_to_p(int clientfd, const char *msg);

void thread_make(int i)
{
	void *thread_main(void *);
	pthread_create( &tptr[i].thread_tid, NULL, &thread_main, (void*)i);
}
void *thread_main(void *arg)
{
	int cfd, n_have_read;
	printf("thread %d starting \n", (int)arg);
	for( ; ;)
	{
		pthread_mutex_lock(&clifd_mutex);
		while(iget == iput)
			pthread_cond_wait(&clifd_cond, &clifd_mutex);

		cfd = clifd[iget];
		if(++iget == MAXNCLI)
			iget = 0;
		pthread_mutex_unlock(&clifd_mutex);
		tptr[(int) arg].thread_count++;

		 if(( n_have_read = p_read_from_p(cfd ) )== 0)
		 {
			 //connection closed by other end
			 close(cfd);
			 continue;
		 }
		 printf("n_have_read = %d , clifd = %d \n", n_have_read, cfd);
		 p_write_to_p(cfd, "hi , this is server");
		 close(cfd);
	}
}

