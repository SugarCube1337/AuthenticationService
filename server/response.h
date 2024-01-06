#ifndef AuthenticationService_RESPONSE_H
#define AuthenticationService_RESPONSE_H

#include <stdlib.h>
#include <string.h>

#define MAX_RESPONSE_SIZE 4096

#define OK 200
#define BAD_REQUEST 400
#define FORBIDDEN 401
#define INTERNAL_SERVER_ERROR 500
#define NOT_FOUND 404

struct ResponseParam_s {
    int status;
    char *contentType;
    char *body;
    // other params
};

struct ResponseParam_s *CreateEmptyResp();
void ReleaseResp(struct ResponseParam_s *rs);

void SetContentType(struct ResponseParam_s *rs, const char *contentType);
void ConstructStrResp(struct ResponseParam_s *rs, char *response);

#endif

