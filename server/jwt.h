#ifndef AuthenticationService_JWT_H
#define AuthenticationService_JWT_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

#include "base64.h"
#include "../hmac/hmac_sha2.h"

enum JwtAldos_e {
    HS256
    // other types
};

struct Jwt_s {
    enum JwtAldos_e algo;
    uint8_t *key;
    int keySize;
    char *header;
    char *payload;
    uint8_t sign[32];
};

struct Jwt_s *CreateJwt(const uint8_t *key, int keySize);
void AddJsonAsPayload(struct Jwt_s *jwt, const char *jsonStr);
char *ConstructJwt(struct Jwt_s *jwt);
void ReleaseJwt(struct Jwt_s *jwt);

int CheckJwtHeader(const char *encodedJwt);
int CheckJwtSign(const char *encodedJwt, const uint8_t *key, int keySize);

#endif
