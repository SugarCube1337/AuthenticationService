#ifndef TEST_JWT_GENERATE_REQUEST_H
#define TEST_JWT_GENERATE_REQUEST_H

#include <stdlib.h>

#include "query_list.h"
#include "header_list.h"

#define MAX_REQUEST_SIZE 4096

struct RequestParam_s {
    struct HeaderList *headers;
    struct QueryList *queryParams;
    int type;
    char *path;
    char *body;
};

struct RequestParam_s *CreateEmptyReq();
void ReleaseReq(struct RequestParam_s *rq);

struct RequestParam_s *ParseParams(const char *request);

#endif
