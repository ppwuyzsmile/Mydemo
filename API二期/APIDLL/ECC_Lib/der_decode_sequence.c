/* LibTomCrypt, modular cryptographic library -- Tom St Denis
 *
 * LibTomCrypt is a library that provides various cryptographic
 * algorithms in a highly modular and flexible manner.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 *
 * Tom St Denis, tomstdenis@gmail.com, http://libtomcrypt.org
 */
#include "tomcrypt.h"
//#include <stdarg.h>


/**
  @file der_decode_sequence.c
  ASN.1 DER, decode a SEQUENCE, Tom St Denis
*/

#ifdef LTC_DER

/**
   Decode a SEQUENCE
   @param in       The DER encoded input
   @param inlen    The size of the input
   @param list     The list of items to decode
   @param outlen   The number of items in the list
   @return CRYPT_OK on success
*/
int der_decode_sequence(const unsigned char *in,   unsigned long  inlen,
                              ltc_asn1_list *list, unsigned long  outlen)
{
   int           err, type;
   unsigned long size, x, y, z, i, blksize;
   void          *data;

   LTC_ARGCHK(in   != NULL);
   LTC_ARGCHK(list != NULL);

   /* get blk size */
   if (inlen < 2) {
      return CRYPT_INVALID_PACKET;
   }

   /* sequence type? */
   x = 0;
   if (in[x++] != 0x30) {
      return CRYPT_INVALID_PACKET;
   }

   if (in[x] < 128) {
      blksize = in[x++];
   } else if (in[x] & 0x80) {
      if (in[x] < 0x81 || in[x] > 0x83) {
         return CRYPT_INVALID_PACKET;
      }
      y = in[x++] & 0x7F;

      /* would reading the len bytes overrun? */
      if (x + y > inlen) {
         return CRYPT_INVALID_PACKET;
      }

      /* read len */
      blksize = 0;
      while (y--) {
          blksize = (blksize << 8) | (unsigned long)in[x++];
      }
  }

  /* would this blksize overflow? */
  if (x + blksize > inlen) {
     return CRYPT_INVALID_PACKET;
  }

  /* ok read data */
   inlen = blksize;
   for (i = 0; i < outlen; i++) {
       type = list[i].type;
       size = list[i].size;
       data = list[i].data;

       if (type == LTC_ASN1_EOL) { 
          break;
       }

       switch (type) {
           case LTC_ASN1_INTEGER:
               z = inlen;
               if ((err = der_decode_integer(in + x, z, data)) != CRYPT_OK) {
                  goto LBL_ERR;
               }
               if ((err = der_length_integer(data, &z)) != CRYPT_OK) {
                  goto LBL_ERR;
               }
               x     += z;
               inlen -= z;
               break;


           case LTC_ASN1_SHORT_INTEGER:
               z = inlen;
               if ((err = der_decode_short_integer(in + x, z, data)) != CRYPT_OK) {
                  goto LBL_ERR;
               }
               if ((err = der_length_short_integer(size, &z)) != CRYPT_OK) {
                  goto LBL_ERR;
               }
               x     += z;
               inlen -= z;
               break;

           case LTC_ASN1_BIT_STRING:
               z = inlen;
               if ((err = der_decode_bit_string(in + x, z, data, &size)) != CRYPT_OK) {
                  goto LBL_ERR;
               }
               list[i].size = size;
               if ((err = der_length_bit_string(size, &z)) != CRYPT_OK) {
                  goto LBL_ERR;
               }
               x     += z;
               inlen -= z;
               break;

           case LTC_ASN1_OCTET_STRING:
               z = inlen;
               if ((err = der_decode_octet_string(in + x, z, data, &size)) != CRYPT_OK) {
                  goto LBL_ERR;
               }
               list[i].size = size;
               if ((err = der_length_octet_string(size, &z)) != CRYPT_OK) {
                  goto LBL_ERR;
               }
               x     += z;
               inlen -= z;
               break;

           case LTC_ASN1_NULL:
               if (inlen < 2 || in[x] != 0x05 || in[x+1] != 0x00) {
                  err = CRYPT_INVALID_PACKET;
                  goto LBL_ERR;
               }
               x     += 2;
               inlen -= 2;
               break;
                  
           case LTC_ASN1_OBJECT_IDENTIFIER:
               z = inlen;
               if ((err = der_decode_object_identifier(in + x, z, data, &size)) != CRYPT_OK) {
                  goto LBL_ERR;
               }
               list[i].size = size;
               if ((err = der_length_object_identifier(data, size, &z)) != CRYPT_OK) {
                  goto LBL_ERR;
               }
               x     += z;
               inlen -= z;
               break;

           case LTC_ASN1_IA5_STRING:
               z = inlen;
               if ((err = der_decode_ia5_string(in + x, z, data, &size)) != CRYPT_OK) {
                  goto LBL_ERR;
               }
               list[i].size = size;
               if ((err = der_length_ia5_string(data, size, &z)) != CRYPT_OK) {
                  goto LBL_ERR;
               }
               x     += z;
               inlen -= z;
               break;


           case LTC_ASN1_PRINTABLE_STRING:
               z = inlen;
               if ((err = der_decode_printable_string(in + x, z, data, &size)) != CRYPT_OK) {
                  goto LBL_ERR;
               }
               list[i].size = size;
               if ((err = der_length_printable_string(data, size, &z)) != CRYPT_OK) {
                  goto LBL_ERR;
               }
               x     += z;
               inlen -= z;
               break;

           case LTC_ASN1_UTCTIME:
               z = inlen;
               if ((err = der_decode_utctime(in + x, &z, data)) != CRYPT_OK) {
                  goto LBL_ERR;
               }
               x     += z;
               inlen -= z;
               break;

           case LTC_ASN1_SEQUENCE:
               z = inlen;
               if ((err = der_decode_sequence(in + x, z, data, size)) != CRYPT_OK) {
                  goto LBL_ERR;
               }
               if ((err = der_length_sequence(data, size, &z)) != CRYPT_OK) {
                  goto LBL_ERR;
               }
               x     += z;
               inlen -= z;
               break;


           case LTC_ASN1_CHOICE:
               z = inlen;
               if ((err = der_decode_choice(in + x, &z, data, size)) != CRYPT_OK) {
                  goto LBL_ERR;
               }
               x     += z;
               inlen -= z;
               break;

           default:
               err = CRYPT_INVALID_ARG;
               goto LBL_ERR;
       }
   }
   err = CRYPT_OK;   

LBL_ERR:
   return err;
}  
 
#endif

/* $Source: /cvs/libtom/libtomcrypt/src/pk/asn1/der/sequence/der_decode_sequence.c,v $ */
/* $Revision: 1.8 $ */
/* $Date: 2005/06/18 19:20:23 $ */
