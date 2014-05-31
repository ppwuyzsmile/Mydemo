#include "stdio.h"
#include "string.h"
#include "PcCommand.h"
#include "stdlib.h"
#define  send_cmd(x)  //�����Ϊ�Լ��ĺ���
#define  receive()	  //�����Լ��Ľ��պ���

#pragma pack(1)
typedef struct{
	char name[8];
	unsigned int age;
	unsigned char score;
}STU_INFO,*PSTU_INFO;
#pragma pack()
void	main()
{
	/***************************************
	 *��ʾ����͵�һ������:
	 *1:�����̴�
	 *2:ѡ��һ����ǩ
	 *3:�б��ļ�(��ʡ��,ֱ�Ӷ��Լ�֪�����ļ�)
	 *4:��һ���ļ�,���ǩ�����ļ�0XA103
	 *****************************************/
	//first,collection
	PC_CMD_DATA pcCmdData;
	STU_INFO userInfo;
	char SELECT_TID[9]="12345678";

	printf("length of struct PC_CMD_DATA:%d\n",CMD_LEN);
	memset(&pcCmdData,0,sizeof(PC_CMD_DATA));

	pcCmdData.sigStart = SIG_START_NUM;
	pcCmdData.sigEnd = SIG_END_NUM;
	pcCmdData.cmdType = PC_CMD_TYPE_COLLECT;
	send_cmd(&pcCmdData);

	receive();/*�����̴�ı�ǩ����*/
	receive();//���վ����TID����,����ǩ״̬��


	//ѡ��һ����ǩ
	pcCmdData.cmdType = PC_CMD_TYPE_SELECT_TAG;
	memcpy(pcCmdData.SELECT_TAG_PRAM.TID,SELECT_TID,TID_LEN);
	send_cmd(&pcCmdData);

	//�б��ļ�,��ǰֻ֧�ֹ����е�����ģʽ,��һ��һ���ķ���.
	pcCmdData.cmdType = PC_CMD_TYPE_LIST_FILE;
	pcCmdData.uParam.LIST_FILE_PARAM.dfRecPointer = 2;
	pcCmdData.uParam.LIST_FILE_PARAM.lsitMode =CMD_LIST_MODE_NORMAL;
	pcCmdData.uParam.LIST_FILE_PARAM.opMode = DERECTORY_LIST_MODE_CURRENT;
	send_cmd(&pcCmdData);
	receive();//�����ļ�ID����������Ϣ.

	//ѡ���ļ�
	pcCmdData.cmdType = PC_CMD_TYPE_SELECT_FILE;
	pcCmdData.uParam.SELECT_FILE_PARAM.fileID = FILE_DESCRIPTOR_UII;
	memcpy(pcCmdData.uParam.SELECT_TAG_PRAM.TID,SELECT_TID,TID_LEN);
	send_cmd(&pcCmdData);

	//���ļ�
	pcCmdData.cmdType = PC_CMD_TYPE_READ_FILE;
	pcCmdData.uParam.READ_FILE_PARAM.fileType = FILE_TYPE_TRANS;
	pcCmdData.uParam.READ_FILE_PARAM.uReadPram.transBinFile.dataLen=7;
	pcCmdData.uParam.READ_FILE_PARAM.uReadPram.transBinFile.fileOffset = 0;
	send_cmd(&pcCmdData);

	//ѡ���ļ�,�����ļ�ID:0xABCD
	pcCmdData.cmdType = PC_CMD_TYPE_SELECT_FILE;
	pcCmdData.uParam.SELECT_FILE_PARAM.fileID = 0xABCD;
	memcpy(pcCmdData.uParam.SELECT_TAG_PRAM.TID,SELECT_TID,TID_LEN);
	send_cmd(&pcCmdData);

	//д�ļ�
	pcCmdData.cmdType = PC_CMD_TYPE_WRITE_FILE;
	pcCmdData.uParam.WRITE_FILE_PARAM.fileType = FILE_TYPE_RECORD;
	pcCmdData.uParam.WRITE_FILE_PARAM.uWriteParam.recEF.recPointer = 8;
	pcCmdData.uParam.WRITE_FILE_PARAM.uWriteParam.recEF.opMode = RECORD_IN_FILE_READ_MODE_FIX;//FILE_SELECTED_REC
	pcCmdData.uParam.WRITE_FILE_PARAM.uWriteParam.recEF.dataLen = sizeof(STU_INFO);

	memset(&userInfo,0,sizeof(STU_INFO));
	userInfo.age = 24;
	userInfo.score = 99;
	strcpy(userInfo.name,"�й�");
	memcpy(pcCmdData.uParam.WRITE_FILE_PARAM.uWriteParam.recEF.dataBuffer,&userInfo,sizeof(STU_INFO));

	send_cmd(&pcCmdData);
	receive();//���շ�����Ϣ
	
	
	system("pause");
}