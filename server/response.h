#ifndef TEST_JWT_GENERATE_RESPONSE_H
#define TEST_JWT_GENERATE_RESPONSE_H

#include <stdlib.h>
#include <string.h>

#define MAX_RESPONSE_SIZE 4096

#define OK 200
#define BAD_REQUEST 400
#define INTERNAL_SERVER_ERROR 500

struct ResponseParam_s {
    int status;
    char *contentType;
    char *body;
    // other
};

struct ResponseParam_s *CreateEmptyResp();
void ReleaseResp(struct ResponseParam_s *rs);

void ConstructStrResp(struct ResponseParam_s *rs, char *response);

#endif
