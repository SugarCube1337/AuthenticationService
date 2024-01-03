#ifndef THREADSDATA_H
#define THREADSDATA_H

#include <pthread.h>
#include "database/mongodata.h"
#include "server/server.h"

struct ThreadData_s {
    int mainPid;
    pthread_mutex_t stopper;
    pthread_t threadIdCli;
    struct MongoData_s db;
    pthread_t threadIdNet;
    struct ServerData_s sd;
};

#endif
