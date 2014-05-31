#ifndef __MAKEECDH__
#define __MAKEECDH__

#include "ECC_Lib\tomcrypt.h"

#define PRNG_LENGTH     8
#define U8              unsigned char
#define U16				unsigned int
#define U32             unsigned long

int PointToOID (ecc_point* eccPoint,U8* out);
int makeECDH (U8* seed,U8* output);

#endif