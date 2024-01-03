#include "response.h"

struct ResponseParam_s *CreateEmptyResp() {
    struct ResponseParam_s * rs = malloc(sizeof(struct ResponseParam_s));
    rs->status = 0;
    rs->body = NULL;
    rs->contentType = NULL;
    return rs;
}

void ReleaseResp(struct ResponseParam_s *rs) {
    if (rs->body != NULL) {
        free(rs->body);
    }
    if (rs->contentType != NULL) {
        free(rs->contentType);
    }
    free(rs);
}

char *GetStatusMessage(int status) {
    switch (status) {
        case OK:
            return "OK";
        case BAD_REQUEST:
            return "Bad Request";
        case INTERNAL_SERVER_ERROR:
            return "Internal Server Error";
    }
}

void ConstructStrResp(struct ResponseParam_s *rs, char *response) {
    char *r = strdup("HTTP/1.0 "); // create local heap copy of constant string

    if (rs->status == 0) {
        asprintf(&r, "%d ", INTERNAL_SERVER_ERROR);
        asprintf(&r, "%s %s\r\n\r\n", r, GetStatusMessage(INTERNAL_SERVER_ERROR));
        strncpy(response, r, MAX_RESPONSE_SIZE);

    } else {
        asprintf(&r, "%d ", rs->status);
        asprintf(&r, "%s %s\r\n", r, GetStatusMessage(rs->status));
        if (rs->contentType != NULL) {
            asprintf(&r, "%sContent-Type: %s\r\n", r, rs->contentType);
        }
        // ...other headers...

        asprintf(&r, "%s\r\n", r); // add empty line

        if (rs->body != NULL) {
            asprintf(&r, "%s%s", r, rs->body);
        }
        strncpy(response, r, MAX_RESPONSE_SIZE);
    }

    free(r);
}
