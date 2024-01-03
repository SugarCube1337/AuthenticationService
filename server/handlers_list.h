#ifndef TEST_JWT_GENERATE_HANDLERS_LIST_H
#define TEST_JWT_GENERATE_HANDLERS_LIST_H

#include <stdlib.h>
#include <string.h>

#include "handlers_types.h"

struct HandlersListItem {
    enum HandlerTypes_e type;
    serv_func_type f;
    char *path;
    struct HandlersListItem *next;
};

struct HandlersList {
    int count;
    struct HandlersListItem *list;
};

struct HandlersList * CreateHandlersList();
void FreeHandlersList(struct HandlersList *data);

int AddToHandlersList(struct HandlersList *data, enum HandlerTypes_e type, char *path, serv_func_type f);
serv_func_type LookUpHandler(struct HandlersList *data, enum HandlerTypes_e type, char *path);

#endif
