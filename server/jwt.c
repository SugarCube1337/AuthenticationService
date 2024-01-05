#include "jwt.h"

struct Jwt_s *CreateJwt(const uint8_t *key, int keySize) {
    struct Jwt_s *jwt = malloc(sizeof(struct Jwt_s));

    jwt->algo = HS256;
    jwt->key = malloc(keySize);
    memcpy(jwt->key, key, keySize);
    jwt->keySize = keySize;
    jwt->header = strdup("{\"alg\": \"HS256\", \"typ\": \"JWT\"}"); // we are using only hmac-sha256 here, so we can construct const header without 3rd party json library
    jwt->payload = NULL;

    return jwt;
}

void AddJsonAsPayload(struct Jwt_s *jwt, const char *jsonStr) {
    /*
     1. from RFC-7519:
     "the set of claims that a JWT must contain to be considered valid is context dependent
     and is outside the scope of this specification"
     2. And we get from DB a json
     => we can use this json as valid JWT payload
     */
    jwt->payload = strdup(jsonStr);
    /* yes, for good practice we need to append this json "exp" and "iss" claims, but...
       this is a study project and the main benefit is that i know about auth and jwt and good practices */
}

char *ConstructJwt(struct Jwt_s *jwt) {
    if (jwt->payload == NULL) {
        return NULL;
    }

    char *ret = NULL;

    // Base64(header)
    int encHeaderLen = CalcEncodedLen(strlen(jwt->header));
    char *encodedHeader = malloc(encHeaderLen + 1);
    memset(encodedHeader, 0, encHeaderLen + 1);
    Base64Encode(jwt->header, strlen(jwt->header), encodedHeader);

    // Base64(payload)
    int encPayloadLen = CalcEncodedLen(strlen(jwt->payload));
    char *encodedPayload = malloc(encPayloadLen + 1);
    memset(encodedPayload, 0, encPayloadLen + 1);
    Base64Encode(jwt->payload, strlen(jwt->payload), encodedPayload);

    asprintf(&ret, "%s.%s", encodedHeader, encodedPayload);

    HmacSha256(jwt->key, jwt->keySize, ret, strlen(ret), jwt->sign);

    // Base64(sign)
    int encSignLen = CalcEncodedLen(32);
    char *encodedSign = malloc(encSignLen + 1);
    memset(encodedSign, 0, encSignLen + 1);
    Base64Encode(jwt->sign, 32, encodedSign);

    // final construction
    asprintf(&ret, "%s.%s", ret, encodedSign);

    free(encodedHeader);
    free(encodedPayload);
    free(encodedSign);
    return ret;
}

#define burn(mem,size) do { volatile char *burnm = (volatile char *)(mem); int burnc = size; while (burnc--) *burnm++ = 0; } while (0)

void ReleaseJwt(struct Jwt_s *jwt) {
    free(jwt->header);
    if (jwt->payload != NULL) {
        free(jwt->payload);
    }
    burn(jwt->key, jwt->keySize);
    free(jwt->key);
    free(jwt);
}

int CheckJwtHeader(const char *encodedJwt) {
    char *localJwt = strdup(encodedJwt);
    char *header = strtok(localJwt, ".");
    if (header == NULL) {
        return 0;
    }

    int decHeaderLen = CalcDecodedLen(strlen(header));
    char *decodedHeader = malloc(decHeaderLen + 1);
    memset(decodedHeader, 0, decHeaderLen + 1);
    Base64Decode(header, strlen(header), decodedHeader);

    cJSON *headerJson = cJSON_Parse(decodedHeader);
    if (headerJson == NULL) {
        free(decodedHeader);
        free(localJwt);
        return 0;
    }

    cJSON *alg = cJSON_GetObjectItem(headerJson, "alg");
    if (!cJSON_IsString(alg) || alg->valuestring == NULL) {
        cJSON_Delete(headerJson);
        free(decodedHeader);
        free(localJwt);
        return 0;
    }

    if (strcmp(alg->valuestring, "HS256") != 0) {
        cJSON_Delete(headerJson);
        free(decodedHeader);
        free(localJwt);
        return 0;
    }

    cJSON *typ = cJSON_GetObjectItem(headerJson, "typ");
    if (!cJSON_IsString(typ) || typ->valuestring == NULL) {
        cJSON_Delete(headerJson);
        free(decodedHeader);
        free(localJwt);
        return 0;
    }

    if (strcmp(typ->valuestring, "JWT") != 0) {
        cJSON_Delete(headerJson);
        free(decodedHeader);
        free(localJwt);
        return 0;
    }

    cJSON_Delete(headerJson);
    free(decodedHeader);
    free(localJwt);
    return 1;
}

int CheckJwtSign(const char *encodedJwt, const uint8_t *key, int keySize) {
    char *localJwt = strdup(encodedJwt);

    char *lastDotPos = strrchr(localJwt, '.');
    if (lastDotPos == NULL) {
        return 0;
    }
    *lastDotPos = '\0'; // separate jwt on two strings: data & sign

    char *jwtData = localJwt;
    char *jwtSign = lastDotPos + 1;

    char decSign[32];
    memset(decSign, 0, 32);
    Base64Decode(jwtSign, strlen(jwtSign), decSign);

    unsigned char dataSign[32];
    HmacSha256(key, keySize, jwtData, strlen(jwtData), dataSign);

    if (memcmp(dataSign, decSign, 32) != 0) {
        free(localJwt);
        return 0;
    }

    free(localJwt);
    return 1;
}
