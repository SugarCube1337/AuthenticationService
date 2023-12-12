#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <asm-generic/errno-base.h>


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