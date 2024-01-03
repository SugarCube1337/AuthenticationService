#include "query_list.h"

struct QueryList * CreateQueryList() {
    struct QueryList *list = (struct QueryList *)malloc(sizeof(struct QueryList));
    if (list == NULL) {
        /* error handler */
        return NULL;
    }
    list->count = 0;
    list->list = NULL;
    return list;
}

void FreeQueryList(struct QueryList *data) {
    if (data) {
        struct QueryListItem *currentItem = data->list;
        struct QueryListItem *remItem;

        while (currentItem != NULL) {
            remItem = currentItem;
            currentItem = currentItem->next;

            free(remItem->param);
            free(remItem->value);
            free(remItem);
        }
        free(data);
    }
}

int AddToQueryList(struct QueryList *data, const char *paramName, const char *paramValue) {
    struct QueryListItem *newItem = malloc(sizeof(struct QueryListItem));
    if (newItem == NULL) {
        /* error handler */
        return 0;
    }

    newItem->param = malloc(strlen(paramName) + 1);
    memset(newItem->param, 0, strlen(paramName) + 1);
    strcpy(newItem->param, paramName);

    newItem->value = malloc(strlen(paramValue) + 1);
    memset(newItem->value, 0, strlen(paramValue) + 1);
    strcpy(newItem->value, paramValue);

    newItem->next = data->list;
    data->list = newItem;
    data->count++;
    return 1;
}

char * LookUpParamValue(struct QueryList *data, const char *paramName) {
    struct QueryListItem *item;
    for (item = data->list; item != NULL; item = item->next) {
        if (strcmp(paramName, item->param) == 0) {
            return item->value; /* found! */
        }
    }
    return NULL;
}
