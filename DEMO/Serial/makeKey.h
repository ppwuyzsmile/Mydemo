#ifndef __MAKE_KEY__
#define __MAKE_KEY__

#include "ECC_Lib\tomcrypt.h"
#define PRNG_LENGTH     8
#define U8              unsigned char
#define U16				unsigned int
#define U32             unsigned long

void makeKey (char* prng_str,U8* pubKey,unsigned long *pubKeyLen,U8* priKey,unsigned long *priKeyLen);

#endif