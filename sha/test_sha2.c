#include <stdio.h>
#include <string.h>
#include <time.h>

#include <openssl/sha.h>
#include "sha.h"

void bench1();
void bench2();

int main(int argc, char *argv[]) {
    char *pass = "abc"; // sha256: 0xba 0x78 0x16 0xbf 0x8f 0x01 0xcf 0xea 0x41 0x41 0x40 0xde 0x5d 0xae 0x22 0x23 0xb0 0x03 0x61 0xa3 0x96 0x17 0x7a 0x9c 0xb4 0x10 0xff 0x61 0xf2 0x00 0x15 0xad

    unsigned char resultOpenSsl[32];
    memset(resultOpenSsl, 0, 32);

    unsigned char resultMy[32];
    memset(resultMy, 0, 32);

    unsigned char resultExercize[32];
    memset(resultExercize, 0, 32);

    // OpenSSL
    SHA256_CTX sha2;
    SHA256_Init(&sha2);
    SHA256_Update(&sha2, pass, strlen(pass));
    SHA256_Final(resultOpenSsl, &sha2);

    // my realization
    struct Sha256_s md;
    Sha256Init(&md);
    Sha256Process(&md, pass, strlen(pass));
    Sha256Final(&md, resultMy);

    int i;
    for (i = 0; i < sha2.md_len; i++) {
        printf("0x%02x ", resultOpenSsl[i]);
    }
    printf("\n");
    for (i = 0; i < sha2.md_len; i++) {
        printf("0x%02x ", resultMy[i]);
    }
    printf("\n");

    bench1();
    bench2();
    return 0;
}

void bench1() {
    printf("OpenSSL speed test\n");
    const int N = 1000000;
    int i;
    char *pass = "abc";

    unsigned char result[32];
    memset(result, 0, 32);

    SHA256_CTX sha2;
    SHA256_Init(&sha2);
    SHA256_Update(&sha2, pass, strlen(pass));
    SHA256_Final(result, &sha2);

    clock_t start_time = clock();
    for (i = 0; i < N; i++) {
        SHA256_Update(&sha2, result, sizeof(result));
    }
    SHA256_Final(result, &sha2);
    double testResult = (double) N * 32 / (clock() - start_time) * CLOCKS_PER_SEC / 1048576;
    printf("\tSpeed: %.1f MiB/s\n", testResult);
}

void bench2() {
    printf("My speed test\n");
    const int N = 1000000;
    int i;
    char *pass = "abc";

    unsigned char result[32];
    memset(result, 0, 32);

    struct Sha256_s md;
    Sha256Init(&md);
    Sha256Process(&md, pass, strlen(pass));
    Sha256Final(&md, result);

    clock_t start_time = clock();
    for (i = 0; i < N; i++) {
        Sha256Process(&md, result, sizeof(result));
    }
    Sha256Final(&md, result);
    double testResult = (double) N * 32 / (clock() - start_time) * CLOCKS_PER_SEC / 1048576;
    printf("\tSpeed: %.1f MiB/s\n", testResult);
}
