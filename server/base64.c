#include "base64.h"

void Base64DecStep(unsigned char *dest, const unsigned char *src) {
    unsigned int x = 0;
    int i;
    for (i = 0; i < 4; i++) {
        if (src[i] >= 'A' && src[i] <= 'Z')
            x = (x << 6) + (unsigned int)(src[i] - 'A' + 0);
        else if (src[i] >= 'a' && src[i] <= 'z')
            x = (x << 6) + (unsigned int)(src[i] - 'a' + 26);
        else if (src[i] >= '0' && src[i] <= '9')
            x = (x << 6) + (unsigned int)(src[i] - '0' + 52);
        else if (src[i] == '+')
            x = (x << 6) + 62;
        else if (src[i] == '/')
            x = (x << 6) + 63;
        else if (src[i] == '=')
            x = (x << 6);
    }

    dest[2] = (x & 255);
    x >>= 8;
    dest[1] = (x & 255);
    x >>= 8;
    dest[0] = (x & 255);
}

int CalcDecodedLen(int toDecodeLen) {
    return (toDecodeLen / 4) * 3;
}

void Base64Decode(const char *input, int inputLen, unsigned char *decoded) {
    int out = 0;
    int in = 0;
    for (int j = 0; j < inputLen / 4; j++) {
        Base64DecStep(decoded + out, input + in);
        out += 3;
        in += 4;
    }
}

static unsigned char binToBase64[64] = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
        'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
        'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
};

void Base64EncStep(const unsigned char *in, char *out) {
    out[0] = binToBase64[(in[0] & 0xFC) >> 2];
    out[1] = binToBase64[((in[0] & 0x03) << 4) | (in[1] >> 4)];
    out[2] = binToBase64[((in[1] & 0x0F) << 2) | (in[2] >> 6)];
    out[3] = binToBase64[in[2] & 0x3F];
}

int CalcEncodedLen(int toEncodeLen) {
    return ((toEncodeLen + 3 ) / 3) * 4;
}

void Base64Encode(const unsigned char *input, int inputLen, char *encoded) {
    int inputConsumed = 0;
    int outputProduced = 0;
    int inputRemaining = inputLen;
    while (inputRemaining >= 3) {
        Base64EncStep(input + inputConsumed, encoded + outputProduced);

        inputConsumed += 3;
        outputProduced += 4;
        inputRemaining -= 3;
    }
    /* ending encoding */
    if (inputRemaining) {
        encoded[outputProduced++] = binToBase64[input[inputConsumed] >> 2];
        if (inputRemaining == 1) {
            encoded[outputProduced++] = binToBase64[(input[inputConsumed] & 0x03) << 4];
            encoded[outputProduced++] = '=';
        } else {
            encoded[outputProduced++] = binToBase64[((input[inputConsumed] & 0x03) << 4) |
                                                    (input[inputConsumed + 1] >> 4)];
            encoded[outputProduced++] = binToBase64[(input[inputConsumed + 1] & 0x0f)<< 2];
        }
        encoded[outputProduced] = '=';
    }
}
