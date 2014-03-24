#ifndef __PTHREAD_CHAT_H
#define __PTHREAD_CHAT_H
#include "chat.h"
#include <pthread.h>

#define maxthreads 10
#define MAXNCLI 2000
typedef struct 
{
	    pthread_t thread_tid;
		long thread_count;
} Thread;
Thread * tptr;//array of Thread struct; will call calloc to set point

int clifd[MAXNCLI], iget, iput;
pthread_mutex_t clifd_mutex;
pthread_cond_t clifd_cond;

void thread_make(int i);
void *thread_main(void *arg);

#endif

