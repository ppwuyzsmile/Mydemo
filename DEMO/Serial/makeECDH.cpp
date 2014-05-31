
#include "makeECDH.h"

int initPrng (int* prng,prng_state *prng_st,const U8* seed,int length)
{
	*prng = register_prng (&rc4_desc);               //����������
	prng_descriptor[*prng].start (prng_st);           //��ʼ�������
	prng_descriptor[*prng].add_entropy (seed,length,prng_st);		//�˴�������������Seed
	//��������������ģ�����
	prng_descriptor[*prng].ready (prng_st);			//��Seedȥ����PRNG

	return 0;
}

int PointToOID (ecc_point* eccPoint,U8* out)
{
	U32 value[23],*wValue;
	U16 length,*wLen;
	U8 buf[128];
	U8 *wOut;
	/*int i;*/

	length = 0x100; 

	value[0] = 0;
	value[1] = 21;					//7+7+7
	wOut = out;		//�ճ��ռ�д����
	*wOut = 0x01;					//ʹ��OID����
	wOut++;							//��������
	wLen =(U16*)wOut;
	wOut += sizeof(U16);			//����������

	wValue = &(value[2]);
	memcpy (wValue,(eccPoint->x.dp),7*sizeof(U32));
	wValue += 7;
	memcpy (wValue,(eccPoint->y.dp),7*sizeof(U32));
	wValue += 7;
	memcpy (wValue,(eccPoint->z.dp),7*sizeof(U32));			//ȫ��װ��value[]

	der_encode_object_identifier (value,23,buf,(unsigned long*)&length);

	memcpy (wOut,buf,length);

	*wLen = length;					//����

	return length;
}


//-----------------------------------ע:�˺�������ECDH�����Ĺ���������һ��ȷ���Ͳ�����ģ�������PCִ��
int makeECDH (U8* seed,U8* output)
{
	int prng,i;
	ecc_key tmpKey;				//�蹫Կ���ɺ�������һ��ECC��

	prng_state prng_st;

	initPrng (&prng,&prng_st,seed,strlen ((const char*)seed));

	i = ecc_make_key (&prng_st,prng,24,&tmpKey);

	mp_grow (&(tmpKey.pubkey.z),7);

	PointToOID (&(tmpKey.pubkey),output);

	ecc_free (&tmpKey);
	return i;
}