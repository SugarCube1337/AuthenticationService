#ifndef TEST_JWT_GENERATE_HANDLERS_TYPES_H
#define TEST_JWT_GENERATE_HANDLERS_TYPES_H

#include "request.h"
#include "response.h"

enum HandlerTypes_e {
    GET,
    POST,
    // other types
    UNSUPPORTED
};

typedef void (*serv_func_type)(const struct RequestParam_s *, struct ResponseParam_s *, const struct MongoData_s *);
#endif
