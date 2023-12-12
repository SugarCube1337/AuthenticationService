// DATA
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "cli.h"
#include "database.h"

// Main thread

int InitWorkThreads(struct ThreadData_s *threadData) {

    // Инициализация мьютекса
    if (pthread_mutex_init(&threadData->stopper, NULL) != 0) {
        perror("Error initializing mutex for thread");
        return 0;
    }

    if (pthread_mutex_lock(&threadData->stopper) != 0) {
        perror("Error locking mutex for thread initialization");
        return 0;  // Возвращаем 0
    }

    if (pthread_create(&threadData->threadIdCli, NULL, Cli, threadData) != 0) {
        perror("Error creating thread");
        pthread_mutex_unlock(&threadData->stopper);  // Разблокировать мьютекс перед выходом
        return 0;
    }

    return 1;
}


void DestroyWorkThreads(struct ThreadData_s *threadData) {

    if (pthread_mutex_unlock(&threadData->stopper) == EPERM) {
        perror("Error unlocking mutex");
        return;
    }

    if (pthread_join(threadData->threadIdCli, NULL) != 0) {
        perror("Error joining thread");
    }

}

int main() {
    struct ThreadData_s threadData;
    threadData.mainPid = getpid();
    sigset_t sigset;
    int signo, status;

    status = InitDb(&threadData);
    if (status == 0) {
        exit(1);
    }

    sigemptyset(&sigset);
    sigaddset(&sigset, SIGQUIT);
    sigaddset(&sigset, SIGINT);
    sigaddset(&sigset, SIGTERM);
    sigaddset(&sigset, SIGUSR1);

    sigprocmask(SIG_BLOCK, &sigset, NULL);

    status = InitWorkThreads(&threadData);
    if (status == 1) {
        sigwait(&sigset, &signo);
        DestroyWorkThreads(&threadData);
    } else {
        fprintf(stderr, "Failed to initialize threads\n");
    }
    DestroyDb(&threadData);

    printf("Main thread says goodbye!\n");
    return 0;
}