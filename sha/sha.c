#include "sha.h"

static inline uint32_t pack8to32_be(const void *src) {
    return ((uint32_t)(((const uint8_t *)src)[0]) << 24)
           | ((uint32_t)(((const uint8_t *)src)[1]) << 16)
           | ((uint32_t)(((const uint8_t *)src)[2]) << 8)
           | (uint32_t)(((const uint8_t *)src)[3]);
}

static inline void pack32to8_be(void *dst, uint32_t val) {
    ((uint8_t *)dst)[0] = (val >> 24);
    ((uint8_t *)dst)[1] = (val >> 16);
    ((uint8_t *)dst)[2] = (val >> 8);
    ((uint8_t *)dst)[3] = val;
}

void Sha256Init(struct Sha256_s *md) {
    md->state[0] = 0x6a09e667;
    md->state[1] = 0xbb67ae85;
    md->state[2] = 0x3c6ef372;
    md->state[3] = 0xa54ff53a;
    md->state[4] = 0x510e527f;
    md->state[5] = 0x9b05688c;
    md->state[6] = 0x1f83d9ab;
    md->state[7] = 0x5be0cd19;

    md->msgLen = 0;
    md->addedW = 0;
}

void Sha256Compress(struct Sha256_s *md) {
    uint32_t W[64];
    uint32_t a,b,c,d,e,f,g,h;
    uint32_t t, t0, t1;
    int i;

    /* load W[0..15] */
    for (i = 0; i < 16; i++) {
        W[i] = pack8to32_be(md->w + 4 * i);
    }

    /* compute W[16..63] */
    for (i = 16; i < 64; i++) {
        W[i] = gamma1(W[i-2]) + W[i-7] + gamma0(W[i-15]) + W[i-16];
    }

    /* load a copy of the state */
    a = md->state[0];
    b = md->state[1];
    c = md->state[2];
    d = md->state[3];
    e = md->state[4];
    f = md->state[5];
    g = md->state[6];
    h = md->state[7];

    /* 64 rounds */
    for (i = 0; i < 64; i++) {
        /* compute */
        t0 = h + sigma1(e) + ch(e, f, g) + k256[i] + W[i];
        t1 = sigma0(a) + maj(a,b,c);
        d += t0;
        h = t0 + t1;

        /* swap */
        t = h;
        h = g;
        g = f;
        f = e;
        e = d;
        d = c;
        c = b;
        b = a;
        a = t;
    }

    /* update md state */
    md->state[0] += a;
    md->state[1] += b;
    md->state[2] += c;
    md->state[3] += d;
    md->state[4] += e;
    md->state[5] += f;
    md->state[6] += g;
    md->state[7] += h;
}

void Sha256Process(struct Sha256_s *md, const unsigned char *input, int inputLen) {
    int loadingInput;
    while (inputLen) {
        loadingInput = (64 - md->addedW) < inputLen ? 64 - md->addedW : inputLen;
        inputLen -= loadingInput;

        /* copy x bytes from input to MD buffer */
        for (int i = 0; i < loadingInput; i++) {
            md->w[md->addedW] = *input++;
            md->addedW++;
        }

        /* if we have a full MD block, transform it */
        if (md->addedW == 64) {
            Sha256Compress(md);
            md->addedW = 0;
            md->msgLen += 64;
        }
    }
}

void Sha256Final(struct Sha256_s *md, unsigned char *output) {
    uint32_t l1, l2, i;

    /* compute final length in bits (eq md->msgLen*8) */
    md->msgLen += md->addedW; // add last (non compression yet) block length
    l2 = md->msgLen >> 29;
    l1 = (md->msgLen << 3) & 0xffFFffFF;

    /* add the padding bit */
    md->w[md->addedW] = 0x80;
    md->addedW++;

    /* if current block len > 56 we need to padding zero & compress this block and only then handle last block */
    if (md->addedW > 56) {
        while (md->addedW < 64) {
            md->w[md->addedW] = 0x00;
            md->addedW++;
        }
        Sha256Compress(md);
        md->addedW = 0;
    }

    /* now pad until we are at position 56 */
    while(md->addedW < 56) {
        md->w[md->addedW] = 0x00;
        md->addedW++;
    }

    /* store msg length */
    pack32to8_be(md->w+56, l2);
    pack32to8_be(md->w+60, l1);

    /* last compress */
    Sha256Compress(md);

    /* save result hash */
    for (i = 0; i < 8; i++) {
        pack32to8_be(output + i*4, md->state[i]);
    }
}