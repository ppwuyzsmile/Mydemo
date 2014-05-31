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

//查询USB设备
RFIDLIBDLL RF_STATUS SearchHidDevice();

//关闭USB设备
RFIDLIBDLL void CloseHidDevice();

// 通过USB发送数据
// RFIDLIBDLL void SendDataToUsb( IN char *VID,		
// 							   IN char *PID, 
// 							   IN char *SendData
// 							   );
// 
// 通过USB接收数据
// RFIDLIBDLL void ReceiveDataFromUsb(	IN  char *VID,
// 									IN  char *PID,
// 									IN  int numTimeOut,//超时时间，单位毫秒
// 									OUT char *ReceiveData
// 								    );
// 
// RFIDLIBDLL void UsbReceiveThread();
// 
//建立套接字连接,返回值为套接字sockRet
RFIDLIBDLL RF_STATUS StartConnect(IN int  m_Port,
								  IN char *adressIP);

//关闭套接字
RFIDLIBDLL void CloseConnect();



// 网口发送数据
// RFIDLIBDLL void SendDataToSocket( IN SOCKET sockSrv, 
// 								  IN char *SendData
// 								  );

// 网口接收数据
// RFIDLIBDLL void RceiveDataFromSocket( IN SOCKET sockSrv);

// RFIDLIBDLL void SocketReceiveThread();

//测试发送api接口
RFIDLIBDLL RF_STATUS TestSendCmd(PC_CMD_DATA* pData);

//测试接收api接口
RFIDLIBDLL RF_STATUS TestRecvCmd(PC_CMD_REPLY *pcCmdReply, PC_CMD_TYPE pcCmdType, unsigned int TimeOut);

//测试接收灵敏度接收api接口
RFIDLIBDLL RF_STATUS TestRecvSensitivityCmd(BYTE RandomNum[128], int TimeOut);

//唤醒
RFIDLIBDLL RF_STATUS rfWakeUpTag(IN int wakeupNum);

/*--------------------------移植一期apiDLL.h----------------------------*/
typedef struct  
{
	bool isListPwdPassed;
	bool isValidPwdPassed;
	bool isReadPwdPassed;
	bool isSelectPwdPassed;
	bool isUpdatePwdPassed;
}PWD_STATE;


//初始化
RFIDLIBDLL RF_STATUS rfLibraryInit(IN  BYTE	  comNum,		//端口号
								   IN  DWORD  baudRate	//波特率（112500、128000、9600）
								   );

//解初始化
RFIDLIBDLL RF_STATUS rfLibraryDeInit(/*IN HANDLE serialPort*/);

//盘存
RFIDLIBDLL RF_STATUS rfCollectTag(OUT 	TAG_INFO* 	tagInfoArray,	//标签信息数据指针
								  IN	DWORD		tagNums,		//数组大小
								  OUT	WORD*		collectTagNums,	//收集到的标签个数
								  IN	BYTE		retryTimes,		//设置收集期次数
								  IN	DWORD		readyCode		//就绪口令
								  );

//盘存指定内容标签
RFIDLIBDLL RF_STATUS rfCollectSpecContTag(IN	WORD		fileID,			//文件标示符
										  IN	BYTE		specCont[70],	//指定内容
										  IN	char		datalen,		//指定内容的长度(<80字节)
										  OUT 	TAG_INFO* 	tagInfoArray,	//标签信息数据指针
										  OUT	DWORD*		collectTagNums,	//收集到的标签个数
										  IN	BYTE		retryTimes,		//设置收集期次数
										  IN	DWORD		readyCode		//就绪口令
										  );


//唤醒单个标签
RFIDLIBDLL RF_STATUS rfWakeupOneTag(IN	 const BYTE* TID,
									IN	 DWORD		 readyCode,		//就绪口令
									OUT  TAG_INFO* 	 tagInfoArray	//标签信息数据指针
									);

//选卡
RFIDLIBDLL RF_STATUS rfSelectTag(IN BYTE TID[8]);

//打开\关闭蜂鸣器
RFIDLIBDLL RF_STATUS rfOpenMonitorSensor(IN  WORD   FileId,		//文件ID(任意)
										 IN  BYTE   SensorId,	//传感器ID(必须为0x01)
										 IN  BYTE   SensorState	//传感器状态（0：打开蜂鸣器，1：关闭蜂鸣器）
										 );

//休眠所有标签
RFIDLIBDLL RF_STATUS rfSleepAllTag(IN BYTE 	 sleepTime,		//睡眠时间(单位:1s)
								   IN DWORD  sleepCode,		//睡眠口令
								   IN BYTE 	 listenTime		//监听时间（保留值：11）
								   );

//休眠除某一类标签外所有标签
RFIDLIBDLL RF_STATUS rfSleepExOnesTag(IN BYTE 	sleepTime,		//睡眠时间(单位:1s)
									  IN DWORD 	sleepCode,		//睡眠口令
									  IN BYTE 	listenTime,		//监听时间（保留值：11）
									  IN WORD	fileID,			//文件标识符
									  IN BYTE	offset,			//偏移
									  IN BYTE	dataLen,		//数据长度
									  IN BYTE	dataBuffer[]	//数据内容（不超过80字节）
									  );

//休眠除某个标签外所有标签
RFIDLIBDLL RF_STATUS rfSleepExOneTag(IN BYTE	TID[8],
									 IN BYTE 	sleepTime,		//睡眠时间(单位:1s)
									 IN DWORD 	sleepCode,		//睡眠口令
									 IN BYTE 	listenTime		//监听时间（保留值：11）
									 );

//列表文件
RFIDLIBDLL RF_STATUS rfListFile(IN  BYTE 	listMode,		//列表模式:02:当前选中文件前一个文件,03:当前选择文件后一个文件,04:指定偏移量位置文件
								IN	BYTE 	dfRecPointer,	//04模式时,指定的文件偏移量
								IN  DWORD 	fileNums1,		//输入缓冲区fileAttr_R的大小,单位为FILE_ATTRIBUTE_R大小
								OUT FILE_ATTRIBUTE_R* 	fileAttr_R,//文件属性缓冲区指针
								OUT DWORD* fileNums2		//返回实际列表文件时文件个数
								);

//列表文件（新）
RFIDLIBDLL RF_STATUS rfListFileNew(IN  BYTE   listMode,		//列表模式:02:当前选中文件前一个文件,03:当前选择文件后一个文件,04:指定偏移量位置文件
								   IN  BYTE   dfRecPointer,	//04模式时,指定的文件偏移量
								   IN  BYTE   opMode,
								   OUT FILE_ATTRIBUTE_R*   fileAttr_R//文件属性缓冲区指针
								   );

//创建文件
RFIDLIBDLL RF_STATUS rfCreateFile(IN  FILE_ATTRIBUTE_W*  fileAttr_W);	//文件属性:fileAttr_W指针

//选择文件
RFIDLIBDLL RF_STATUS rfSelectFile(IN   WORD fileID,	
								  OUT  FILE_ATTRIBUTE_R* fileAttr_R	//返回选择的文件属性
								  );

//读记录文件
RFIDLIBDLL RF_STATUS rfReadRecFile(IN 	BYTE 	recPointer,			//记录偏移量
								   IN 	BYTE 	opMode,				//02:当前记录文件内下一条记录,03:当前选中文件中的上一条记录,04:记录偏移由recOffset指定
								   IN 	BYTE 	recOffset,			//记录偏移
								   IN 	BYTE 	dataLen,			//读取长度
								   OUT  WORD* 	retLen,				//返回读到的数据长度
								   OUT  BYTE 	buff[]				//读文件返回缓冲区
								   );

//读透明文件
RFIDLIBDLL RF_STATUS rfReadTransFile(IN  WORD 	fileOffset,		//文件偏移量
									 IN  WORD  NumToRead,  //从偏移量fileoffset开始读NumToRead个字节，若为0，默认读到文件尾
									 OUT WORD* retLen,			//返回读到的数据长度
									 OUT BYTE 	buff[]		//读文件返回缓冲区
									 );

//写文件
RFIDLIBDLL RF_STATUS rfWriteFile(IN char	FileType,	//文件类型，0为透明，1为记录
								 IN WORD 	FileOffset,			//文件偏移量
								 IN WORD 	WriteLength,		//写文件的长度
								 IN char* 	pData				//待写入文件的数据缓冲区
								 );


//密码验证
RFIDLIBDLL RF_STATUS rfVerifyPwdFile(IN BYTE 	pwdType,		//验证密码类型
									 IN DWORD 	passWord		//密码
									 );

//使文件有效或无效
RFIDLIBDLL RF_STATUS rfFileValidEn(IN BYTE newFileState);		//新的文件状态（0:新状态为无效,1:新状态为有效）	


//初始化标签
RFIDLIBDLL RF_STATUS rfInitTag(IN TAG_ATTRIBUTE* tagAttr);	//标签属性

//设置阅读器基本信息
RFIDLIBDLL RF_STATUS rfSetReaderInfo(IN READER_INFO_W*	readerInfo_w);		//设置阅读器信息

//获取阅读器基本信息
RFIDLIBDLL RF_STATUS rfGetReaderInfo(OUT READER_INFO*	readerInfo);		//获取阅读器信息

//获取标签信息
RFIDLIBDLL RF_STATUS rfGetTagInfo(OUT TAG_ATTRIBUTE_R*	tagInfo);		//获取标签信息


//清除所有注册标签信息
RFIDLIBDLL RF_STATUS rfCleanAllTag();

//注册Aes标签
RFIDLIBDLL RF_STATUS rfAesRegTag(IN BYTE*	getTID,
								  IN BYTE*	getMIK,
								  IN BYTE*	getMEK
								  );
//注册Ecc标签，导入Ecc公钥
RFIDLIBDLL RF_STATUS rfEccRegTag(IN BYTE*  getTID,
								 IN BYTE*  getTagPubkey,
								 IN BYTE   keyLen
								 );

//打开安全会话
RFIDLIBDLL RF_STATUS rfSafeSession();

//多标签报警
RFIDLIBDLL RF_STATUS rfMultiTagAlert(IN TID_INFO*	tidArray,		//标签数组
									 IN DWORD		tagNums,		//标签个数
									 IN BYTE		openOrClose		//打开或关闭（0开1闭）
									 );


//写大透明文件, 256K以内
RFIDLIBDLL RF_STATUS rfWriteLargeTransFile(IN  DWORD 	fileOffset,		//文件偏移量
										   IN  DWORD 	dataLen,		//数据长度
										   OUT DWORD* 	retLen,			//返回读到的数据长度
										   IN  BYTE 	buff[]			//写入的buffer
										   );

//文件删除
RFIDLIBDLL RF_STATUS rfFileDelete();

//文件系统格式化
RFIDLIBDLL RF_STATUS rfFileSysFormat(IN DWORD pwd);

// 生成阅读器公私钥，导入阅读器私钥，将阅读器公钥保存至本地
RFIDLIBDLL RF_STATUS rfImpoReaderPriKey(OUT  BYTE    pubKeyReader[PKI_PUBLICKEY_LEN],	//阅读器公钥
										OUT  DWORD*	pubLenReader						//阅读器公钥长度
										);

// 生成标签公私钥，导入标签私钥，将标签公钥保存至本地，导入阅读器公钥
RFIDLIBDLL RF_STATUS rfImpoTagPriReaderPub(IN	BYTE	TID[8],								//输入标签ID
										   IN  BYTE	pubKeyReader[PKI_PUBLICKEY_LEN],	//阅读器公钥
										   OUT	BYTE	pubKeyTag[PKI_PUBLICKEY_LEN],		//标签公钥
										   OUT	DWORD*	pubLenTag							//标签公钥长度
										   );

// 设置信道
RFIDLIBDLL RF_STATUS rfSetChannel(IN BYTE	newChannel,		//范围从0到15
								  IN BYTE	readerOrTag		//0:Reader,1:Tag
								  );

//设置功率
RFIDLIBDLL RF_STATUS rfSetPower(IN	BYTE  newPower,			//阅读器范围从0到6，标签范围从0到8
								IN	BYTE  readerOrTag			//0:Reader,1:Tag
								);


//标签灭活
RFIDLIBDLL RF_STATUS rfKillTag(IN DWORD killTagToken=0);

RFIDLIBDLL RF_STATUS rfMakeKey(OUT	BYTE	w_pubKeyReader[PKI_PUBLICKEY_LEN],	//阅读器公钥
							   OUT	DWORD*	w_pubLenReader,						//阅读器公钥长度
							   OUT	BYTE	w_priKeyReader[PKI_PRIVATEKEY_LEN],	//阅读器私钥
							   OUT  DWORD*	w_priLenReader,						//阅读器私钥长度
							   OUT  BYTE	w_pubKeyTag[PKI_PUBLICKEY_LEN],		//标签公钥
							   OUT  DWORD*	w_pubLenTag,						//标签公钥长度
							   OUT  BYTE	w_priKeyTag[PKI_PRIVATEKEY_LEN],	//标签私钥
							   OUT  DWORD*	w_priLenTag							//标签私钥长度
							   );

//空中发送阅读器与标签的公钥、私钥
RFIDLIBDLL RF_STATUS rfWirelessSendKey(IN BYTE	 w_pubKeyReader[PKI_PUBLICKEY_LEN],	//阅读器公钥
									   IN DWORD	 w_pubLenReader,						//阅读器公钥长度
									   IN BYTE	 w_priKeyReader[PKI_PRIVATEKEY_LEN],	//阅读器私钥
									   IN DWORD	 w_priLenReader,						//阅读器私钥长度
									   IN BYTE	 w_pubKeyTag[PKI_PUBLICKEY_LEN],		//标签公钥
									   IN DWORD	 w_pubLenTag,						//标签公钥长度
									   IN BYTE	 w_priKeyTag[PKI_PRIVATEKEY_LEN],	//标签私钥
									   IN DWORD	 w_priLenTag							//标签私钥长度
									   );


//检索记录
RFIDLIBDLL RF_STATUS rfSearchRecords(IN  SEARCH_ELEMENT* searchElement,
									 IN  DWORD	elementNums,
									 OUT WORD*	retRecords
									 );

//检索所有（标签）记录 
RFIDLIBDLL RF_STATUS rfSearchAlltagRecs(IN SEARCH_ELEMENT*	searchElement,
										IN DWORD  elementNums
										);

//检索所有（标签）透明
RFIDLIBDLL RF_STATUS rfSearchAlltagTrans(IN SEARCH_ELEMENT* searchElement,
										 IN DWORD	elementNums
										 );

//更新系统口令
RFIDLIBDLL RF_STATUS rfUpdatePwd(IN DWORD	adminPWD,	//提供管理员密码
								 IN WORD	indexPWD,	//口令索引(0-4)
								 IN BYTE	mode,		//index为文件标示符时使用，区分密码类别(1-4)
								 IN DWORD	newPWD		//新密码
								 );

//监测配置命令
RFIDLIBDLL RF_STATUS rfMonitorTrConf(IN  WORD	fileID,				//文件ID
									 IN  BYTE	fileOffset,			//文件偏移（0~65500）
									 IN  BYTE	dataLen,			//数据长度（1~80）
									 IN  WORD	timeSlotNum,		//时隙序号（从0开始）
									 IN  WORD	reportedCycle,		//上报周期（为监测周期倍数值，每个监测周期内所有标签时隙宽度必须相等）
									 IN  WORD	monitorCycle,		//监测周期（最高位为1时单位为分钟，最高位为0单位为秒钟，最小取值为1秒，不能为零）
									 IN  WORD	timeSlotWidth,		//时隙宽度（不小于20ms）
									 IN  WORD	timeSlotSum,		//传输时隙总数（一个监测周期内的时隙数，（时隙数）*（时隙宽度）<(监测周期-40ms)）
									 IN  BYTE	reportIndication,	//收发指示（保留值：0）
									 IN  BYTE   *tmpTid
									 );

//监测起始命令
RFIDLIBDLL RF_STATUS rfMonitorTrStart();

//监测停止命令
RFIDLIBDLL RF_STATUS rfMonitorTrStop(IN WORD monitorCycle);	 //监测周期（单位：秒）

//监测周期数据
RFIDLIBDLL RF_STATUS rfMonitorTrReport(IN  DWORD	waitTime, //（单位：秒）
									   OUT CYCLE_INFO*	CycleInfo
									   );

//更新管理员密码
RFIDLIBDLL RF_STATUS rfUpdateAdminPwd(IN  DWORD	 oldPWD, //旧密码
									  IN  DWORD	 newPWD	 //新密码
									  );

//获取标签安全参数
RFIDLIBDLL RF_STATUS rfGetTagSafeParam(OUT WORD* keyParam,		//密钥参数
									   OUT WORD* safePowerParam	//安全能力参数
									   );

//请求随机数
RFIDLIBDLL RF_STATUS rfRequireRandom(IN  BYTE randomLenth,		//随机数长度（s传1-4）
									 OUT BYTE randomNums[8]		//随机数
									 );

//数据广播
RFIDLIBDLL RF_STATUS rfDataBoardcast(IN DWORD readyToken,			//就绪口令
									 IN BYTE  boardcastData[100]	//广播数据
									 );

RFIDLIBDLL RF_STATUS rfWriteOrReadFile(IN	WORD  fileId, 
									   IN	BYTE  writeBuffer[10], //写数据
									   OUT	BYTE  readBuffer[10] //读数据
									   );

//设置接收模式
RFIDLIBDLL RF_STATUS rfSetRecvModel();

//停止接收模式
RFIDLIBDLL RF_STATUS rfStopRecvModel();

//设置纯载波
RFIDLIBDLL RF_STATUS rfSendPurewave();

//停止纯载波
RFIDLIBDLL RF_STATUS rfStopPurewave();

//发射调制信息
RFIDLIBDLL RF_STATUS rfSendModulInfo();

//停止调制信息
RFIDLIBDLL RF_STATUS rfStopModulInfo();

#endif