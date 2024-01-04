#include "handlers_list.h"

struct HandlersList * CreateHandlersList() {
    struct HandlersList *list = (struct HandlersList *)malloc(sizeof(struct HandlersList));
    if (list == NULL) {
        /* error handler */
        return NULL;
    }
    list->count = 0;
    list->list = NULL;
    return list;
}

void FreeHandlersList(struct HandlersList *data) {
    if (data) {
        struct HandlersListItem *currentItem = data->list;
        struct HandlersListItem *remItem;

        while (currentItem != NULL) {
            remItem = currentItem;
            currentItem = currentItem->next;

            free(remItem->path);
            free(remItem);
        }
        free(data);
    }
}

int AddToHandlersList(struct HandlersList *data, enum HandlerTypes_e type, char *path, serv_func_type f) {
    struct HandlersListItem *newItem = malloc(sizeof(struct HandlersListItem));
    if (newItem == NULL) {
        /* error handler */
        return 0;
    }
    newItem->type = type;
    newItem->f = f;
    newItem->path = malloc(strlen(path) + 1);
    memset(newItem->path, 0, strlen(path) + 1);
    strcpy(newItem->path, path);

    newItem->next = data->list;
    data->list = newItem;
    data->count++;
    return 1;
}

serv_func_type LookUpHandler(struct HandlersList *data, enum HandlerTypes_e type, char *path) {
    struct HandlersListItem *item;
    for (item = data->list; item != NULL; item = item->next) {
        if (type == item->type) {
            if (strlen(path) == strlen(item->path)) {
                if (strncmp(path, item->path, strlen(item->path)) == 0) {
                    return item->f; /* found! */
                }
            }
        }
    }
    return NULL;
}