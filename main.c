// DATA
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

struct ThreadData_s {
    pthread_t threadIdCli;
    pthread_mutex_t stopper;
    pthread_t mainPid;
};

#define SET_WORK_TIMEOUT \
    struct timespec ts; \
    ts.tv_sec = 0; \
    ts.tv_nsec = 100000000;

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

int InputAvailable() {
    struct timeval tv;
    fd_set fdSet;

    tv.tv_sec = 0;
    tv.tv_usec = 0;

    FD_ZERO(&fdSet);
    FD_SET(STDIN_FILENO, &fdSet);

    select(STDIN_FILENO + 1, &fdSet, NULL, NULL, &tv);
    return (FD_ISSET(0, &fdSet));

}

void *Cli(void *arg) {
    struct ThreadData_s *threadData = (struct ThreadData_s *) arg;

    SET_WORK_TIMEOUT

    char userInput[256];
    memset(userInput, 0, sizeof(userInput));
    setbuf(stdout, NULL);
    printf("> ");

    while (!NeedQuit(&threadData->stopper)) {
        if (InputAvailable()) {
            fgets(userInput, sizeof(userInput), stdin);

            if (memcmp(userInput, "exit", 4) == 0) {
                kill(threadData->mainPid, SIGUSR1);
                break;
            } else if (memcmp(userInput, "\n", 1) == 0) {
                printf("> ");
                memset(userInput, 0, sizeof(userInput));
            } else {
                printf("Unknown command: %s", userInput);
                printf("> ");
                memset(userInput, 0, sizeof(userInput));
            }
        }

        WORK_TIMEOUT
    }

    // Прощальное сообщение
    printf("Child thread says goodbye!\n");

    return NULL;
}

//--------------

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

    sigset_t sigset;
    int signo;

    sigemptyset(&sigset);
    sigaddset(&sigset, SIGQUIT);
    sigaddset(&sigset, SIGINT);
    sigaddset(&sigset, SIGTERM);
    sigaddset(&sigset, SIGUSR1);

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