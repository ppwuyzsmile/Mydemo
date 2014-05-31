#pragma	once
#ifndef _DATA_TYPE_H_
#define _DATA_TYPE_H_
#include "PcCommand.h"

typedef struct _LIST_FILE_ATTR{
	struct _LIST_FILE_ATTR * pNext;
	unsigned int nIndex;
	FILE_ATTRIBUTE_R fileAttr;
	bool TokenListState;
	bool TokenRdSearchState;
	bool TokenUpdateState;
	bool TokenValidState;
}LIST_FILE_ATTR,*PLIST_FILE_ATTR;

typedef struct _TAG_ATTR{
	struct _TAG_ATTR *pNext;
	unsigned char TID[8];
	unsigned char tagSpeakerState;
	unsigned char tagSafeSessionState;
	unsigned char nIndex;
}TAG_ATTR,*PTAG_ATTR;

typedef struct _CURRENT_READ_FILE_STATE{
	FILE_ATTRIBUTE_R * pFileAttr;
	unsigned short FileOffset;
	unsigned short FileLen;
	unsigned char readlen;
	bool readFinish;
	unsigned int readCount;
}CURRENT_READ_FILE_STATE;

typedef struct _GOODS_INFO{
	struct _GOODS_INFO * pNext;
	unsigned char nIndex;
}GOODS_INFO,*PGOODS_INFO;

typedef struct _CURRENT_VERIFY_PWD_STATE{
	PLIST_FILE_ATTR  pListFileAttr;
	unsigned int nIndex;
}CURRENT_VERIFY_PWD_STATE;

typedef struct _TIME_OF_READ_LARGE_FILE{
	LARGE_INTEGER   litmp;     
	LONGLONG   QPart1,QPart2;     
	double   dfMinus,dfFreq,dfTim;
}TIME_OF_READ_LARGE_FILE;
#endif