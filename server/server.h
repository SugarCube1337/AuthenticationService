#ifndef TEST_JWT_GENERATE_SERVER_H
#define TEST_JWT_GENERATE_SERVER_H

#define PORT 7777
#define INSOCK_QUEUE_SIZE 256

#include "handlers_list.h"

struct ServerData_s {
    struct HandlersList *handlres;
};

void *NetInterface (void *arg);

#endif
