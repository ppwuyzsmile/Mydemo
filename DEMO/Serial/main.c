#include "stdio.h"
#include "string.h"
#include "PcCommand.h"
#include "stdlib.h"
#define  send_cmd(x)  //请填充为自己的函数
#define  receive()	  //定义自己的接收函数

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
	 *演示命令发送的一个过程:
	 *1:首先盘存
	 *2:选择一个标签
	 *3:列表文件(可省略,直接读自己知道的文件)
	 *4:读一个文件,如标签配置文件0XA103
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

	receive();/*接收盘存的标签个数*/
	receive();//接收具体的TID内容,及标签状态字


	//选择一个标签
	pcCmdData.cmdType = PC_CMD_TYPE_SELECT_TAG;
	memcpy(pcCmdData.SELECT_TAG_PRAM.TID,SELECT_TID,TID_LEN);
	send_cmd(&pcCmdData);

	//列表文件,当前只支持国标中的正常模式,即一条一条的返回.
	pcCmdData.cmdType = PC_CMD_TYPE_LIST_FILE;
	pcCmdData.uParam.LIST_FILE_PARAM.dfRecPointer = 2;
	pcCmdData.uParam.LIST_FILE_PARAM.lsitMode =CMD_LIST_MODE_NORMAL;
	pcCmdData.uParam.LIST_FILE_PARAM.opMode = DERECTORY_LIST_MODE_CURRENT;
	send_cmd(&pcCmdData);
	receive();//接收文件ID及其属性信息.

	//选择文件
	pcCmdData.cmdType = PC_CMD_TYPE_SELECT_FILE;
	pcCmdData.uParam.SELECT_FILE_PARAM.fileID = FILE_DESCRIPTOR_UII;
	memcpy(pcCmdData.uParam.SELECT_TAG_PRAM.TID,SELECT_TID,TID_LEN);
	send_cmd(&pcCmdData);

	//读文件
	pcCmdData.cmdType = PC_CMD_TYPE_READ_FILE;
	pcCmdData.uParam.READ_FILE_PARAM.fileType = FILE_TYPE_TRANS;
	pcCmdData.uParam.READ_FILE_PARAM.uReadPram.transBinFile.dataLen=7;
	pcCmdData.uParam.READ_FILE_PARAM.uReadPram.transBinFile.fileOffset = 0;
	send_cmd(&pcCmdData);

	//选择文件,测试文件ID:0xABCD
	pcCmdData.cmdType = PC_CMD_TYPE_SELECT_FILE;
	pcCmdData.uParam.SELECT_FILE_PARAM.fileID = 0xABCD;
	memcpy(pcCmdData.uParam.SELECT_TAG_PRAM.TID,SELECT_TID,TID_LEN);
	send_cmd(&pcCmdData);

	//写文件
	pcCmdData.cmdType = PC_CMD_TYPE_WRITE_FILE;
	pcCmdData.uParam.WRITE_FILE_PARAM.fileType = FILE_TYPE_RECORD;
	pcCmdData.uParam.WRITE_FILE_PARAM.uWriteParam.recEF.recPointer = 8;
	pcCmdData.uParam.WRITE_FILE_PARAM.uWriteParam.recEF.opMode = RECORD_IN_FILE_READ_MODE_FIX;//FILE_SELECTED_REC
	pcCmdData.uParam.WRITE_FILE_PARAM.uWriteParam.recEF.dataLen = sizeof(STU_INFO);

	memset(&userInfo,0,sizeof(STU_INFO));
	userInfo.age = 24;
	userInfo.score = 99;
	strcpy(userInfo.name,"中国");
	memcpy(pcCmdData.uParam.WRITE_FILE_PARAM.uWriteParam.recEF.dataBuffer,&userInfo,sizeof(STU_INFO));

	send_cmd(&pcCmdData);
	receive();//接收反馈信息
	
	
	system("pause");
}