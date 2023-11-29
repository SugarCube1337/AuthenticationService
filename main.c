// DATA
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

struct ThreadData_s {
    pthread_t threadIdCli;
    pthread_mutex_t stopper;
};

#define SET_WORK_TIMEOUT \
    struct timespec ts; \
    ts.tv_sec = 0; \
    ts.tv_nsec = 1000000000 / 2;

#define WORK_TIMEOUT \
    nanosleep(&ts, NULL);

/*--------------*/

// Child thread

int NeedQuit(pthread_mutex_t *mtx) {
    switch (pthread_mutex_trylock(mtx)) {
        case 0:
            pthread_mutex_unlock(mtx);
            return 1;
        case EBUSY:
            return 0;
        default:
            perror("Error locking mutex");
            exit(EXIT_FAILURE);
    }
}

void *Cli(void *arg) {
    struct ThreadData_s *threadData = (struct ThreadData_s *)arg;

    SET_WORK_TIMEOUT

    while (!NeedQuit(&threadData->stopper)) {
        // ...
        printf("Child thread is doing some work...\n");
        WORK_TIMEOUT
    }

    // Прощальное сообщение
    printf("Child thread says goodbye!\n");

    return NULL;
}

//--------------

// Main thread

int InitWorkThreads(struct ThreadData_s *threadData) {
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

    threadData.threadIdCli = pthread_self(); // Инициализация текущим потоком для предотвращения ошибки

    sigset_t sigset;
    int signo;

    sigemptyset(&sigset);
    sigaddset(&sigset, SIGQUIT);
    sigaddset(&sigset, SIGINT);
    sigaddset(&sigset, SIGTERM);

    sigprocmask(SIG_BLOCK, &sigset, NULL);

    int status = InitWorkThreads(&threadData);
    if (status == 1) {
        sigwait(&sigset, &signo);
        DestroyWorkThreads(&threadData);
    } else {
        fprintf(stderr, "Failed to initialize threads\n");
    }

    return 0;
}
