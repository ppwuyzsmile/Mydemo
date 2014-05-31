#ifndef APIDLL_H
#define APIDLL_H

#include "PcCommand.h"


#define IN
#define OUT
#define INOUT
#define GLOBAL
#define PRINT		printf
#define CHAR_NUM	64 
#define TCP_DATA	sizeof(PC_CMD_REPLY)
#define SEND_TCP_DATA  1024
#define LARGE_FILE_SIZE	 2048
#define TIME_OUT  1000
#define RFIDLIBDLL extern "C" _declspec(dllexport)
typedef  unsigned int RF_STATUS;

//��ѯUSB�豸
RFIDLIBDLL RF_STATUS SearchHidDevice();

//�ر�USB�豸
RFIDLIBDLL void CloseHidDevice();

// ͨ��USB��������
// RFIDLIBDLL void SendDataToUsb( IN char *VID,		
// 							   IN char *PID, 
// 							   IN char *SendData
// 							   );
// 
// ͨ��USB��������
// RFIDLIBDLL void ReceiveDataFromUsb(	IN  char *VID,
// 									IN  char *PID,
// 									IN  int numTimeOut,//��ʱʱ�䣬��λ����
// 									OUT char *ReceiveData
// 								    );
// 
// RFIDLIBDLL void UsbReceiveThread();
// 
//�����׽�������,����ֵΪ�׽���sockRet
RFIDLIBDLL RF_STATUS StartConnect(IN int  m_Port,
								  IN char *adressIP);

//�ر��׽���
RFIDLIBDLL void CloseConnect();



// ���ڷ�������
// RFIDLIBDLL void SendDataToSocket( IN SOCKET sockSrv, 
// 								  IN char *SendData
// 								  );

// ���ڽ�������
// RFIDLIBDLL void RceiveDataFromSocket( IN SOCKET sockSrv);

// RFIDLIBDLL void SocketReceiveThread();

//���Է���api�ӿ�
RFIDLIBDLL RF_STATUS TestSendCmd(PC_CMD_DATA* pData);

//���Խ���api�ӿ�
RFIDLIBDLL RF_STATUS TestRecvCmd(PC_CMD_REPLY *pcCmdReply, PC_CMD_TYPE pcCmdType, unsigned int TimeOut);

//���Խ��������Ƚ���api�ӿ�
RFIDLIBDLL RF_STATUS TestRecvSensitivityCmd(BYTE RandomNum[128], int TimeOut);

//����
RFIDLIBDLL RF_STATUS rfWakeUpTag(IN int wakeupNum);

/*--------------------------��ֲһ��apiDLL.h----------------------------*/
typedef struct  
{
	bool isListPwdPassed;
	bool isValidPwdPassed;
	bool isReadPwdPassed;
	bool isSelectPwdPassed;
	bool isUpdatePwdPassed;
}PWD_STATE;


//��ʼ��
RFIDLIBDLL RF_STATUS rfLibraryInit(IN  BYTE	  comNum,		//�˿ں�
								   IN  DWORD  baudRate	//�����ʣ�112500��128000��9600��
								   );

//���ʼ��
RFIDLIBDLL RF_STATUS rfLibraryDeInit(/*IN HANDLE serialPort*/);

//�̴�
RFIDLIBDLL RF_STATUS rfCollectTag(OUT 	TAG_INFO* 	tagInfoArray,	//��ǩ��Ϣ����ָ��
								  IN	DWORD		tagNums,		//�����С
								  OUT	WORD*		collectTagNums,	//�ռ����ı�ǩ����
								  IN	BYTE		retryTimes,		//�����ռ��ڴ���
								  IN	DWORD		readyCode		//��������
								  );

//�̴�ָ�����ݱ�ǩ
RFIDLIBDLL RF_STATUS rfCollectSpecContTag(IN	WORD		fileID,			//�ļ���ʾ��
										  IN	BYTE		specCont[70],	//ָ������
										  IN	char		datalen,		//ָ�����ݵĳ���(<80�ֽ�)
										  OUT 	TAG_INFO* 	tagInfoArray,	//��ǩ��Ϣ����ָ��
										  OUT	DWORD*		collectTagNums,	//�ռ����ı�ǩ����
										  IN	BYTE		retryTimes,		//�����ռ��ڴ���
										  IN	DWORD		readyCode		//��������
										  );


//���ѵ�����ǩ
RFIDLIBDLL RF_STATUS rfWakeupOneTag(IN	 const BYTE* TID,
									IN	 DWORD		 readyCode,		//��������
									OUT  TAG_INFO* 	 tagInfoArray	//��ǩ��Ϣ����ָ��
									);

//ѡ��
RFIDLIBDLL RF_STATUS rfSelectTag(IN BYTE TID[8]);

//��\�رշ�����
RFIDLIBDLL RF_STATUS rfOpenMonitorSensor(IN  WORD   FileId,		//�ļ�ID(����)
										 IN  BYTE   SensorId,	//������ID(����Ϊ0x01)
										 IN  BYTE   SensorState	//������״̬��0���򿪷�������1���رշ�������
										 );

//�������б�ǩ
RFIDLIBDLL RF_STATUS rfSleepAllTag(IN BYTE 	 sleepTime,		//˯��ʱ��(��λ:1s)
								   IN DWORD  sleepCode,		//˯�߿���
								   IN BYTE 	 listenTime		//����ʱ�䣨����ֵ��11��
								   );

//���߳�ĳһ���ǩ�����б�ǩ
RFIDLIBDLL RF_STATUS rfSleepExOnesTag(IN BYTE 	sleepTime,		//˯��ʱ��(��λ:1s)
									  IN DWORD 	sleepCode,		//˯�߿���
									  IN BYTE 	listenTime,		//����ʱ�䣨����ֵ��11��
									  IN WORD	fileID,			//�ļ���ʶ��
									  IN BYTE	offset,			//ƫ��
									  IN BYTE	dataLen,		//���ݳ���
									  IN BYTE	dataBuffer[]	//�������ݣ�������80�ֽڣ�
									  );

//���߳�ĳ����ǩ�����б�ǩ
RFIDLIBDLL RF_STATUS rfSleepExOneTag(IN BYTE	TID[8],
									 IN BYTE 	sleepTime,		//˯��ʱ��(��λ:1s)
									 IN DWORD 	sleepCode,		//˯�߿���
									 IN BYTE 	listenTime		//����ʱ�䣨����ֵ��11��
									 );

//�б��ļ�
RFIDLIBDLL RF_STATUS rfListFile(IN  BYTE 	listMode,		//�б�ģʽ:02:��ǰѡ���ļ�ǰһ���ļ�,03:��ǰѡ���ļ���һ���ļ�,04:ָ��ƫ����λ���ļ�
								IN	BYTE 	dfRecPointer,	//04ģʽʱ,ָ�����ļ�ƫ����
								IN  DWORD 	fileNums1,		//���뻺����fileAttr_R�Ĵ�С,��λΪFILE_ATTRIBUTE_R��С
								OUT FILE_ATTRIBUTE_R* 	fileAttr_R,//�ļ����Ի�����ָ��
								OUT DWORD* fileNums2		//����ʵ���б��ļ�ʱ�ļ�����
								);

//�б��ļ����£�
RFIDLIBDLL RF_STATUS rfListFileNew(IN  BYTE   listMode,		//�б�ģʽ:02:��ǰѡ���ļ�ǰһ���ļ�,03:��ǰѡ���ļ���һ���ļ�,04:ָ��ƫ����λ���ļ�
								   IN  BYTE   dfRecPointer,	//04ģʽʱ,ָ�����ļ�ƫ����
								   IN  BYTE   opMode,
								   OUT FILE_ATTRIBUTE_R*   fileAttr_R//�ļ����Ի�����ָ��
								   );

//�����ļ�
RFIDLIBDLL RF_STATUS rfCreateFile(IN  FILE_ATTRIBUTE_W*  fileAttr_W);	//�ļ�����:fileAttr_Wָ��

//ѡ���ļ�
RFIDLIBDLL RF_STATUS rfSelectFile(IN   WORD fileID,	
								  OUT  FILE_ATTRIBUTE_R* fileAttr_R	//����ѡ����ļ�����
								  );

//����¼�ļ�
RFIDLIBDLL RF_STATUS rfReadRecFile(IN 	BYTE 	recPointer,			//��¼ƫ����
								   IN 	BYTE 	opMode,				//02:��ǰ��¼�ļ�����һ����¼,03:��ǰѡ���ļ��е���һ����¼,04:��¼ƫ����recOffsetָ��
								   IN 	BYTE 	recOffset,			//��¼ƫ��
								   IN 	BYTE 	dataLen,			//��ȡ����
								   OUT  WORD* 	retLen,				//���ض��������ݳ���
								   OUT  BYTE 	buff[]				//���ļ����ػ�����
								   );

//��͸���ļ�
RFIDLIBDLL RF_STATUS rfReadTransFile(IN  WORD 	fileOffset,		//�ļ�ƫ����
									 IN  WORD  NumToRead,  //��ƫ����fileoffset��ʼ��NumToRead���ֽڣ���Ϊ0��Ĭ�϶����ļ�β
									 OUT WORD* retLen,			//���ض��������ݳ���
									 OUT BYTE 	buff[]		//���ļ����ػ�����
									 );

//д�ļ�
RFIDLIBDLL RF_STATUS rfWriteFile(IN char	FileType,	//�ļ����ͣ�0Ϊ͸����1Ϊ��¼
								 IN WORD 	FileOffset,			//�ļ�ƫ����
								 IN WORD 	WriteLength,		//д�ļ��ĳ���
								 IN char* 	pData				//��д���ļ������ݻ�����
								 );


//������֤
RFIDLIBDLL RF_STATUS rfVerifyPwdFile(IN BYTE 	pwdType,		//��֤��������
									 IN DWORD 	passWord		//����
									 );

//ʹ�ļ���Ч����Ч
RFIDLIBDLL RF_STATUS rfFileValidEn(IN BYTE newFileState);		//�µ��ļ�״̬��0:��״̬Ϊ��Ч,1:��״̬Ϊ��Ч��	


//��ʼ����ǩ
RFIDLIBDLL RF_STATUS rfInitTag(IN TAG_ATTRIBUTE* tagAttr);	//��ǩ����

//�����Ķ���������Ϣ
RFIDLIBDLL RF_STATUS rfSetReaderInfo(IN READER_INFO_W*	readerInfo_w);		//�����Ķ�����Ϣ

//��ȡ�Ķ���������Ϣ
RFIDLIBDLL RF_STATUS rfGetReaderInfo(OUT READER_INFO*	readerInfo);		//��ȡ�Ķ�����Ϣ

//��ȡ��ǩ��Ϣ
RFIDLIBDLL RF_STATUS rfGetTagInfo(OUT TAG_ATTRIBUTE_R*	tagInfo);		//��ȡ��ǩ��Ϣ


//�������ע���ǩ��Ϣ
RFIDLIBDLL RF_STATUS rfCleanAllTag();

//ע��Aes��ǩ
RFIDLIBDLL RF_STATUS rfAesRegTag(IN BYTE*	getTID,
								  IN BYTE*	getMIK,
								  IN BYTE*	getMEK
								  );
//ע��Ecc��ǩ������Ecc��Կ
RFIDLIBDLL RF_STATUS rfEccRegTag(IN BYTE*  getTID,
								 IN BYTE*  getTagPubkey,
								 IN BYTE   keyLen
								 );

//�򿪰�ȫ�Ự
RFIDLIBDLL RF_STATUS rfSafeSession();

//���ǩ����
RFIDLIBDLL RF_STATUS rfMultiTagAlert(IN TID_INFO*	tidArray,		//��ǩ����
									 IN DWORD		tagNums,		//��ǩ����
									 IN BYTE		openOrClose		//�򿪻�رգ�0��1�գ�
									 );


//д��͸���ļ�, 256K����
RFIDLIBDLL RF_STATUS rfWriteLargeTransFile(IN  DWORD 	fileOffset,		//�ļ�ƫ����
										   IN  DWORD 	dataLen,		//���ݳ���
										   OUT DWORD* 	retLen,			//���ض��������ݳ���
										   IN  BYTE 	buff[]			//д���buffer
										   );

//�ļ�ɾ��
RFIDLIBDLL RF_STATUS rfFileDelete();

//�ļ�ϵͳ��ʽ��
RFIDLIBDLL RF_STATUS rfFileSysFormat(IN DWORD pwd);

// �����Ķ�����˽Կ�������Ķ���˽Կ�����Ķ�����Կ����������
RFIDLIBDLL RF_STATUS rfImpoReaderPriKey(OUT  BYTE    pubKeyReader[PKI_PUBLICKEY_LEN],	//�Ķ�����Կ
										OUT  DWORD*	pubLenReader						//�Ķ�����Կ����
										);

// ���ɱ�ǩ��˽Կ�������ǩ˽Կ������ǩ��Կ���������أ������Ķ�����Կ
RFIDLIBDLL RF_STATUS rfImpoTagPriReaderPub(IN	BYTE	TID[8],								//�����ǩID
										   IN  BYTE	pubKeyReader[PKI_PUBLICKEY_LEN],	//�Ķ�����Կ
										   OUT	BYTE	pubKeyTag[PKI_PUBLICKEY_LEN],		//��ǩ��Կ
										   OUT	DWORD*	pubLenTag							//��ǩ��Կ����
										   );

// �����ŵ�
RFIDLIBDLL RF_STATUS rfSetChannel(IN BYTE	newChannel,		//��Χ��0��15
								  IN BYTE	readerOrTag		//0:Reader,1:Tag
								  );

//���ù���
RFIDLIBDLL RF_STATUS rfSetPower(IN	BYTE  newPower,			//�Ķ�����Χ��0��6����ǩ��Χ��0��8
								IN	BYTE  readerOrTag			//0:Reader,1:Tag
								);


//��ǩ���
RFIDLIBDLL RF_STATUS rfKillTag(IN DWORD killTagToken=0);

RFIDLIBDLL RF_STATUS rfMakeKey(OUT	BYTE	w_pubKeyReader[PKI_PUBLICKEY_LEN],	//�Ķ�����Կ
							   OUT	DWORD*	w_pubLenReader,						//�Ķ�����Կ����
							   OUT	BYTE	w_priKeyReader[PKI_PRIVATEKEY_LEN],	//�Ķ���˽Կ
							   OUT  DWORD*	w_priLenReader,						//�Ķ���˽Կ����
							   OUT  BYTE	w_pubKeyTag[PKI_PUBLICKEY_LEN],		//��ǩ��Կ
							   OUT  DWORD*	w_pubLenTag,						//��ǩ��Կ����
							   OUT  BYTE	w_priKeyTag[PKI_PRIVATEKEY_LEN],	//��ǩ˽Կ
							   OUT  DWORD*	w_priLenTag							//��ǩ˽Կ����
							   );

//���з����Ķ������ǩ�Ĺ�Կ��˽Կ
RFIDLIBDLL RF_STATUS rfWirelessSendKey(IN BYTE	 w_pubKeyReader[PKI_PUBLICKEY_LEN],	//�Ķ�����Կ
									   IN DWORD	 w_pubLenReader,						//�Ķ�����Կ����
									   IN BYTE	 w_priKeyReader[PKI_PRIVATEKEY_LEN],	//�Ķ���˽Կ
									   IN DWORD	 w_priLenReader,						//�Ķ���˽Կ����
									   IN BYTE	 w_pubKeyTag[PKI_PUBLICKEY_LEN],		//��ǩ��Կ
									   IN DWORD	 w_pubLenTag,						//��ǩ��Կ����
									   IN BYTE	 w_priKeyTag[PKI_PRIVATEKEY_LEN],	//��ǩ˽Կ
									   IN DWORD	 w_priLenTag							//��ǩ˽Կ����
									   );


//������¼
RFIDLIBDLL RF_STATUS rfSearchRecords(IN  SEARCH_ELEMENT* searchElement,
									 IN  DWORD	elementNums,
									 OUT WORD*	retRecords
									 );

//�������У���ǩ����¼ 
RFIDLIBDLL RF_STATUS rfSearchAlltagRecs(IN SEARCH_ELEMENT*	searchElement,
										IN DWORD  elementNums
										);

//�������У���ǩ��͸��
RFIDLIBDLL RF_STATUS rfSearchAlltagTrans(IN SEARCH_ELEMENT* searchElement,
										 IN DWORD	elementNums
										 );

//����ϵͳ����
RFIDLIBDLL RF_STATUS rfUpdatePwd(IN DWORD	adminPWD,	//�ṩ����Ա����
								 IN WORD	indexPWD,	//��������(0-4)
								 IN BYTE	mode,		//indexΪ�ļ���ʾ��ʱʹ�ã������������(1-4)
								 IN DWORD	newPWD		//������
								 );

//�����������
RFIDLIBDLL RF_STATUS rfMonitorTrConf(IN  WORD	fileID,				//�ļ�ID
									 IN  BYTE	fileOffset,			//�ļ�ƫ�ƣ�0~65500��
									 IN  BYTE	dataLen,			//���ݳ��ȣ�1~80��
									 IN  WORD	timeSlotNum,		//ʱ϶��ţ���0��ʼ��
									 IN  WORD	reportedCycle,		//�ϱ����ڣ�Ϊ������ڱ���ֵ��ÿ��������������б�ǩʱ϶��ȱ�����ȣ�
									 IN  WORD	monitorCycle,		//������ڣ����λΪ1ʱ��λΪ���ӣ����λΪ0��λΪ���ӣ���СȡֵΪ1�룬����Ϊ�㣩
									 IN  WORD	timeSlotWidth,		//ʱ϶��ȣ���С��20ms��
									 IN  WORD	timeSlotSum,		//����ʱ϶������һ����������ڵ�ʱ϶������ʱ϶����*��ʱ϶��ȣ�<(�������-40ms)��
									 IN  BYTE	reportIndication,	//�շ�ָʾ������ֵ��0��
									 IN  BYTE   *tmpTid
									 );

//�����ʼ����
RFIDLIBDLL RF_STATUS rfMonitorTrStart();

//���ֹͣ����
RFIDLIBDLL RF_STATUS rfMonitorTrStop(IN WORD monitorCycle);	 //������ڣ���λ���룩

//�����������
RFIDLIBDLL RF_STATUS rfMonitorTrReport(IN  DWORD	waitTime, //����λ���룩
									   OUT CYCLE_INFO*	CycleInfo
									   );

//���¹���Ա����
RFIDLIBDLL RF_STATUS rfUpdateAdminPwd(IN  DWORD	 oldPWD, //������
									  IN  DWORD	 newPWD	 //������
									  );

//��ȡ��ǩ��ȫ����
RFIDLIBDLL RF_STATUS rfGetTagSafeParam(OUT WORD* keyParam,		//��Կ����
									   OUT WORD* safePowerParam	//��ȫ��������
									   );

//���������
RFIDLIBDLL RF_STATUS rfRequireRandom(IN  BYTE randomLenth,		//��������ȣ�s��1-4��
									 OUT BYTE randomNums[8]		//�����
									 );

//���ݹ㲥
RFIDLIBDLL RF_STATUS rfDataBoardcast(IN DWORD readyToken,			//��������
									 IN BYTE  boardcastData[100]	//�㲥����
									 );

RFIDLIBDLL RF_STATUS rfWriteOrReadFile(IN	WORD  fileId, 
									   IN	BYTE  writeBuffer[10], //д����
									   OUT	BYTE  readBuffer[10] //������
									   );

//���ý���ģʽ
RFIDLIBDLL RF_STATUS rfSetRecvModel();

//ֹͣ����ģʽ
RFIDLIBDLL RF_STATUS rfStopRecvModel();

//���ô��ز�
RFIDLIBDLL RF_STATUS rfSendPurewave();

//ֹͣ���ز�
RFIDLIBDLL RF_STATUS rfStopPurewave();

//���������Ϣ
RFIDLIBDLL RF_STATUS rfSendModulInfo();

//ֹͣ������Ϣ
RFIDLIBDLL RF_STATUS rfStopModulInfo();

#endif