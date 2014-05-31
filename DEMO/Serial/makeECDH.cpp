
#include "makeECDH.h"

int initPrng (int* prng,prng_state *prng_st,const U8* seed,int length)
{
	*prng = register_prng (&rc4_desc);               //随机数池序号
	prng_descriptor[*prng].start (prng_st);           //初始化随机数
	prng_descriptor[*prng].add_entropy (seed,length,prng_st);		//此处负责给随机数池Seed
	//与板上随机数生成模块关联
	prng_descriptor[*prng].ready (prng_st);			//用Seed去生成PRNG

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
	wOut = out;		//空出空间写长度
	*wOut = 0x01;					//使用OID编码
	wOut++;							//跳过编码
	wLen =(U16*)wOut;
	wOut += sizeof(U16);			//跳过长度区

	wValue = &(value[2]);
	memcpy (wValue,(eccPoint->x.dp),7*sizeof(U32));
	wValue += 7;
	memcpy (wValue,(eccPoint->y.dp),7*sizeof(U32));
	wValue += 7;
	memcpy (wValue,(eccPoint->z.dp),7*sizeof(U32));			//全部装入value[]

	der_encode_object_identifier (value,23,buf,(unsigned long*)&length);

	memcpy (wOut,buf,length);

	*wLen = length;					//首字

	return length;
}


//-----------------------------------注:此函数生成ECDH交换的公共参数，一旦确定就不会更改，建议由PC执行
int makeECDH (U8* seed,U8* output)
{
	int prng,i;
	ecc_key tmpKey;				//借公钥生成函数生成一个ECC点

	prng_state prng_st;

	initPrng (&prng,&prng_st,seed,strlen ((const char*)seed));

	i = ecc_make_key (&prng_st,prng,24,&tmpKey);

	mp_grow (&(tmpKey.pubkey.z),7);

	PointToOID (&(tmpKey.pubkey),output);

	ecc_free (&tmpKey);
	return i;
}