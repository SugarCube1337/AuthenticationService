#include "request.h"
#include "handlers_types.h"

struct RequestParam_s *CreateEmptyReq() {
    struct RequestParam_s *rq = malloc(sizeof(struct RequestParam_s));
    rq->type = UNSUPPORTED;
    rq->path = NULL;
    rq->body = NULL;
    rq->headers = NULL;
    rq->queryParams = NULL;
    return rq;
}

void ReleaseReq(struct RequestParam_s *rq) {
    if (rq->path != NULL) {
        free(rq->path);
    }
    if (rq->body != NULL) {
        free(rq->body);
    }
    FreeHeaderList(rq->headers);
    FreeQueryList(rq->queryParams);
    free(rq);
}


enum HandlerTypes_e GetRequestType(const char *request) {
    char *subStr;
    char *localRequestCopy = strdup(request);

    // split request on several sub-strings
    subStr = strtok(localRequestCopy, " ");
    if (subStr == NULL) {
        return UNSUPPORTED;
    }
    if (strncmp(subStr, "GET", 3) == 0) {
        free(localRequestCopy);  // here subStr will be destroyed too
        return GET;
    }
    if (strncmp(subStr, "POST", 4) == 0) {
        free(localRequestCopy);  // here subStr will be destroyed too
        return POST;
    }
    free(localRequestCopy);  // here subStr will be destroyed too
    return UNSUPPORTED;
}

char *GetRequestPath(const char *request) {
    char *subStr;
    char *delimiter = " ";
    char *localRequestCopy = strdup(request);

    // split request on several sub-strings
    subStr = strtok(localRequestCopy, delimiter);
    while (subStr != NULL) {
        if (subStr[0] == '/') { // check if it is a path

            // if we have ?query_str, cut it off
            char *questionMarkPos = strchr(subStr, '?'); // find char position in str
            if (questionMarkPos) {
                *questionMarkPos = '\0';
            }

            char *retPath = malloc(strlen(subStr) + 1);
            memset(retPath, 0, strlen(subStr) + 1);
            strcpy(retPath, subStr);
            free(localRequestCopy); // here subStr will be destroyed too
            return retPath;
        }
        // go to next sub-str
        subStr = strtok(NULL, delimiter);
    }
    free(localRequestCopy);  // here subStr will be destroyed too
    return "Unknown";
}

struct QueryList *GetQueries(const char *request) {
    struct QueryList *q = CreateQueryList();

    char *scratch, *subStr;
    int subStrCounter = 0;

    char *queryStr, *queryScratch;

    char *queryParam, *queryValue;
    int queryParamCounter = 0;

    char *localRequestCopy = strdup(request);
    // GET /path?query_string HTTP/1.0\r\n\r\n
    while ((subStr = strtok_r(!subStrCounter ? localRequestCopy : NULL, " ", &scratch))) {
        subStrCounter++;
        if ((queryStr = strchr(subStr, '?'))) {
            queryStr++;
            while ((queryParam = strtok_r(!queryParamCounter ? queryStr : NULL, "&", &queryScratch))) {
                queryParamCounter++;
                queryValue = strchr(queryParam, '=');
                *queryValue = '\0';
                queryValue++;
                AddToQueryList(q, queryParam, queryValue);
            }
            free(localRequestCopy);
            break;
        }
    }
    return q;
}

struct HeaderList *GetHeaders(const char *request) {
    struct HeaderList *h = CreateHeaderList();
    char *localRequestCopy = strdup(request);

    // skip first line
    char *requestPtr = strchr(localRequestCopy, '\n'); // find char position in str
    requestPtr++;

    // empty str separates headers from body
    while (requestPtr[0] != '\r' && requestPtr[1] != '\n') {
        // name
        size_t nameLen = strcspn(requestPtr, ":"); // calc distance between current position and :
        char *headerName = requestPtr;
        *(requestPtr + nameLen) = '\0';

        requestPtr += nameLen + 1; // move :
        while (*requestPtr == ' ') {
            requestPtr++;
        }

        // value
        size_t valueLen = strcspn(requestPtr, "\r\n"); // calc distance between current position and \r or \n
        char *headerValue = requestPtr;
        *(requestPtr + valueLen) = '\0';

        requestPtr += valueLen + 2; // move \r\n

        AddToHeaderList(h, headerName, headerValue);
    }

    free(localRequestCopy);
    return h;
}

char *GetBody(const char *request, int contentLength) {
    char *bodyStartIndex = strstr(request, "\r\n\r\n"); // find str inside bigger str
    if (bodyStartIndex == NULL) {
        return NULL;
    }
    bodyStartIndex += 4;

    // empty body
    if (contentLength == -1 && ( (request + strlen(request)) == bodyStartIndex) ) {
        return NULL;
    }

    char *body;

    // have body & contentLength
    if (contentLength != -1) {
        body = malloc(contentLength + 1);
        memset(body, 0, contentLength + 1);
        strncpy(body, bodyStartIndex, contentLength);
        return body;
    }

    // have only body
    char *bodyEndIndex = strstr(bodyStartIndex, "\r\n\r\n");
    if (bodyEndIndex == NULL) {
        // wrong body
        return NULL;
    }
    bodyEndIndex += 4;
    size_t bodyLen = bodyEndIndex - bodyStartIndex;
    body = malloc(bodyLen + 1);
    memset(body, 0, bodyLen + 1);
    strncpy(body, bodyStartIndex, bodyLen);
    return body;
}

struct RequestParam_s *ParseParams(const char *request) {
    struct RequestParam_s *rq = CreateEmptyReq();


    rq->type = GetRequestType(request);
    if (rq->type == UNSUPPORTED) {
        return rq; // empty rq
    }

    rq->path = GetRequestPath(request);
    rq->queryParams = GetQueries(request);
    rq->headers = GetHeaders(request);

    int bodyLen = -1;
    char *bodyLenAsStr = LookUpHeaderValue(rq->headers, "Content-Length");
    if (bodyLenAsStr != NULL) {
        bodyLen = atoi(bodyLenAsStr);
    }
    rq->body = GetBody(request, bodyLen);

    return rq;
}
