#ifndef MONGODATA_H
#define MONGODATA_H

#include <mongoc.h>

struct MongoData_s {
    mongoc_uri_t *uri;
    mongoc_client_t *client;
    mongoc_database_t *db;
    mongoc_collection_t *collServ;
    mongoc_collection_t *collUser;
};

#endif
