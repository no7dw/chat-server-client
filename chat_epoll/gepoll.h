#ifndef __G_EPOLL_H
#define __G_EPOLL_H

#include "chat.h"

struct myevent_s
{
        int fd; 
        int (*call_back)(int fd, int events, void *arg, int, myevent_s *);
        int events;
        void *arg;
        int status;//// 1: in epoll wait list, 0 not in
        Message msg;
        int len;
        long last_active;
};

#define MAX_EVENTS 5000

#endif
