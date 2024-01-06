#ifndef UTILS_H
#define UTILS_H

#include <pthread.h>

int NeedQuit(pthread_mutex_t *mtx);
int InputAvailable();
#define SET_WORK_TIMEOUT \
    struct timespec ts; \
    ts.tv_sec = 0; \
    ts.tv_nsec = 1000000 ;

#define WORK_TIMEOUT \
    nanosleep(&ts, NULL);

#endif
