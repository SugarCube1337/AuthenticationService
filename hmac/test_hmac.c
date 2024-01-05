# include <stdio.h>
#include <string.h>

#include <openssl/hmac.h>
#include "hmac_sha2.h"

int main(int argc, char *argv[]) {
    unsigned char key[] = {0xaa, 0xbb, 0xcc, 0xdd};
    const char *data = "signed public data";

    unsigned char res1[32];
    memset(res1, 0, 32);

    unsigned char res2[32];
    memset(res2, 0, 32);

    unsigned char res3[32];
    memset(res3, 0, 32);

    // 1
    int mdLen;
    HMAC(EVP_sha256(), key, sizeof(key), data, strlen(data),  res1, &mdLen);

    int i;
    for (i = 0; i < mdLen; i++) {
        printf("0x%02x ", res1[i]);
    }
    printf("\n");

    // 2
    HMAC_CTX *ctx = HMAC_CTX_new();
    HMAC_Init_ex(ctx, key, sizeof(key), EVP_sha256(), NULL);
    HMAC_Update(ctx, data, strlen(data));
    HMAC_Final(ctx, res2, &mdLen);

    for (i = 0; i < mdLen; i++) {
        printf("0x%02x ", res2[i]);
    }
    printf("\n");

    // 3
    HmacSha256(key, sizeof(key), data, strlen(data), res3);

    for (i = 0; i < mdLen; i++) {
        printf("0x%02x ", res3[i]);
    }
    printf("\n");

    return 0;
}
