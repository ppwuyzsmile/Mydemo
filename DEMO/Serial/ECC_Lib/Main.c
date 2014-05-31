#include "tomcrypt.h"
//#include <stdio.h>
//#include <string.h>

int prng,hash;
prng_state prng_st;
hash_state hash_st;

typedef struct TEST
{
	char flag;
	int size;
	char buf[64];
}testStr;

int initPrng ()
{
    prng = register_prng (&rc4_desc);               //����������
    prng_descriptor[prng].start (&prng_st);           //��ȡ�����
	prng_descriptor[prng].add_entropy ("See you next",12,&prng_st);		//�˴�������������Seed																	//��������������ģ�����
    prng_descriptor[prng].ready (&prng_st);			//��Seedȥ����PRNG
}

int initHash ()
{
    int err;
    register_hash (&md5_desc);
    err = hash_descriptor[hash].init(&hash_st);
}

int initECDSA (ecc_key *key,int state)
{

    (*key).type = state;
    (*key).idx = 0;

    mp_init (&((*key).pubkey.x));
    mp_init (&((*key).pubkey.y));
    mp_init (&((*key).pubkey.z));
    mp_init (&((*key).k));
    return 0;
}

//int setKey (ecc_key *key,char* key_X,char* key_Y,char* key_Z,int length)
//{
//    mp_read_radix (&((*key).pubkey.x),key_X,length);
//    mp_read_radix (&((*key).pubkey.y),key_Y,length);
//    mp_set (&((*key).k),1);
//    mp_read_radix (&((*key).k),key_Z,length);
//
//    return 0;
//}                                           //��������KEY��X,Y,Z
//
/*int makeKey (ecc_key *key,int keySize)
{
    ecc_make_key (&prng_st,prng,keySize,key);

    return 0;
}  */                 //����ECC��Կ

int importKey (unsigned char *code,unsigned long codeLength,ecc_key *import)
{
    return ecc_import (code,codeLength,import);
}                   //������byte���е���ECC��Կ

int exportKey (unsigned char *code,unsigned long *codeLength,int type,ecc_key *key)
{
    return ecc_export (code,codeLength,type,key);
}

int longEncrypt (const unsigned char* input,
                 unsigned long inlen,
                 unsigned char* output,
                 unsigned long* outlen,
                 prng_state* prng,
                 int wprng,
                 int hash,
                 ecc_key *key)
{
    char flag;
    unsigned char buf[128];
    int times,err;
    unsigned long lenPerTimes;
    const unsigned char *inputHandle;
    unsigned char *outputHandle;

    inputHandle = input;
    outputHandle = output;

    if ((inlen & 0xf) != 0)
    {
        flag = 1;               //inlen mod 16 != 0,��Ҫ��һ��
    }else
        {
            flag = 0;
        }
    times = (inlen >> 4);         //inlen/16

    for (;times > 0;times--)
    {
       err = ecc_encrypt_key (inputHandle,16,outputHandle,&lenPerTimes,prng,wprng,hash,key);
       (*outlen) += lenPerTimes;
       outputHandle += lenPerTimes;
       inputHandle += 16;
    }                             //ÿ�μ���16�ֽ�����

    if (flag)
    {
        err = ecc_encrypt_key (inputHandle,16,buf,&lenPerTimes,prng,wprng,hash,key); 
        memcpy (outputHandle,buf,lenPerTimes);
		(*outlen) += lenPerTimes;
    }

    return err;
    
}

int longDecrypt (const unsigned char* input,
                 unsigned long inlen,
                 unsigned char* output,
                 unsigned long* outlen,
                 ecc_key *key)
{
    char flag;
    unsigned char buf[16];
    int times,err;
    unsigned long lenPerTimes;
    const unsigned char *inputHandle;
    unsigned char *outputHandle;

    inputHandle = input;
    outputHandle = output;
	err = 0;
    times = (inlen / 0x5e);         //

    for (;times > 0;times--)
    {
       err = ecc_decrypt_key (inputHandle,0x5e,outputHandle,&lenPerTimes,key); 

       (*outlen) += lenPerTimes;
       outputHandle += lenPerTimes;
       inputHandle += 0x5e;
    }                             //ÿ�ν���16�ֽ�����

	return err;
    
}

void printOut (char* title,char* input,int length)
{
	int i;
	printf("%s:",title);
	for (i = 0;i < length;i++)
	{
		 printf(" %x",*(input + i));
	}
	printf("\n");
}

#define U8 unsigned char
#define U32 unsigned long

void makeKey (unsigned char* prng_str,U8* pubKey,unsigned long *pubKeyLen,U8* priKey,unsigned long *priKeyLen)
{
	int pn;
	prng_state tg;
	ecc_key key;
	ecc_key output;
	U8 buf[256];                                //�ݴ�
	U32 *length;
	U8 err[128];
	U8 *wPt;
	int i;

	pn = register_prng(&rc4_desc);

	prng_descriptor[pn].start(&tg);
	prng_descriptor[pn].add_entropy(prng_str,strlen((const char*)prng_str),&tg);

	prng_descriptor[pn].ready (&tg);

	i = ecc_make_key (&tg,pn,24,&key);

	memcpy (err,error_to_string (i),strlen(error_to_string(i)));

	//mp_init_multi (&output.pubkey.x,&output.pubkey.y,&output.pubkey.z,&output.k,NULL);
	key.type = PK_PUBLIC;
	//output.idx = key.idx;
	//mp_copy (&(key.pubkey.x),&(output.pubkey.x));				//b = a
	//mp_copy (&(key.pubkey.y),&(output.pubkey.y));
	//mp_set (&(output.pubkey.z),1);

//	i = ecc_export ((pubKey + 4),pubKeyLen,PK_PUBLIC,&key);			//��ͷ�ճ�4�ֽڴ����Կ����
	i = ecc_export (buf,pubKeyLen,PK_PUBLIC,&key);	
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
	//mp_copy (&(key.k),&(output.k));                     //�桤��Կ

	i = ecc_export ((priKey + 4),priKeyLen,PK_PRIVATE,&key);//��ͷ�ճ�4�ֽڴ����Կ����
	length = (U32*)priKey;

	//wPt = priKey + sizeof(U32);
	*length = *priKeyLen + sizeof(U32);
	//memcpy (wPt,buf,*priKeyLen);

}

void main ()
{
    ecc_key publicKey,priKey;
    ecc_key privateKey,pubKey;

    ecc_key getPbKey,getPrKey;
    unsigned long pubSize,priSize;

	unsigned char pubKeyCode[100] = {0};/*{0x3F,0x00,0x00,0x00,0x30,0x3D,0x03,0x02,0x07,0x00,0x02,0x01,0x18,0x02,0x19,0x00,
	0xA9,0xD9,0x59,0x07,0xB5,0x6C,0x5C,0x54,0x2E,0xD5,0x5F,0xDF,0x57,0xA9,0x0B,0x79,
	0xB4,0x1C,0xAE,0x4B,0xD1,0xD1,0xD9,0x69,0x02,0x19,0x00,0x9A,0xD2,0x4C,0xDF,0x3A,
	0x56,0x13,0x68,0x9B,0x13,0xE4,0x7E,0xDE,0xA6,0x1F,0x2F,0x6D,0x66,0xE5,0x36,0x08,
	0x10,0x2E,0xDF}; */   //192 bit
   
	unsigned char priKeyCode[100] = {0};/*{0x5A,0x00,0x00,0x00,0x30,0x58,0x03,0x02,0x07,0x80,0x02,0x01,0x18,0x02,0x19,0x00,
    0xA9,0xD9,0x59,0x07,0xB5,0x6C,0x5C,0x54,0x2E,0xD5,0x5F,0xDF,0x57,0xA9,0x0B,0x79,
    0xB4,0x1C,0xAE,0x4B,0xD1,0xD1,0xD9,0x69,0x02,0x19,0x00,0x9A,0xD2,0x4C,0xDF,0x3A,
    0x56,0x13,0x68,0x9B,0x13,0xE4,0x7E,0xDE,0xA6,0x1F,0x2F,0x6D,0x66,0xE5,0x36,0x08,
    0x10,0x2E,0xDF,0x02,0x19,0x00,0xB9,0x1F,0xC6,0xD9,0xA7,0x4E,0xAD,0x7D,0xF0,0xCA,
    0xD5,0xF7,0xC1,0x2D,0x8A,0x64,0x36,0x75,0x2F,0x8D,0xFF,0x0E,0x25,0x59};   */ //192 bit

    const char msg[] = "Hello,this beautiful and ugly world!!aldsfiqewnmkcxz,ncvjuewahorqndasfadssdsdaadsasj";                         //�����ܵ��ַ���
    char decryptPlain[100];
    char encryptPlain[400];
    char sign[100];
	char buf[200];
    unsigned long signSize,decryptSize,encryptSize;
    int state,err;
    FILE *fp;
	unsigned char *p;
	testStr stu,stu2;

	stu.flag = 1;
	stu.size = 7 + 6;
	strcpy (stu.buf,"ABCDEFrrrrrr");

	memcpy (buf,&stu,sizeof (testStr));
	memcpy (&stu2,buf,sizeof(testStr));

    initPrng ();                                //��ʼ�������   
    initHash ();                                //��ʼ��Hash            ʹ�õ���MD5
    initECDSA (&publicKey,PK_PUBLIC);
    initECDSA (&privateKey,PK_PRIVATE);          //��ʼ��key
    decryptSize = 0;
    encryptSize = 0;

 //   makeKey ((unsigned char*)"Nothing.",pubKeyCode,&pubSize,priKeyCode,&priSize);           //192bit������Կ
//    makeKey (&privateKey,24);           //192bit������Կ

//    priKey.type=PK_PRIVATE;
//    priKey.idx=0;
//    mp_init_multi(&priKey.pubkey.x, &priKey.pubkey.y, &priKey.pubkey.z, &priKey.k, NULL);
//    mp_read_radix(&priKey.pubkey.x, "pXrgVru+Nnchh+Z79jszFAj7Kn9CBVkN", 64);
//    mp_read_radix(&priKey.pubkey.y, "3F2fpmHKb+3St9hQq9MPUtN2oIx25MGW", 64);
//    mp_set(&priKey.pubkey.z, 1);
//    mp_read_radix(&priKey.k, "mmR+WdM2HVibOmEAVXkWiLECsI5YUbzM", 64);
//
//    pubKey.type=PK_PUBLIC;
//    pubKey.idx=0;
//    mp_init_multi(&pubKey.pubkey.x, &pubKey.pubkey.y, &pubKey.pubkey.z, &pubKey.k, NULL);
//    mp_read_radix(&pubKey.pubkey.x, "pXrgVru+Nnchh+Z79jszFAj7Kn9CBVkN", 64);
//    mp_read_radix(&pubKey.pubkey.y, "3F2fpmHKb+3St9hQq9MPUtN2oIx25MGW", 64);
//    mp_set(&pubKey.pubkey.z, 1);
//
//
//	ecc_export (pubKeyCode,&pubSize,PK_PUBLIC,&pubKey);
//	ecc_export (priKeyCode,&priSize,PK_PRIVATE,&priKey);


     // exportKey (pubKeyCode,&pubSize,PK_PUBLIC,&privateKey);            //������Կ��byte��                 <---��������Ŀ���ǽ���Կ˽Կת��Ϊbyte����ԭ���������ṹ��
     //exportKey (priKeyCode,&priSize,PK_PRIVATE,&privateKey);          //����˽Կ

    fp = fopen ("PubKey","r+");
    fread (&pubSize,4,1,fp);        //read length
    printf ("\nPublic key length:%d\n",pubSize);
    fread (pubKeyCode,1,pubSize,fp);    //read code
    fclose (fp);

    fp = fopen ("PriKey","r+");
    fread (&priSize,4,1,fp);        //read length
    printf ("\nPrivate key length:%d\n",priSize);
    fread (priKeyCode,1,priSize,fp);    //read code
    fclose (fp);

    printOut ("PUBLIC KEY",(char*)pubKeyCode,pubSize);
    printOut ("PRIVATE KEY",(char*)priKeyCode,priSize);

	 pubSize = *pubKeyCode;
	 priSize = *priKeyCode;
	 p = pubKeyCode + 4;
	 err = 0;

      err =   ecc_import ((const unsigned char*)(pubKeyCode /*+ 4*/),pubSize,&getPbKey);
//    err = importKey ((unsigned char*)pubKeyCode,pubSize,&getPbKey);                        //���빫Կ         <--������Ŀ���ǽ�byte����ʽ�Ĺ�Կ��˽Կ��ɼ����㷨���õĽṹ��
      err  = ecc_import ((const unsigned char*)(priKeyCode /*+ 4*/),priSize,&getPrKey);                        //����˽Կ

    printf ("Error:%s",error_to_string (err));

	prng = register_prng (&rc4_desc);               //����������
    prng_descriptor[prng].start (&prng_st);           //��ȡ�����
	prng_descriptor[prng].add_entropy ("I want some PRNG",16,&prng_st);		//�˴�������������Seed																	//��������������ģ�����
    prng_descriptor[prng].ready (&prng_st);			//��Seedȥ����PRNG

    ecc_sign_hash (msg,strlen(msg),sign,&signSize,&prng_st,prng,&getPrKey);      //ǩ��                 <---�˲�Ŀ��������ǩ����Ϣ����˽Կȥ�����ù�Կȥ��֤
 //   ecc_sign_hash (msg,strlen(msg),sign,&signSize,&prng_st,prng,&privateKey);      //ǩ��                 <---�˲�Ŀ��������ǩ����Ϣ����˽Կȥ�����ù�Կȥ��֤
 //  printf ("Size Of verify:%ld\n",signSize);
	
    if (CRYPT_OK == ecc_verify_hash (sign,signSize,msg,strlen(msg),&state,&getPbKey))
    {
        printf ("Verify Ok!");                                 //У��ɹ�
 while (1);
    }else
        {
            printf ("Verify Failed!");                         //У��ʧ��
while (1);
        }

    hash = find_hash ("md5");                                   //����MD5�㷨���ӽ����㷨
    err = longEncrypt (msg,strlen (msg) + 1,encryptPlain,&encryptSize,&prng_st,prng,hash,&getPbKey);        //���ַ����ļ���
    //                 �������ַ���|�����ܳ���|�����������|����������ȣ����ֵ��|�����״̬|��������|��ϣ���|ת������˽Կ�ṹ��
    //err = ecc_encrypt_key (msg,strlen (msg) + 1,encryptPlain,&encryptSize,&prng_st,prng,hash,&getPbKey);
 //   printf ("Error:%s\n",error_to_string (err));
 //   printf ("Length encryptSize:%ld\n",encryptSize);
    err = longDecrypt (encryptPlain,encryptSize,decryptPlain,&decryptSize,&getPrKey);                       //���ַ����Ľ���
    //������ֶκ�����ͬ
//    printf ("Output length decryptSize:%ld\n",decryptSize);
//    printf ("Decrypt: %s",decryptPlain);
}