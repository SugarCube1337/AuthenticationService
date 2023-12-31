#include <signal.h>
#include <stdio.h>
#include <string.h>
#include "cli.h"
#include "../utils/utils.h"


// Command interface
typedef struct {
    void (*execute)(struct ThreadData_s *threadData);
} Command;

void exitCommand(struct ThreadData_s *threadData) {
    kill(threadData->mainPid, SIGUSR1);
}

void helpCommand() {
    printf("Available commands:\n"
           "help - shows information about commands\n"
           "adduser - add new user\n"
           "showusers - shows all information about user\n"
           "delusers - delete user by it's ...\n"
           "updateuser - update user's information by it's ...\n"
           "addservice - add new service\n"
           "showservices - shows all information about services\n"
           "delservices - delete service by it's ...\n"
           "exit - stops the program\n");
}

void AddService(struct ThreadData_s *threadData) {
    bson_error_t error;
    char userInput[1024];
    memset(userInput, 0, sizeof(userInput));

    SET_WORK_TIMEOUT

    printf("Enter service name: ");
    while (!NeedQuit(&threadData->stopper)) {
        if (InputAvailable()) {
            fgets(userInput, sizeof(userInput), stdin);
            userInput[strlen(userInput) - 1] = '\0'; // rem '\n'
            break;
        }
        WORK_TIMEOUT
    }

    bson_t *createdServDoc = bson_new();
    bson_oid_t oid;
    bson_oid_init(&oid, NULL);
    BSON_APPEND_OID(createdServDoc, "_id", &oid);
    BSON_APPEND_UTF8(createdServDoc, "service", userInput);

    if (!mongoc_collection_insert_one(threadData->db.collServ, createdServDoc, NULL, NULL, &error)) {
        fprintf(stderr, "mongoc_collection_insert_one() : %s\n", error.message);
    }

    printf("Ok\n");
    bson_destroy(createdServDoc);
}

void ShowServices(struct ThreadData_s *threadData) {
    const bson_t *foundDoc;
    bson_t *query = bson_new();
    mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(threadData->db.collServ, query, NULL, NULL);

    while (mongoc_cursor_next(cursor, &foundDoc)) {
        char *str = bson_as_canonical_extended_json(foundDoc, NULL);
        printf("%s\n", str);
        bson_free(str);
    }

    bson_destroy(query);
    mongoc_cursor_destroy(cursor);
}

void DelService(struct ThreadData_s *threadData) {
    bson_error_t error;
    char userInput[1024];
    memset(userInput, 0, sizeof(userInput));

    SET_WORK_TIMEOUT

    printf("Enter service OID: ");
    while (!NeedQuit(&threadData->stopper)) {
        if (InputAvailable()) {
            fgets(userInput, sizeof(userInput), stdin);
            userInput[strlen(userInput) - 1] = '\0'; // rem '\n'
            break;
        }
        WORK_TIMEOUT
    }

    bson_oid_t oid;
    bson_t *deletedDoc = bson_new();

    bson_oid_init_from_string(&oid, userInput);

    BSON_APPEND_OID (deletedDoc, "_id", &oid);
    if (!mongoc_collection_delete_one(
            threadData->db.collServ, deletedDoc, NULL, NULL, &error)) {
        fprintf(stderr, "Delete failed: %s\n", error.message);
        exit(1);
    }

    printf("Ok\n");
    bson_destroy(deletedDoc);
}


void addUser(struct ThreadData_s *threadData) {
    bson_error_t error;
    char userInput[1024];
    memset(userInput, 0, sizeof(userInput));

    SET_WORK_TIMEOUT

    printf("Enter user name: ");
    while (!NeedQuit(&threadData->stopper)) {
        if (InputAvailable()) {
            fgets(userInput, sizeof(userInput), stdin);
            userInput[strlen(userInput)-1] = '\0'; // rem '\n'
            break;
        }
        WORK_TIMEOUT
    }
    bson_t *createdUserDoc = bson_new();
    bson_oid_t oid;
    bson_oid_init(&oid, NULL);
    BSON_APPEND_OID(createdUserDoc, "_id", &oid);
    BSON_APPEND_UTF8(createdUserDoc, "username", userInput);

    if (!mongoc_collection_insert_one(threadData->db.collUser, createdUserDoc, NULL, NULL, &error)) {
        fprintf(stderr, "mongoc_collection_insert_one() : %s\n", error.message);
    }

    printf("Ok\n");
    bson_destroy(createdUserDoc);
}


void updateUser(struct ThreadData_s *threadData) {
    bson_error_t error;
    char userInput[1024];
    memset(userInput, 0, sizeof(userInput));

    SET_WORK_TIMEOUT

    printf("Enter user OID: ");
    while (!NeedQuit(&threadData->stopper)) {
        if (InputAvailable()) {
            fgets(userInput, sizeof(userInput), stdin);
            userInput[strlen(userInput) - 1] = '\0'; // remove '\n'
            break;
        }
        WORK_TIMEOUT
    }

    printf("User OID: %s\n", userInput);

    bson_oid_t userOid;
    bson_oid_init_from_string(&userOid, userInput);

    bson_t *query = bson_new();
    BSON_APPEND_OID(query, "_id", &userOid);

    const bson_t *foundDoc;
    mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(threadData->db.collUser, query, NULL, NULL);

    if (!mongoc_cursor_next(cursor, &foundDoc)) {
        printf("User not found.\n");
        bson_destroy(query);
        mongoc_cursor_destroy(cursor);
        return;
    }

    printf("User found.\n");

    printf("Do you want to add or remove a service? (add/remove): ");
    while (!NeedQuit(&threadData->stopper)) {
        if (InputAvailable()) {
            fgets(userInput, sizeof(userInput), stdin);
            userInput[strlen(userInput) - 1] = '\0'; // remove '\n'
            break;
        }
        WORK_TIMEOUT
    }

    printf("Option: %s\n", userInput);

    if (strcmp(userInput, "add") == 0) {
        printf("Enter service name to add: ");
        while (!NeedQuit(&threadData->stopper)) {
            if (InputAvailable()) {
                fgets(userInput, sizeof(userInput), stdin);
                userInput[strlen(userInput) - 1] = '\0'; // remove '\n'
                break;
            }
            WORK_TIMEOUT
        }

        printf("Adding service: %s\n", userInput);

        bson_t *update = BCON_NEW("$addToSet", "{", "services", BCON_UTF8(userInput), "}");
        if (!mongoc_collection_update_one(threadData->db.collUser, query, update, NULL, NULL, &error)) {
            fprintf(stderr, "Update failed: %s\n", error.message);
        }
        bson_destroy(update);
        printf("Service added.\n");
    } else if (strcmp(userInput, "remove") == 0) {
        printf("Enter service name to remove: ");
        while (!NeedQuit(&threadData->stopper)) {
            if (InputAvailable()) {
                fgets(userInput, sizeof(userInput), stdin);
                userInput[strlen(userInput) - 1] = '\0'; // remove '\n'
                break;
            }
            WORK_TIMEOUT
        }

        printf("Removing service: %s\n", userInput);

        bson_t *update = BCON_NEW("$pull", "{", "services", BCON_UTF8(userInput), "}");
        if (!mongoc_collection_update_one(threadData->db.collUser, query, update, NULL, NULL, &error)) {
            fprintf(stderr, "Update failed: %s\n", error.message);
        }
        bson_destroy(update);
        printf("Service removed.\n");
    } else {
        printf("Invalid option.\n");
    }

    bson_destroy(query);
    mongoc_cursor_destroy(cursor);
}


void showUsers(struct ThreadData_s *threadData) {
    const bson_t *foundDoc;
    bson_t *query = bson_new();
    mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(threadData->db.collUser, query, NULL, NULL);

    while (mongoc_cursor_next(cursor, &foundDoc)) {
        char *str = bson_as_canonical_extended_json(foundDoc, NULL);
        printf("%s\n", str);
        bson_free(str);
    }

    bson_destroy(query);
    mongoc_cursor_destroy(cursor);
}

void deleteUser(struct ThreadData_s *threadData) {
    bson_error_t error;
    char userInput[1024];
    memset(userInput, 0, sizeof(userInput));

    SET_WORK_TIMEOUT

    printf("Enter user OID: ");
    while (!NeedQuit(&threadData->stopper)) {
        if (InputAvailable()) {
            fgets(userInput, sizeof(userInput), stdin);
            userInput[strlen(userInput)-1] = '\0'; // rem '\n'
            break;
        }
        WORK_TIMEOUT
    }

    bson_oid_t oid;
    bson_t *deletedDoc = bson_new();

    bson_oid_init_from_string(&oid, userInput);

    BSON_APPEND_OID(deletedDoc, "_id", &oid);

    if (!mongoc_collection_delete_one(
            threadData->db.collUser, deletedDoc, NULL, NULL, &error)) {
        fprintf(stderr, "Delete failed: %s\n", error.message);
        exit(1);
    }

    printf("Ok\n");
    bson_destroy(deletedDoc);
}

//---------------------------------------------------//

    void *Cli(void *arg) {
        struct ThreadData_s *threadData = (struct ThreadData_s *) arg;

        SET_WORK_TIMEOUT

        char userInput[256];
        memset(userInput, 0, sizeof(userInput));

        // Define commands
        Command exitCmd = {exitCommand};
        Command helpCmd = {helpCommand};
        Command addUserCmd = {addUser};
        Command addServCmd = {AddService};
        Command showServCmd = {ShowServices};
        Command delServCmd = {DelService};
        Command delUserCmd = {deleteUser};
        Command showUsersCmd = {showUsers};
        Command updateUserCmd = {updateUser};

        setbuf(stdout, NULL);
        printf("Type 'help' for available commands.\n");
        printf("> ");

        while (!NeedQuit(&threadData->stopper)) {
            if (InputAvailable()) {
                fgets(userInput, sizeof(userInput), stdin);

                if (memcmp(userInput, "\n", 1) == 0) {
                    //printf("> ");
                } else if (memcmp(userInput, "exit", 4) == 0) {
                    exitCmd.execute(threadData);
                    break;
                } else if (memcmp(userInput, "help", 4) == 0) {
                    helpCmd.execute(threadData);
                } else if (memcmp(userInput, "adduser", 7) == 0) {
                    addUserCmd.execute(threadData);
                } else if (memcmp(userInput, "updateuser", 8) == 0) {
                    updateUserCmd.execute(threadData);
                } else if (memcmp(userInput, "deluser", 7) == 0) {
                    delUserCmd.execute(threadData);
                } else if (memcmp(userInput, "addservice", 10) == 0) {
                    addServCmd.execute(threadData);
                } else if (memcmp(userInput, "delservice", 10) == 0) {
                    delServCmd.execute(threadData);
                } else if (memcmp(userInput, "showusers", 9) == 0) {
                    showUsersCmd.execute(threadData);
                } else if (memcmp(userInput, "showservices", 12) == 0) {
                    showServCmd.execute(threadData);
                } else {
                    printf("Unknown command: %s", userInput);
                    printf("Type 'help' for available commands.\n");

                }
                printf("> ");
                memset(userInput, 0, sizeof(userInput));
            }

            WORK_TIMEOUT
        }

        // Прощальное сообщение
        printf("Child thread says goodbye!\n");
        return NULL;
    }
