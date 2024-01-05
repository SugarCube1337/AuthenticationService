#ifndef AuthenticationService_BASE64_H
#define AuthenticationService_BASE64_H

int CalcEncodedLen(int toEncodeLen);
void Base64Encode(const unsigned char *input, int inputLen, char *encoded);

int CalcDecodedLen(int toDecodeLen);
void Base64Decode(const char *input, int inputLen, unsigned char *decoded);

#endif
