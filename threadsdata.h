#ifndef THREADSDATA_H
#define THREADSDATA_H

#include <pthread.h>

struct ThreadData_s {
    pthread_t threadIdCli;
    pthread_mutex_t stopper;
    pthread_t mainPid;
};

#endif

