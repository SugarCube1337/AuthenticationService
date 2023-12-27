#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include "../threadsdata.h"
#include "../utils/utils.h"

#define PORT 7777
#define INSOCK_QUEUE_SIZE 256

void *NetInterface (void *arg) {
    struct ThreadData_s *threadData = (struct ThreadData_s*)arg;

    int listfd = 0;
    int connfd = 0;
    struct sockaddr_in saddr;

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(PORT);
    saddr.sin_addr.s_addr = htons(INADDR_ANY);

    //SET_WORKING_TIMEOUT

    listfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listfd == -1) {
        printf("socket() ret %d, %s\nstop child thread\n", errno, strerror(errno));
        kill(threadData->mainPid, SIGUSR1);
        pthread_exit(threadData);
    }

    /* set non-blocking socket option */
    fcntl(listfd, F_SETFL, O_NONBLOCK);

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

    char recBuf[1024];
    memset(recBuf, 0, sizeof(recBuf));

    char sendBuf[1024 + 16];
    memset(sendBuf, 0, sizeof(sendBuf));

    while (!NeedQuit(&threadData->stopper)) {

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

        if (memcmp(recBuf, "exit", 4) == 0) {
            kill(threadData->mainPid, SIGUSR1);
            close(connfd);
            break;
        }

        n = write(connfd, sendBuf, strlen(sendBuf));
        if (n == -1) {
            close(connfd);
            printf("write() ret %d, %s\n", errno, strerror(errno));
            kill(threadData->mainPid, SIGUSR1);
            break;
        }

        close(connfd);
        //WORKING_TIMEOUT
    }

    printf("stop child thread\n");
    close(listfd);
    pthread_exit(threadData);
}