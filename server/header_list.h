#ifndef TEST_JWT_GENERATE_HEADER_LIST_H
#define TEST_JWT_GENERATE_HEADER_LIST_H

#include <stdlib.h>
#include <string.h>

struct HeaderListItem {
    char *name;
    char *value;
    struct HeaderListItem *next;
};

struct HeaderList {
    int count;
    struct HeaderListItem *list;
};

struct HeaderList * CreateHeaderList();
void FreeHeaderList(struct HeaderList *data);

int AddToHeaderList(struct HeaderList *data, const char *paramName, const char *paramValue);
char * LookUpHeaderValue(struct HeaderList *data, const char *headerName);

#endif
