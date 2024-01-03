#include "header_list.h"

struct HeaderList * CreateHeaderList() {
    struct HeaderList *list = (struct HeaderList *)malloc(sizeof(struct HeaderList));
    if (list == NULL) {
        /* error handler */
        return NULL;
    }
    list->count = 0;
    list->list = NULL;
    return list;
}

void FreeHeaderList(struct HeaderList *data) {
    if (data) {
        struct HeaderListItem *currentItem = data->list;
        struct HeaderListItem *remItem;

        while (currentItem != NULL) {
            remItem = currentItem;
            currentItem = currentItem->next;

            free(remItem->name);
            free(remItem->value);
            free(remItem);
        }
        free(data);
    }
}

int AddToHeaderList(struct HeaderList *data, const char *paramName, const char *paramValue) {
    struct HeaderListItem *newItem = malloc(sizeof(struct HeaderListItem));
    if (newItem == NULL) {
        /* error handler */
        return 0;
    }

    newItem->name = malloc(strlen(paramName) + 1);
    memset(newItem->name, 0, strlen(paramName) + 1);
    strcpy(newItem->name, paramName);

    newItem->value = malloc(strlen(paramValue) + 1);
    memset(newItem->value, 0, strlen(paramValue) + 1);
    strcpy(newItem->value, paramValue);

    newItem->next = data->list;
    data->list = newItem;
    data->count++;
    return 1;
}

char * LookUpHeaderValue(struct HeaderList *data, const char *headerName) {
    struct HeaderListItem *item;
    for (item = data->list; item != NULL; item = item->next) {
        if (strcmp(headerName, item->name) == 0) {
            return item->value; /* found! */
        }
    }
    return NULL;
}
