#ifndef TEST_JWT_GENERATE_QUERY_LIST_H
#define TEST_JWT_GENERATE_QUERY_LIST_H

#include <stdlib.h>
#include <string.h>

struct QueryListItem {
    char *param;
    char *value;
    struct QueryListItem *next;
};

struct QueryList {
    int count;
    struct QueryListItem *list;
};

struct QueryList * CreateQueryList();
void FreeQueryList(struct QueryList *data);

int AddToQueryList(struct QueryList *data, const char *paramName, const char *paramValue);
char * LookUpParamValue(struct QueryList *data, const char *paramName);

#endif
