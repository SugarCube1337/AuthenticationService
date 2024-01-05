#include "hmac_sha2.h"

void HmacSha256(uint8_t *key, int keyLen, uint8_t *data, int dataLen, uint8_t *digest) {
    uint8_t ipad[65];
    uint8_t opad[65];
    uint8_t k[32];
    int i;
    struct Sha256_s md;

    /* if key is longer than 64 bytes reset it to key := HASH(key) */
    if (keyLen > 64) {
        Sha256Init(&md);
        Sha256Process(&md, key, keyLen);
        Sha256Final(&md, k);

        key = k;
        keyLen = 32;
    }

    /* the HMAC transform looks like
     * HASH(K XOR opad || HASH(K XOR ipad || text))
     *
     * where K is an n byte key with zero padding to 64 bytes
     * ipad is the byte 0x36 repeated 64 times
     * opad is the byte 0x5c repeated 64 times
     * text is the data being protected
     * */

    bzero(ipad, sizeof(ipad));
    bzero(opad, sizeof(opad));
    bcopy(key, ipad, keyLen);
    bcopy(key, opad, keyLen);

    /* XOR with opad and ipad */
    for (i = 0; i < 64; i++) {
        ipad[i] ^= 0x36;
        opad[i] ^= 0x5c;
    }

    /* inner hash */
    Sha256Init(&md);
    Sha256Process(&md, ipad, 64);
    Sha256Process(&md, data, dataLen);
    Sha256Final(&md, digest);

    /* outer hash */
    Sha256Init(&md);
    Sha256Process(&md, opad, 64);
    Sha256Process(&md, digest, 32);
    Sha256Final(&md, digest);
}
