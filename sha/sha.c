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
    md->state[0] = 0x6a09e667;\
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

void Sha256Compress_v1(struct Sha256_s *md) {
    uint32_t v0, v1, v2, v3, v4, v5, v6, v7;
    uint32_t W[16];

    v0 = md->state[0]; v1 = md->state[1];
    v2 = md->state[2]; v3 = md->state[3];
    v4 = md->state[4]; v5 = md->state[5];
    v6 = md->state[6]; v7 = md->state[7];

    for (int i = 0; i < 16; i++) {
        W[i] = pack8to32_be(md->w + 4 * i);
    }

    one_cycle(0, 1, 2, 3, 4, 5, 6, 7, k256[0], w00);
    one_cycle(7, 0, 1, 2, 3, 4, 5, 6, k256[1], w01);
    one_cycle(6, 7, 0, 1, 2, 3, 4, 5, k256[2], w02);
    one_cycle(5, 6, 7, 0, 1, 2, 3, 4, k256[3], w03);
    one_cycle(4, 5, 6, 7, 0, 1, 2, 3, k256[4], w04);
    one_cycle(3, 4, 5, 6, 7, 0, 1, 2, k256[5], w05);
    one_cycle(2, 3, 4, 5, 6, 7, 0, 1, k256[6], w06);
    one_cycle(1, 2, 3, 4, 5, 6, 7, 0, k256[7], w07);
    one_cycle(0, 1, 2, 3, 4, 5, 6, 7, k256[8], w08);
    one_cycle(7, 0, 1, 2, 3, 4, 5, 6, k256[9], w09);
    one_cycle(6, 7, 0, 1, 2, 3, 4, 5, k256[10], w10);
    one_cycle(5, 6, 7, 0, 1, 2, 3, 4, k256[11], w11);
    one_cycle(4, 5, 6, 7, 0, 1, 2, 3, k256[12], w12);
    one_cycle(3, 4, 5, 6, 7, 0, 1, 2, k256[13], w13);
    one_cycle(2, 3, 4, 5, 6, 7, 0, 1, k256[14], w14);
    one_cycle(1, 2, 3, 4, 5, 6, 7, 0, k256[15], w15);

    one_cycle(0, 1, 2, 3, 4, 5, 6, 7, k256[16], hf00);
    one_cycle(7, 0, 1, 2, 3, 4, 5, 6, k256[17], hf01);
    one_cycle(6, 7, 0, 1, 2, 3, 4, 5, k256[18], hf02);
    one_cycle(5, 6, 7, 0, 1, 2, 3, 4, k256[19], hf03);
    one_cycle(4, 5, 6, 7, 0, 1, 2, 3, k256[20], hf04);
    one_cycle(3, 4, 5, 6, 7, 0, 1, 2, k256[21], hf05);
    one_cycle(2, 3, 4, 5, 6, 7, 0, 1, k256[22], hf06);
    one_cycle(1, 2, 3, 4, 5, 6, 7, 0, k256[23], hf07);
    one_cycle(0, 1, 2, 3, 4, 5, 6, 7, k256[24], hf08);
    one_cycle(7, 0, 1, 2, 3, 4, 5, 6, k256[25], hf09);
    one_cycle(6, 7, 0, 1, 2, 3, 4, 5, k256[26], hf10);
    one_cycle(5, 6, 7, 0, 1, 2, 3, 4, k256[27], hf11);
    one_cycle(4, 5, 6, 7, 0, 1, 2, 3, k256[28], hf12);
    one_cycle(3, 4, 5, 6, 7, 0, 1, 2, k256[29], hf13);
    one_cycle(2, 3, 4, 5, 6, 7, 0, 1, k256[30], hf14);
    one_cycle(1, 2, 3, 4, 5, 6, 7, 0, k256[31], hf15);

    one_cycle(0, 1, 2, 3, 4, 5, 6, 7, k256[32], hf00);
    one_cycle(7, 0, 1, 2, 3, 4, 5, 6, k256[33], hf01);
    one_cycle(6, 7, 0, 1, 2, 3, 4, 5, k256[34], hf02);
    one_cycle(5, 6, 7, 0, 1, 2, 3, 4, k256[35], hf03);
    one_cycle(4, 5, 6, 7, 0, 1, 2, 3, k256[36], hf04);
    one_cycle(3, 4, 5, 6, 7, 0, 1, 2, k256[37], hf05);
    one_cycle(2, 3, 4, 5, 6, 7, 0, 1, k256[38], hf06);
    one_cycle(1, 2, 3, 4, 5, 6, 7, 0, k256[39], hf07);
    one_cycle(0, 1, 2, 3, 4, 5, 6, 7, k256[40], hf08);
    one_cycle(7, 0, 1, 2, 3, 4, 5, 6, k256[41], hf09);
    one_cycle(6, 7, 0, 1, 2, 3, 4, 5, k256[42], hf10);
    one_cycle(5, 6, 7, 0, 1, 2, 3, 4, k256[43], hf11);
    one_cycle(4, 5, 6, 7, 0, 1, 2, 3, k256[44], hf12);
    one_cycle(3, 4, 5, 6, 7, 0, 1, 2, k256[45], hf13);
    one_cycle(2, 3, 4, 5, 6, 7, 0, 1, k256[46], hf14);
    one_cycle(1, 2, 3, 4, 5, 6, 7, 0, k256[47], hf15);

    one_cycle(0, 1, 2, 3, 4, 5, 6, 7, k256[48], hf00);
    one_cycle(7, 0, 1, 2, 3, 4, 5, 6, k256[49], hf01);
    one_cycle(6, 7, 0, 1, 2, 3, 4, 5, k256[50], hf02);
    one_cycle(5, 6, 7, 0, 1, 2, 3, 4, k256[51], hf03);
    one_cycle(4, 5, 6, 7, 0, 1, 2, 3, k256[52], hf04);
    one_cycle(3, 4, 5, 6, 7, 0, 1, 2, k256[53], hf05);
    one_cycle(2, 3, 4, 5, 6, 7, 0, 1, k256[54], hf06);
    one_cycle(1, 2, 3, 4, 5, 6, 7, 0, k256[55], hf07);
    one_cycle(0, 1, 2, 3, 4, 5, 6, 7, k256[56], hf08);
    one_cycle(7, 0, 1, 2, 3, 4, 5, 6, k256[57], hf09);
    one_cycle(6, 7, 0, 1, 2, 3, 4, 5, k256[58], hf10);
    one_cycle(5, 6, 7, 0, 1, 2, 3, 4, k256[59], hf11);
    one_cycle(4, 5, 6, 7, 0, 1, 2, 3, k256[60], hf12);
    one_cycle(3, 4, 5, 6, 7, 0, 1, 2, k256[61], hf13);
    one_cycle(2, 3, 4, 5, 6, 7, 0, 1, k256[62], hf14);
    one_cycle(1, 2, 3, 4, 5, 6, 7, 0, k256[63], hf15);

    md->state[0] += v0;
    md->state[1] += v1;
    md->state[2] += v2;
    md->state[3] += v3;
    md->state[4] += v4;
    md->state[5] += v5;
    md->state[6] += v6;
    md->state[7] += v7;
}



void Sha256Process(compress_func_type f, struct Sha256_s *md, const unsigned char *input, int inputLen) {
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
            f(md);
            md->addedW = 0;
            md->msgLen += 64;
        }
    }
}

void Sha256Final(compress_func_type f, struct Sha256_s *md, unsigned char *output) {
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
        f(md);
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
    f(md);

    /* save result hash */
    for (i = 0; i < 8; i++) {
        pack32to8_be(output + i*4, md->state[i]);
    }
}