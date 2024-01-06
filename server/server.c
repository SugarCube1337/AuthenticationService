#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <sys/epoll.h>
#include "../threadsdata.h"
#include "server.h"
#include "jwt.h"
#include "../utils/utils.h"
#include "../database/database.h"

// --------- server work logic in handlers

const char *jwtKey = "YW50aS1wYXR0ZXJu"; // anti-pattern


void AskToken(const struct RequestParam_s *request, struct ResponseParam_s *response, const struct MongoData_s *db) {

    char *username = LookUpParamValue(request->queryParams, "name");
    if (username) {
        printf("%s ask token\n", username);

        // 1. Search in DB for the user
        bson_t *query = bson_new();
        BSON_APPEND_UTF8(query, "username", username);

        mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(
                db->collUser, query, NULL, NULL
        );

        bson_t *dbRes;

        if (mongoc_cursor_next(cursor, &dbRes)) {
            char *str = bson_as_canonical_extended_json(dbRes, NULL);
            printf("%s\n", str);

            // 2. jwt

            struct Jwt_s *jwt = CreateJwt(jwtKey, strlen(jwtKey));
            AddJsonAsPayload(jwt, str);

            response->body = ConstructJwt(jwt);
            if (response->body == NULL) {
                response->status = INTERNAL_SERVER_ERROR;
            } else {
                SetContentType(response, "plain/text");
                response->status = OK;
            }

            ReleaseJwt(jwt);
            bson_free(str);
        } else {
            response->status = BAD_REQUEST;
        }
        mongoc_cursor_destroy(cursor);
        bson_destroy(query);
    } else {
        response->status = BAD_REQUEST;
    }
}


void ValidateToken(const struct RequestParam_s *request, struct ResponseParam_s *response, const struct MongoData_s *db) {
    if (request->body == NULL) {
        response->status = BAD_REQUEST;
        return;
    }

    if (!CheckJwtHeader(request->body)) {
        response->status = FORBIDDEN;
        return;
    }
    if (!CheckJwtSign(request->body, jwtKey, strlen(jwtKey))) {
        response->status = FORBIDDEN;
        return;
    }

    // check jwt payload

    // 1. get only payload
    char *localBody = strdup(request->body);
    strtok(localBody, "."); // skip header
    char *payload = strtok(NULL, ".");
    if (payload == NULL) {
        free(localBody);
        response->status = FORBIDDEN;
        return;
    }

    // 2. base64 decode
    int decPayloadLen = CalcDecodedLen(strlen(payload));
    char *decodedPayload = malloc(decPayloadLen + 1);
    memset(decodedPayload, 0, decPayloadLen + 1);
    Base64Decode(payload, strlen(payload), decodedPayload);

    // 3. check if it is json and this json has needed field

    cJSON *payloadJson = cJSON_Parse(decodedPayload);
    if (payloadJson == NULL) {
        free(localBody);
        free(decodedPayload);

        response->status = FORBIDDEN;
        return;
    }

    cJSON *userName = cJSON_GetObjectItem(payloadJson, "username");
    if (!cJSON_IsString(userName) || userName->valuestring == NULL) {
        cJSON_Delete(payloadJson);
        free(decodedPayload);
        free(localBody);

        response->status = FORBIDDEN;
        return;
    }

    bson_t *query = bson_new();
    BSON_APPEND_UTF8(query, "username", userName->valuestring);

    mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(
            db->collUser, query, NULL, NULL
    );

    const bson_t *dbRes;

    if (!mongoc_cursor_next(cursor, &dbRes)) {
        response->status = FORBIDDEN;
        cJSON_Delete(payloadJson);
        free(decodedPayload);
        free(localBody);
        bson_destroy(query);
        mongoc_cursor_destroy(cursor);
        return;
    }

    char *str = bson_as_canonical_extended_json(dbRes, NULL);
    printf("%s\n", str);
    bson_free(str);

    // 5. compare DB result and token value
    if (strcmp(decodedPayload, str) != 0) {
        cJSON_Delete(payloadJson);
        free(decodedPayload);
        free(localBody);

        response->status = FORBIDDEN;
        return;
    }

    cJSON_Delete(payloadJson);
    free(decodedPayload);
    free(localBody);
    response->status = OK;
    bson_destroy(query);
    mongoc_cursor_destroy(cursor);
}



// --------- internal server functions

void RegisterMethod(struct ServerData_s *sd, enum HandlerTypes_e type, char *path, serv_func_type f) {
    AddToHandlersList(sd->handlres, type, path, f);
}

void RunMethod(struct ServerData_s *sd, struct MongoData_s *db, const char *request, char *response) {
    struct RequestParam_s *rq = ParseParams(request);
    struct ResponseParam_s *rs = CreateEmptyResp();

    serv_func_type f = LookUpHandler(sd->handlres, rq->type, rq->path);

    if (f == NULL) { // no registered method? or t is UNSUPPORTED? or p is unknown?
        rs->status = BAD_REQUEST;
    } else {
        f(rq, rs, db); // else run registered method
    }

    ConstructStrResp(rs, response);
    ReleaseResp(rs);
    ReleaseReq(rq);
}

int CreateSocket(int port) {
    int listfd = 0;
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = htons(INADDR_ANY);

    listfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listfd == -1) {
        printf("socket() ret %d, %s\nstop child thread\n", errno, strerror(errno));
        return -1;
    }

    // set non-blocking
    int flags;
    if ((flags = fcntl(listfd, F_GETFL, 0)) < 0) {
        close(listfd);

        printf("fcntl(F_GETFL) ret %d, %s\n", errno, strerror(errno));
        return -1;
    }
    if (fcntl(listfd, F_SETFL, flags | O_NONBLOCK) < 0) {
        close(listfd);

        printf("fcntl(F_SETFL) ret %d, %s\n", errno, strerror(errno));
        return -1;
    }

    if (bind(listfd, (const struct sockaddr *) &saddr, sizeof(saddr)) == -1) {
        close(listfd);

        printf("bind() ret %d, %s\nstop child thread\n", errno, strerror(errno));
        return -1;
    }

    if (listen(listfd, INSOCK_QUEUE_SIZE) == -1) {
        close(listfd);

        printf("listen() ret %d %s\nstop child thread\n", errno, strerror(errno));
        return -1;
    }

    return listfd;
}

SSL_CTX *CreateSslContext() {
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = TLS_server_method();
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        printf("Unable to create SSL context\n");
        return NULL;
    }
    return ctx;
}

int ConfigureContext(SSL_CTX *ctx) {
    /* set key and cert */
    if (SSL_CTX_use_certificate_file(ctx, "../server_keys/server-cert.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        return 0;
    }
    if (SSL_CTX_use_PrivateKey_file(ctx, "../server_keys/server-key.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        return 0;
    }
    SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION); // no SSLv2, SSL_v3, TLSv1.0, TLSv1.1

    return 1;
}

void *NetInterface(void *arg) {
    struct ThreadData_s *threadData = (struct ThreadData_s *) arg;

    threadData->sd.handlres = CreateHandlersList();
    if (threadData->sd.handlres == NULL) {
        printf("CreateHandlersList() error\n");
        kill(threadData->mainPid, SIGUSR1);
        pthread_exit(threadData);
    }
    RegisterMethod(&threadData->sd, GET, "/token", AskToken);
    RegisterMethod(&threadData->sd, POST, "/validate", ValidateToken);

    int listfd = 0;
    int connfd = 0;
    SSL_CTX *ctx;

    ctx = CreateSslContext();
    if (ctx == NULL) {
        kill(threadData->mainPid, SIGUSR1);
        pthread_exit(threadData);
    }

    if (!ConfigureContext(ctx)) {
        kill(threadData->mainPid, SIGUSR1);
        pthread_exit(threadData);
    }

    listfd = CreateSocket(PORT);
    if (listfd == -1) {
        kill(threadData->mainPid, SIGUSR1);
        pthread_exit(threadData);
    }

    // create epoll event for listening
    int epollfd = epoll_create(1);
    if (epollfd == -1) {
        printf("epoll_create() ret %d, %s\n", errno, strerror(errno));
        exit(1);
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = listfd;

    // add event settings to epoll
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listfd, &ev) == -1) {
        printf("epoll_ctl ret %d, %s\n", errno, strerror(errno));
        exit(1);
    }

    struct epoll_event detectedEvent;
    memset((void *) &detectedEvent, 0, sizeof(struct epoll_event));

    char recBuf[MAX_REQUEST_SIZE];
    char sendBuf[MAX_RESPONSE_SIZE];

    while (!NeedQuit(&threadData->stopper)) {
        int te = 0; // triggered event

        // waiting incoming event
        int nfds = epoll_wait(epollfd, &detectedEvent, INSOCK_QUEUE_SIZE, 500);
        if (nfds == -1) {
            printf("error calling epoll: %d %s\n", errno, strerror(errno));
            pthread_exit(threadData);
        }

        for (te = 0; te < nfds; te++) {
            struct epoll_event *event = &detectedEvent + te;

            if (event->data.fd == listfd) { // new connection
                memset(recBuf, 0, sizeof(recBuf));
                memset(sendBuf, 0, sizeof(sendBuf));

                connfd = accept(listfd, NULL, NULL);
                if (connfd == -1) {
                    continue;
                }

                SSL *sslConnection;
                sslConnection = SSL_new(ctx);
                SSL_set_fd(sslConnection, connfd);

                if (SSL_accept(sslConnection) <= 0) {
                    ERR_print_errors_fp(stderr);
                } else {

                    //ssize_t n = read(connfd, recBuf, sizeof(recBuf));
                    ssize_t n = SSL_read(sslConnection, recBuf, sizeof(recBuf));
                    if (n == -1) {
                        SSL_shutdown(sslConnection);
                        SSL_free(sslConnection);
                        close(connfd);
                        ERR_print_errors_fp(stderr);
                        continue;
                    }

                    snprintf(sendBuf, sizeof(sendBuf), "server got msg: %s\n", recBuf);
                    printf("%s", sendBuf);

                    RunMethod(&threadData->sd, &threadData->db, recBuf, sendBuf);

                    //n = write(connfd, sendBuf, strlen(sendBuf));
                    n = SSL_write(sslConnection, sendBuf, strlen(sendBuf));
                    if (n == -1) {
                        SSL_shutdown(sslConnection);
                        SSL_free(sslConnection);
                        close(connfd);
                        ERR_print_errors_fp(stderr);

                        kill(threadData->mainPid, SIGUSR1);
                        break;
                    }
                } // if ssl_accept

                SSL_shutdown(sslConnection);
                SSL_free(sslConnection);
                close(connfd);

            } // if our connection
        } // for te
    } // while !NeedQuit()

    printf("stop child thread\n");

    close(listfd);
    SSL_CTX_free(ctx);
    FreeHandlersList(threadData->sd.handlres);
    pthread_exit(threadData);
}
