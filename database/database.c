#include "database.h"
#include "../threadsdata.h"
#include <stdio.h>

const char *uriStr = "mongodb://root:password@localhost:27017";
const char *dbName = "servdb";
const char *collServName = "servcol";
const char *collUserName = "usercol";

mongoc_collection_t *GetCollectionHandle(mongoc_database_t *db, mongoc_client_t *client, const char *collectionName) {
    bson_error_t error;
    mongoc_collection_t *collection;
    bool res = mongoc_database_has_collection(db, collectionName, NULL);
    if (res) {
        collection = mongoc_client_get_collection(client, dbName, collectionName);
    }
    else { // if we don't have collection, create it
        collection = mongoc_database_create_collection(db, collectionName, NULL, &error);
        if (!collection) {
            fprintf(stderr, "mongoc_database_create_collection() : %s\n", error.message);
            return NULL;
        }
    }
    return collection;
}

int InitDb(struct ThreadData_s *threadData) {
    mongoc_init(); // init mongo driver c

    bson_error_t error;

    threadData->db.uri = mongoc_uri_new_with_error(uriStr, &error);
    if (!threadData->db.uri) {
        fprintf(stderr, "mongoc_uri_new_with_error() : %s\n", error.message);
        return 0;
    }

    threadData->db.client = mongoc_client_new_from_uri(threadData->db.uri);
    if (!threadData->db.client) {
        fprintf(stderr, "mongoc_client_new_from_uri() failed\n");
        return 0;
    }

    // get handles of database and collections
    threadData->db.db = mongoc_client_get_database(threadData->db.client, dbName);
    threadData->db.collServ = GetCollectionHandle(threadData->db.db, threadData->db.client, collServName);
    if (threadData->db.collServ == NULL) {
        return 0;
    }
    threadData->db.collUser = GetCollectionHandle(threadData->db.db, threadData->db.client, collUserName);
    if (threadData->db.collUser == NULL) {
        return 0;
    }

    return 1;
}

void DestroyDb(struct ThreadData_s *threadData) {
    mongoc_collection_destroy(threadData->db.collUser);
    mongoc_collection_destroy(threadData->db.collServ);
    mongoc_database_destroy(threadData->db.db);
    mongoc_client_destroy(threadData->db.client);
    mongoc_uri_destroy(threadData->db.uri);
    mongoc_cleanup(); // destroy mongo driver c
}