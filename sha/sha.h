#ifndef TEST_SHA_H
#define TEST_SHA_H

#include <stdint.h>

struct Sha256_s {
    uint8_t w[64];
    int msgLen;
    int addedW;
    uint32_t state[8];
};

static uint32_t k256[64] = {
        0x428a2f98ul, 0x71374491ul, 0xb5c0fbcful, 0xe9b5dba5ul,
        0x3956c25bul, 0x59f111f1ul, 0x923f82a4ul, 0xab1c5ed5ul,
        0xd807aa98ul, 0x12835b01ul, 0x243185beul, 0x550c7dc3ul,
        0x72be5d74ul, 0x80deb1feul, 0x9bdc06a7ul, 0xc19bf174ul,
        0xe49b69c1ul, 0xefbe4786ul, 0x0fc19dc6ul, 0x240ca1ccul,
        0x2de92c6ful, 0x4a7484aaul, 0x5cb0a9dcul, 0x76f988daul,
        0x983e5152ul, 0xa831c66dul, 0xb00327c8ul, 0xbf597fc7ul,
        0xc6e00bf3ul, 0xd5a79147ul, 0x06ca6351ul, 0x14292967ul,
        0x27b70a85ul, 0x2e1b2138ul, 0x4d2c6dfcul, 0x53380d13ul,
        0x650a7354ul, 0x766a0abbul, 0x81c2c92eul, 0x92722c85ul,
        0xa2bfe8a1ul, 0xa81a664bul, 0xc24b8b70ul, 0xc76c51a3ul,
        0xd192e819ul, 0xd6990624ul, 0xf40e3585ul, 0x106aa070ul,
        0x19a4c116ul, 0x1e376c08ul, 0x2748774cul, 0x34b0bcb5ul,
        0x391c0cb3ul, 0x4ed8aa4aul, 0x5b9cca4ful, 0x682e6ff3ul,
        0x748f82eeul, 0x78a5636ful, 0x84c87814ul, 0x8cc70208ul,
        0x90befffaul, 0xa4506cebul, 0xbef9a3f7ul, 0xc67178f2ul
};

void Sha256Init(struct Sha256_s *md);
void Sha256Process(struct Sha256_s *md, const unsigned char *input, int inputLen);
void Sha256Final(struct Sha256_s *md, unsigned char *output);

/* ------------------------------------------- */
/*             SHA-256 MACROS                  */

#define rotr32(x,n)   (((x) >> (n)) | ((x) << (32 - (n))))

#define ch(x,y,z)       ((z) ^ ((x) & ((y) ^ (z))))
#define maj(x,y,z)      (((x) & (y)) | ((z) & ((x) ^ (y))))

#define sigma0(x)  (rotr32((x),  2) ^ rotr32((x), 13) ^ rotr32((x), 22))
#define sigma1(x)  (rotr32((x),  6) ^ rotr32((x), 11) ^ rotr32((x), 25))
#define gamma0(x)  (rotr32((x),  7) ^ rotr32((x), 18) ^ ((x) >>  3))
#define gamma1(x)  (rotr32((x), 17) ^ rotr32((x), 19) ^ ((x) >> 10))

#endif
