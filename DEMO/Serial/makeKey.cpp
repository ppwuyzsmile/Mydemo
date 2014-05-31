#include "makeKey.h"

void makeKey (char* prng_str,U8* pubKey,unsigned long *pubKeyLen,U8* priKey,unsigned long *priKeyLen)
{
	int pn;
	prng_state tg;
	ecc_key key;
	/*ecc_key output;*/
	U8 buf[256];                                //暂存
	U32 *length;
	U8 err[128];
	/*U8 *wPt;*/
	int i;

	pn = register_prng(&rc4_desc);

	*pubKeyLen = 70;			//Max is 70
	*priKeyLen = 100;			//Max is 100

	prng_descriptor[pn].start(&tg);
	prng_descriptor[pn].add_entropy((unsigned char*)prng_str,strlen((const char*)prng_str),&tg);

	prng_descriptor[pn].ready (&tg);

	i = ecc_make_key (&tg,pn,24,&key);

	memcpy (err,error_to_string (i),strlen(error_to_string(i)));

	//mp_init_multi (&output.pubkey.x,&output.pubkey.y,&output.pubkey.z,&output.k,NULL);
	key.type = PK_PUBLIC;
	//output.idx = key.idx;
	//mp_copy (&(key.pubkey.x),&(output.pubkey.x));				//b = a
	//mp_copy (&(key.pubkey.y),&(output.pubkey.y));
	//mp_set (&(output.pubkey.z),1);

//	i = ecc_export ((pubKey + 4),pubKeyLen,PK_PUBLIC,&key);			//开头空出4字节存放密钥长度
	i = ecc_export (buf,pubKeyLen,PK_PUBLIC,&key);	
	memcpy (err,error_to_string (i),strlen(error_to_string(i)));
	memcpy (pubKey+4,buf,*pubKeyLen);
	memcpy (err,error_to_string (i),strlen(error_to_string(i)));
	length = (U32*)pubKey;

	*length = *pubKeyLen + sizeof(U32);

	key.type = PK_PRIVATE;
	//output.idx = key.idx;
	//mp_init_multi (&output.pubkey.x,&output.pubkey.y,&output.pubkey.z,&output.k,NULL);

	//mp_copy (&(key.pubkey.x),&(output.pubkey.x));				//b = a
	//mp_copy (&(key.pubkey.y),&(output.pubkey.y));
	//mp_set (&(output.pubkey.z),1);
	//mp_copy (&(key.k),&(output.k));                     //真·密钥

	i = ecc_export ((priKey + 4),priKeyLen,PK_PRIVATE,&key);//开头空出4字节存放密钥长度
	length = (U32*)priKey;

	//wPt = priKey + sizeof(U32);
	*length = *priKeyLen + sizeof(U32);
	//memcpy (wPt,buf,*priKeyLen);
	
}


//For test
/*
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
*/