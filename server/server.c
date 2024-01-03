#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include "../utils/utils.h"

#define PORT 7777
#define INSOCK_QUEUE_SIZE 256

// --------- server work logic in handlers

void AskToken(const struct RequestParam_s *request, struct ResponseParam_s *response) {

}

void ValidateToken(const struct RequestParam_s *request, struct ResponseParam_s *response) {

}

// --------- internal server functions

void RegisterMethod(struct ServerData_s *sd, enum HandlerTypes_e type, char *path, serv_func_type f) {
    AddToHandlersList(sd->handlres, type, path, f);
}

void RunMethod(struct ServerData_s *sd, const char *request, char *response) {
    struct RequestParam_s *rq = ParseParams(request);
    struct ResponseParam_s *rs = CreateEmptyResp();

    serv_func_type f = LookUpHandler(sd->handlres, rq->type, rq->path);

    if (f == NULL) { // no registered method? or t is UNSUPPORTED? or p is unknown?
        rs->status = BAD_REQUEST;
    } else {
        f(rq, rs); // else run registered method
    }

    ConstructStrResp(rs, response);
    ReleaseResp(rs);
    ReleaseReq(rq);
}

void *NetInterface (void *arg) {
    struct ThreadData_s *threadData = (struct ThreadData_s*)arg;

    threadData->sd.handlres = CreateHandlersList();
    if (threadData->sd.handlres == NULL) {
        printf("CreateHandlersList() error\n");
        kill(threadData->mainPid, SIGUSR1);
        pthread_exit(threadData);
    }
    RegisterMethod(&threadData->sd, GET, "/token", AskToken);
    RegisterMethod(&threadData->sd, POST, "/validate", ValidateToken);

    int listfd = 0;
    int connfd = 0;
    struct sockaddr_in saddr;

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(PORT);
    saddr.sin_addr.s_addr = htons(INADDR_ANY);

    listfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listfd == -1) {
        printf("socket() ret %d, %s\nstop child thread\n", errno, strerror(errno));
        kill(threadData->mainPid, SIGUSR1);
        pthread_exit(threadData);
    }

    // set non-blocking
    int flags;
    if ((flags = fcntl(listfd, F_GETFL, 0)) < 0) {
        close(listfd);

        printf("fcntl(F_GETFL) ret %d, %s\n", errno, strerror(errno));
        kill(threadData->mainPid, SIGUSR1);
        pthread_exit(threadData);
    }
    if (fcntl(listfd, F_SETFL, flags | O_NONBLOCK) < 0) {
        close(listfd);

        printf("fcntl(F_SETFL) ret %d, %s\n", errno, strerror(errno));
        kill(threadData->mainPid, SIGUSR1);
        pthread_exit(threadData);
    }

    if(bind(listfd, (const struct sockaddr *) &saddr, sizeof(saddr)) == -1) {
        close(listfd);

        printf("bind() ret %d, %s\nstop child thread\n", errno, strerror(errno));
        kill(threadData->mainPid, SIGUSR1);
        pthread_exit(threadData);
    }

    if (listen(listfd, INSOCK_QUEUE_SIZE) == -1) {
        close(listfd);

        printf("listen() ret %d %s\nstop child thread\n", errno, strerror(errno));
        kill(threadData->mainPid, SIGUSR1);
        pthread_exit(threadData);
    }

    // create epoll event for listening
    int epollfd = epoll_create(1);
    if (epollfd == -1) {
        printf("epoll_create() ret %d, %s\n", errno, strerror(errno));
        exit(1);
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = listfd;

    // add event settings to epoll
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listfd, &ev) == -1) {
        printf("epoll_ctl ret %d, %s\n", errno, strerror(errno));
        exit(1);
    }

    struct epoll_event detectedEvent;
    memset((void *)&detectedEvent, 0, sizeof(struct epoll_event));

    char recBuf[MAX_REQUEST_SIZE];
    char sendBuf[MAX_RESPONSE_SIZE];

    while (!NeedQuit(&threadData->stopper)) {
        int te = 0; // triggered event

        // waiting incoming event
        int nfds = epoll_wait(epollfd, &detectedEvent, INSOCK_QUEUE_SIZE, 500);
        if (nfds == -1) {
            printf("error calling epoll: %d %s\n", errno, strerror(errno));
            pthread_exit(threadData);
        }

        for (te = 0; te < nfds; te++) {
            struct epoll_event *event = &detectedEvent + te;

            if (event->data.fd == listfd) { // new connection
                memset(recBuf, 0, sizeof(recBuf));
                memset(sendBuf, 0, sizeof(sendBuf));

                connfd = accept(listfd, NULL, NULL);
                if(connfd == -1) {
                    continue;
                }

                ssize_t n = read(connfd, recBuf, sizeof(recBuf));
                if(n == -1) {
                    close(connfd);
                    printf("read() ret %d, %s\n", errno, strerror(errno));
                    continue;
                }

                snprintf(sendBuf, sizeof(sendBuf), "server got msg: %s\n", recBuf);
                printf("%s", sendBuf);

                RunMethod(&threadData->sd, recBuf, sendBuf);

                n = write(connfd, sendBuf, strlen(sendBuf));
                if (n == -1) {
                    close(connfd);
                    printf("write() ret %d, %s\n", errno, strerror(errno));
                    kill(threadData->mainPid, SIGUSR1);
                    break;
                }

                close(connfd);

            } // if our connection
        } // for te
    } // while !NeedQuit()

    printf("stop child thread\n");

    close(listfd);
    FreeHandlersList(threadData->sd.handlres);
    pthread_exit(threadData);
}