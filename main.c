// DATA

#include <pthread.h>

struct ThreadData_s {
    pthread_t threadIdCli;
    pthread_mutex_t stopper;
}

static struct ThreadData_s globTD;

#define SET_WORK_TIMEOUT \
    struct timespec ts; \
    ts.tv_sec = 0; \
    ts.tv_nsec = 1000000000 / 2;

#define WORK_TIMEOUT \
    nanosleep(&ts, NULL);

/*--------------*/

// child thread

int NeedQiut(pthread_mutex_t *mtx) {
    switch(pthread_mutex_trylock(mtx)) {
        case 0:
            pthread_mutex_unlock(mtx);
            return 1;
        case EBUSY:
            return 0;
        default:
            // log err...
            exit(-1);
    }
}

void *Cli(void *arg) {
    struct ThreadData_s *threadData = (struct ThreadData_s *)arg;

    SET_WORK_TIMEOUT

    while(!NeedQiut(&threadData->stopper)) {
        // ...
        WORK_TIMEOUT
    }
    // preint
}

//--------------

// main thread

int InitWorkThreds() {

    struct ThreadData_s *threadData = &globTD;

    if (pthread_mutex_lock(&threadData->stopper, NULL) != 0) {
        // ... log error
        // ... ret
    }

    pthread_create(&threadData->threadIdCli, NULL, Cli, threadData);

    return 1;
}


void DestroyWorkThreads() {
    if (pthread_mutex_unlock(&threadData->stopper) == EPERM) {
        // log
        return;
    }

    pthread_join(threadData->threadIdCli, NULL);
}


int main() {
    sigset_t sigset;
    int signo

    sigemptyset(&sigset);
    sigaddset(&sigset, SIGQUIT);
    sigaddset(&sigset, SIGINT);
    sigaddset(&sigset, SIGTERM);

    sigprocmask(SIG_BLOCK, &sigset, NULL);

    int status = InitWorkThreds();
    if (status == 1) {
        sigwait(&sigset, &signo);
        DestroyWorkThreads();
    }
    return 0;
}