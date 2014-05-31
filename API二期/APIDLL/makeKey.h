#ifndef __MAKE_KEY__
#define __MAKE_KEY__

#include "ECC_Lib\tomcrypt.h"
#define PRNG_LENGTH     8
#define U8              unsigned char
#define U16				unsigned int
#define U32             unsigned long

void makeKey (unsigned char* prng_str,unsigned char* pubKey,unsigned long *pubKeyLen,unsigned char* priKey,unsigned long *priKeyLen);

#endif