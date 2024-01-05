#ifndef AuthenticationService_HMAC_SHA2_H
#define AuthenticationService_HMAC_SHA2_H

#include <stdint.h>
#include <string.h>
#include "../sha/sha.h"

void HmacSha256(uint8_t *key, int keyLen, uint8_t *data, int dataLen, uint8_t *digest);

#endif
