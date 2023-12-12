#ifndef DATABASE_H
#define DATABASE_H

#include "mongodata.h"
#include "../threadsdata.h"

extern const char *uriStr;
extern const char *dbName;
extern const char *collServName;
extern const char *collUserName;

mongoc_collection_t *GetCollectionHandle(mongoc_database_t *db, mongoc_client_t *client, const char *collectionName);
int InitDb(struct ThreadData_s *threadData);
void DestroyDb(struct ThreadData_s *threadData);

#endif // DATABASE_H
