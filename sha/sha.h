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

typedef void (*compress_func_type)(struct Sha256_s *);

void Sha256Compress_v1(struct Sha256_s *md);

void Sha256Init(struct Sha256_s *md);
void Sha256Process(compress_func_type f, struct Sha256_s *md, const unsigned char *input, int inputLen);
void Sha256Final(compress_func_type f, struct Sha256_s *md, unsigned char *output);

/* ------------------------------------------- */
/*             SHA-256 MACROS                  */

#define rotr32(x,n)   (((x) >> (n)) | ((x) << (32 - (n))))

#define ch(x,y,z)       ((z) ^ ((x) & ((y) ^ (z))))
#define maj(x,y,z)      (((x) & (y)) | ((z) & ((x) ^ (y))))

#define sigma0(x)  (rotr32((x),  2) ^ rotr32((x), 13) ^ rotr32((x), 22))
#define sigma1(x)  (rotr32((x),  6) ^ rotr32((x), 11) ^ rotr32((x), 25))
#define gamma0(x)  (rotr32((x),  7) ^ rotr32((x), 18) ^ ((x) >>  3))
#define gamma1(x)  (rotr32((x), 17) ^ rotr32((x), 19) ^ ((x) >> 10))

#define hf00 (w00 += gamma1(w14) + w09 + gamma0(w01))
#define hf01 (w01 += gamma1(w15) + w10 + gamma0(w02))
#define hf02 (w02 += gamma1(w00) + w11 + gamma0(w03))
#define hf03 (w03 += gamma1(w01) + w12 + gamma0(w04))
#define hf04 (w04 += gamma1(w02) + w13 + gamma0(w05))
#define hf05 (w05 += gamma1(w03) + w14 + gamma0(w06))
#define hf06 (w06 += gamma1(w04) + w15 + gamma0(w07))
#define hf07 (w07 += gamma1(w05) + w00 + gamma0(w08))
#define hf08 (w08 += gamma1(w06) + w01 + gamma0(w09))
#define hf09 (w09 += gamma1(w07) + w02 + gamma0(w10))
#define hf10 (w10 += gamma1(w08) + w03 + gamma0(w11))
#define hf11 (w11 += gamma1(w09) + w04 + gamma0(w12))
#define hf12 (w12 += gamma1(w10) + w05 + gamma0(w13))
#define hf13 (w13 += gamma1(w11) + w06 + gamma0(w14))
#define hf14 (w14 += gamma1(w12) + w07 + gamma0(w15))
#define hf15 (w15 += gamma1(w13) + w08 + gamma0(w00))

#define q(n)  v##n

#define one_cycle(a,b,c,d,e,f,g,h,k,w)  \
	q(h) += sigma1(q(e)) + ch(q(e), q(f), q(g)) + (k) + (w); \
	q(d) += q(h); q(h) += sigma0(q(a)) + maj(q(a), q(b), q(c))

/* ------------------------------------------- */
/* inside Sha256Compress() using W[16] array.
 * in SHA-256 macros we are using wN vars.
 * these macros below us to work with W[N] as wN.
 */

#define w00 W[0]
#define w01 W[1]
#define w02 W[2]
#define w03 W[3]
#define w04 W[4]
#define w05 W[5]
#define w06 W[6]
#define w07 W[7]
#define w08 W[8]
#define w09 W[9]
#define w10 W[10]
#define w11 W[11]
#define w12 W[12]
#define w13 W[13]
#define w14 W[14]
#define w15 W[15]

#endif //TEST_SHA_H
