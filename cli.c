#include <signal.h>
#include <stdio.h>
#include <string.h>
#include "cli.h"
#include "utils.h"


// Command interface
typedef struct {
    void (*execute)(struct ThreadData_s *threadData);
} Command;

void exitCommand(struct ThreadData_s *threadData) {
    kill(threadData->mainPid, SIGUSR1);
}

void helpCommand() {
    printf("Available commands:\n"
           "adduser - add new user\n"
           "help - shows information about commands\n"
           "exit - stops the program\n"
           ">");
}

void AddUser() {
}

//---------------------------------------------------//

void *Cli(void *arg) {
    struct ThreadData_s *threadData = (struct ThreadData_s *) arg;

    SET_WORK_TIMEOUT

    char userInput[256];
    memset(userInput, 0, sizeof(userInput));
    setbuf(stdout, NULL);
    printf("> ");

    // Define commands
    Command exitCmd = {exitCommand};
    Command helpCmd = {helpCommand};

    while (!NeedQuit(&threadData->stopper)) {
        if (InputAvailable()) {
            fgets(userInput, sizeof(userInput), stdin);

            if (memcmp(userInput, "exit", 4) == 0) {
                exitCmd.execute(threadData);
                break;
            } else if (memcmp(userInput, "help", 4) == 0) {
                helpCmd.execute(threadData);
            } else if (memcmp(userInput, "\n", 1) == 0) {
                printf("> ");
                memset(userInput, 0, sizeof(userInput));
            } else {
                printf("Unknown command: %s", userInput);
                printf("Type 'help' for available commands.\n");
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
