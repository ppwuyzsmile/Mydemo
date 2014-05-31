#include "makeKey.h"



void makeKey (char* prng_str,U8* pubKey,unsigned long *pubKeyLen,U8* priKey,unsigned long *priKeyLen)
{
    int pn;
    prng_state tg;
    ecc_key key;
    ecc_key output;
    U8 buf[256];                                //‘›¥Ê
    U32 *length;
    U8 *wPt;

    pn = register_prng(&rc4_desc);
    
    prng_descriptor[pn].start(&tg);
    prng_descriptor[pn].add_entropy((const U8*)prng_str,PRNG_LENGTH,&tg);

    prng_descriptor[pn].ready (&tg);

    ecc_make_key (&tg,pn,24,&key);

    output.type = PK_PUBLIC;
    output.idx = key.idx;
    mp_copy (&(output.pubkey.x),&(key.pubkey.x));
    mp_copy (&(output.pubkey.y),&(key.pubkey.y));
    mp_set (&(output.pubkey.z),1);
    ecc_export (buf,pubKeyLen,PK_PUBLIC,&output);

    length = (U32*)pubKey;

    wPt = priKey + sizeof(U32);
    *length = *pubKeyLen + sizeof(U32);
    memcpy (wPt,buf,*pubKeyLen);

    output.type = PK_PRIVATE;
    output.idx = key.idx;
    mp_init_multi (&output.pubkey.x,&output.pubkey.y,&output.pubkey.z,&output.k,NULL);
    
    mp_copy (&(output.pubkey.x),&(key.pubkey.x));
    mp_copy (&(output.pubkey.y),&(key.pubkey.y));
    mp_set (&(output.pubkey.z),1);
    mp_copy (&(output.k),&(key.k));                     //’Ê°§√‹‘ø

    ecc_export (buf,priKeyLen,PK_PRIVATE,&output);
    length = (U32*)priKey;

    wPt = priKey + sizeof(U32);
    *length = *priKeyLen + sizeof(U32);
    memcpy (wPt,buf,*priKeyLen);

}

//For test
int main ()
{
    FILE *fp;
    U8 pubKey[128];
    U8 priKey[128];
    U32 pubLen,priLen;

    makeKey ("ABNUYTRE",pubKey,&pubLen,priKey,&priLen);

    fp = fopen ("PUBLIC","w+");
    fwrite (pubKey,sizeof(U8),pubLen + sizeof(U32),fp);
    fclose (fp);

    fp = fopen ("PRIVATE","w+");
    fwrite (priKey,sizeof(U8),priLen + sizeof(U32),fp);
    fclose (fp);

    return 0;
}