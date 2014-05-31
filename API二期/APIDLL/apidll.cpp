#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <time.h>
#include "apidll.h"
#include "dataType.h"
#include "makeKey.h"
extern "C"
{
#include "hidsdi.h"
#include "setupapi.h"
};

#pragma comment(lib,"Ws2_32.lib")

//--------------GLOBAL-----------------

GLOBAL  HANDLE  hCom; //在usb通信中用于读写文件的句柄全局变量
GLOBAL  SOCKET	sockSrv; //网口通信套接字全局变量
GLOBAL  BYTE  CommunicationFlag;
GLOBAL	BOOL  isTIDselected = FALSE;
GLOBAL	char  TID[8]; //用于保存当前选则的TID
GLOBAL	PWD_STATE  pwdState = {false, false, false, false, false}; //List  Vaild Read  Selct
GLOBAL	unsigned short  listFileID[100]; //列表文件后用于保存所有文件ID的数组
GLOBAL	DWORD  listFileNums; //用于保存列表文件的个数
GLOBAL	FILE_ATTRIBUTE_R  selectedFile;	//用于保存当前选则的文件
GLOBAL	DWORD  pubPwd;

//-------------------------------------

RF_STATUS SearchHidDevice()	//寻找可用于通信的USB设备
{
	// 查找本系统中HID类的GUID标识
	PRINT("正在查找系统中HID类的GUID标识......\n");
	GUID guidHID;
	HidD_GetHidGuid(&guidHID); //取得HID设备的接口类GUID
	PRINT("HID类的GUID标识为......\n");

	PRINT("%08x-%04x-%04x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\n",
		guidHID.Data1,guidHID.Data2,guidHID.Data3,guidHID.Data4[0],
		guidHID.Data4[1],guidHID.Data4[2],guidHID.Data4[3],guidHID.Data4[4],
		guidHID.Data4[5],guidHID.Data4[6],guidHID.Data4[7]);

	// 准备查找符合HID规范的USB设备
	HDEVINFO hDevInfo = SetupDiGetClassDevs(&guidHID,NULL,0,   //得到HID设备类信息，此函数返回由guidHID指定的所有设备的一个信息集合的句柄。
		DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (hDevInfo == INVALID_HANDLE_VALUE)
	{
		PRINT("查找USB设备错误......\n");
		return 0;
	}

	// 查找USB设备接口
	/*UINT nIndex = 0;*/
	PRINT("正在查找可用的USB设备......\n");
	SP_DEVICE_INTERFACE_DATA strtInterfaceData;
	strtInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);//by ppwu
	UINT index = 0;
	for (int i=0; i<10; i++)
	{
		BOOL bSuccess = SetupDiEnumDeviceInterfaces(hDevInfo,NULL,&guidHID,index++,&strtInterfaceData);//获取一个设备接口信息的函数，此函数可以从设备集合中获取某个设备接口信息
		if (!bSuccess)
		{
			PRINT("抱歉，未找到可用的USB设备......\n");
			SetupDiDestroyDeviceInfoList(hDevInfo);//销毁一个设备信息集合，并且释放所有关联的内存
			break;
		}
		else
		{
			if (strtInterfaceData.Flags == SPINT_ACTIVE)
			{
				DWORD strSize=0, requiesize=0;
				PSP_DEVICE_INTERFACE_DETAIL_DATA strtDetailData;

				SetupDiGetDeviceInterfaceDetail(hDevInfo,&strtInterfaceData,NULL,0,&strSize,NULL);//获取一个指定设备接口的详细信息
				requiesize = strSize;

				strtDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(requiesize);
				strtDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

				if (!SetupDiGetDeviceInterfaceDetail(hDevInfo,&strtInterfaceData,
					strtDetailData,strSize,&requiesize,NULL))
				{
					PRINT("查找设备路径时出错!\n");
					SetupDiDestroyDeviceInfoList(hDevInfo);//销毁一个设备信息集合，并且释放所有关联的内存
					break;
				}

				PRINT("设备的路径：%s\n",strtDetailData->DevicePath);
				PRINT("打开设备路径......\n");

				hCom = CreateFile(strtDetailData->DevicePath,
					GENERIC_READ | GENERIC_WRITE,
					FILE_SHARE_READ | FILE_SHARE_WRITE,
					NULL,
					OPEN_EXISTING,
					FILE_FLAG_OVERLAPPED,
					NULL);
				if (hCom == INVALID_HANDLE_VALUE)
				{
					PRINT("无法打开通信端口......\n");
					continue;
					//SetupDiDestroyDeviceInfoList(hDevInfo);//销毁一个设备信息集合，并且释放所有关联的内存
					//break;
				}

				// 查询设备标识
				HIDD_ATTRIBUTES strtAttrib;
				PRINT("正在读取设备的标识......\n");

				if (!HidD_GetAttributes(hCom, &strtAttrib))	//取得VID、PID等信息
				{
					PRINT("查询设备状态时出错!\n");
					CloseHandle(hCom);
					SetupDiDestroyDeviceInfoList(hDevInfo);//销毁一个设备信息集合，并且释放所有关联的内存
					break;
				}
				PRINT("读取设备ID成功!\n");

				CommunicationFlag = FALSE;//设置USB收发数据标识位

				// 显示供应商标识
				PRINT("0x%x\n",strtAttrib.VendorID);

				// 显示产品标识
				PRINT("0x%x\n",strtAttrib.ProductID);

				// 显示产品版本号
				PRINT("%d\n",strtAttrib.VersionNumber);

				//index++;
				SetupDiDestroyDeviceInfoList(hDevInfo);
				return 1;

				//释放资源
				//CloseHandle(hCom);
				//SetupDiDestroyDeviceInfoList(hDevInfo);
			}
		}
	}
	return 0;
}

void CloseHidDevice()
{
	CommunicationFlag = 3;//关闭通信标识位
	CloseHandle(hCom);
}

static int SendDataToUsb(IN char *VID,
				         IN char *PID,
				         IN PC_CMD_DATA *SendData)	//给USB发送数据
{
	char *strVID, *strPID;

	strVID = VID;
	strPID = PID;

	if (!strcmp((const char*)strVID, "0x1fc9") && !strcmp((const char*)strPID, "0x107"))
	{
		BOOL	bResult = 0;
		DWORD	nBytesSend = 0;
		WORD	sendLen;
// 		PHIDP_PREPARSED_DATA    preparsedData;
// 		HIDP_CAPS               hidCapabilities;
		OVERLAPPED	WriteOverLapped = {0};	//此处注意要初始化OVERLAPPED，否则会报错误6

		WriteOverLapped.hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
// 		HidD_GetPreparsedData(hCom, &preparsedData);
// 		HidP_GetCaps(preparsedData, &hidCapabilities);			//在这里获取了其输出缓冲区长度，OutputReportByteLength = 64

		sendLen = sizeof(PC_CMD_DATA);
		if (sendLen <= CHAR_NUM-4)
		{
			MULTI_PACKET_SEND_RECV sendPacket;
			sendPacket.usbFlag = 0;//USB发送数据标识位，必须为0
			sendPacket.pkgNUM = 0;//表示分包序号
			sendPacket.endFlag = 1;//表示数据包发送结束标志，0表示未发送完，1表示发送完成
			sendPacket.dataLen = sendLen;

			memcpy(sendPacket.dataBuf, SendData, sendLen);
			WriteFile(hCom, &sendPacket, 64, NULL, &WriteOverLapped);           //因为其输出缓冲区长度就是64，加上开头的ReportID，真正数据长度
			if (WaitForSingleObject(WriteOverLapped.hEvent, TIME_OUT) != WAIT_OBJECT_0) //超时判断
			{																			//就是63（而不是我们之前理解的，数据长度64，加一个ReportID，一共65）
				PRINT("写USB数据失败\n");													//因为使用了异步，所以要等待发送完成事件的产生
				return -1;
			}

			bResult = GetOverlappedResult(hCom, &WriteOverLapped, &nBytesSend, FALSE);	//真正判断是否发送成功是使用这个函数，并观察bResult是否为0，若为0，则说明发送失败，出错了，再用GetLastError
			if (!bResult && nBytesSend == 0)											//不为0（我观察的是1）则说明发送成功，从nBytesSend(此时被赋值为64)也可得知发送是成功了
			{																			//此时不应该调用GetLastError，如果调用的话，会告诉你997错误（I/O正在发送）,但其实是发送成功了的.
				PRINT("发送数据错误！！！\n");
				return -1;
			}
			PRINT("发送成功，数据为：%s\n", &sendPacket.dataBuf);
			return 0;
		}

		else if (sendLen > CHAR_NUM-4)//分包处理
		{
			int bagNUM = 0;
			MULTI_PACKET_SEND_RECV sendPacket;

			while (sendLen > 0)
			{
				sendPacket.usbFlag = 0;//USB发送数据标识位，必须为0
				sendPacket.pkgNUM = bagNUM++;//表示分包序号
				sendPacket.dataLen = ((sendLen/60)>0) ? 60:(sendLen%60);

				if (sendLen/60 > 0)
				{
					sendPacket.endFlag = 0;//表示数据包发送结束标志，0表示未发送完，1表示发送完成
					memcpy(sendPacket.dataBuf, ((char*)SendData) + ((bagNUM-1)*60), 60);
					sendLen -= 60;
				}
				else
				{
					sendPacket.endFlag = 1;//第二个字节置为1表示分包发送结束
					memcpy(sendPacket.dataBuf, ((char*)SendData) + ((bagNUM-1)*60), sendPacket.dataLen);
					sendLen -= sendPacket.dataLen;
				}

				WriteFile(hCom, &sendPacket, 64, NULL, &WriteOverLapped);
				if (WaitForSingleObject(WriteOverLapped.hEvent, TIME_OUT) != WAIT_OBJECT_0) //超时判断
				{
					PRINT("写USB数据失败\n");
					return -1;
				}

				bResult = GetOverlappedResult(hCom, &WriteOverLapped, &nBytesSend, FALSE);
				if (!bResult && nBytesSend == 0)
				{
					PRINT("发送数据错误！！！\n");
					return -1;
				}
				//Sleep(5);
			}
			PRINT("发送成功，数据为：%s\n", sendPacket.dataBuf);
			return 0;
		}
	}
	return -1;
}

static int ReceiveDataFromUsb(IN  char  *VID,
							  IN  char  *PID,
						      IN  WORD  numTimeOut,
							  IN  PC_CMD_TYPE  pcCmdType,
						      OUT PC_CMD_REPLY*  RecvData)	//接收USB数据
{
	char recvBuf[256]; //接收缓冲区
	char *strVID, *strPID;
	BOOL recvFlag = TRUE;
	strVID = VID;
	strPID = PID;

	if (!strcmp((const char*)strVID, "0x1fc9") && !strcmp((const char*)strPID, "0x107"))
	{
		MULTI_PACKET_SEND_RECV	recvPacket;
// 		PHIDP_PREPARSED_DATA    preparsedData;
// 		HIDP_CAPS               hidCapabilities;
		OVERLAPPED				ReadOverLapped;

		memset(&ReadOverLapped, 0, sizeof(OVERLAPPED));	//此处注意要初始化OVERLAPPED，否则会报错误6
// 		HidD_GetPreparsedData(hCom, &preparsedData);
// 		HidP_GetCaps(preparsedData, &hidCapabilities);
		ReadOverLapped.hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);

		while (recvFlag)
		{
			int		bResult = 0;
			DWORD   nBytesGet = 0;
			memset(&recvPacket, 0, sizeof(MULTI_PACKET_SEND_RECV));

			//Sleep(10);
			ReadFile(hCom, &recvPacket, /*hidCapabilities.InputReportByteLength*/64, NULL, &ReadOverLapped);
			if (WaitForSingleObject(ReadOverLapped.hEvent, TIME_OUT*8) == WAIT_TIMEOUT) //超时判断
			{
				PRINT("读取USB数据失败\n");
				return 0;
			}

			bResult = GetOverlappedResult(hCom, &ReadOverLapped, &nBytesGet, FALSE);
			if (!bResult && nBytesGet == 0)
			{
				PRINT("接收数据失败！！！\n");
				return 0;
			}

			if (recvPacket.endFlag != 1)
			{
				memcpy(&recvBuf[recvPacket.pkgNUM * 60], recvPacket.dataBuf, recvPacket.dataLen);
			}
			else
			{
				memcpy(&recvBuf[recvPacket.pkgNUM * 60], recvPacket.dataBuf, recvPacket.dataLen);
				recvFlag = FALSE;
			}
		}

		//memcpy(RecvData, recvBuf, sizeof(recvBuf));
		memcpy(RecvData, recvBuf, sizeof(PC_CMD_REPLY));
		if (RecvData->cmdType != pcCmdType ||
			RecvData->SIG_START != SIG_START_NUM ||
			RecvData->SIG_END != SIG_END_NUM)
		{
			PRINT("接收数据帧不对！！！\n");
			return 0;
		}
		PRINT("接收成功\n");
		return 1;
	}
	return 0;
}


RF_STATUS StartConnect(IN int   m_Port,
					   IN char* adressIP)
{
	int		 err;
	WORD	 wVersionRequested;
	WSADATA  wsaData;

	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);

	if (err != 0)
	{
		PRINT("初始化失败！\n");
		return 0;
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion != 2))
	{
		WSACleanup();
		return 0;
	}

	//创建用于监听的套接字
	sockSrv = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == sockSrv)
	{
		PRINT("Socket 创建失败，Exit!\n");
		return 0;
	}

	WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	//设置发送超时
// 	struct timeval sendtimeout;
// 	sendtimeout.tv_sec = 10;//秒
// 	sendtimeout.tv_usec = 10;//微秒
	int sendtimeout = TIME_OUT*5; //5s
	if(setsockopt(sockSrv, SOL_SOCKET, SO_SNDTIMEO, (char*)&sendtimeout, sizeof(int)) == SOCKET_ERROR)
	{
		PRINT("set send timeout failed\n");
		return 0;
	}

	//设置接收超时
// 	struct timeval recvtimeout;
// 	recvtimeout.tv_sec = 20;//秒
// 	recvtimeout.tv_usec = 10;//微秒
	int recvtimeout = TIME_OUT*20; //20s
	if(setsockopt(sockSrv, SOL_SOCKET, SO_RCVTIMEO, (char*)&recvtimeout, sizeof(int)) == SOCKET_ERROR)
	{
		PRINT("set recv timeout failed\n");
		return 0;
	}

	//start
	/*后期根据网络数据环境，看是否有必要改成非阻塞模式*/
	//end

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr(adressIP);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(m_Port);//普通用户注册端口号范围1024~49151

	if (connect(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) != 0)
	{
		PRINT("连接失败\n");
		return 0;
	}

	PRINT("连接成功，等待收发数据......\n");
	CommunicationFlag = TRUE;//设置网口收发数据标识位

	return 1;
}

void CloseConnect()
{
	CommunicationFlag = 3;
//	sockSrv = INVALID_SOCKET;
// 	BOOL bDontLinger = FALSE; //调用closesocket函数，不等待TIME_WAIT超时时间后返回
// 	setsockopt(sockSrv, SOL_SOCKET, SO_DONTLINGER, (const char*)&bDontLinger, sizeof(BOOL));

	BOOL bReuseaddr=TRUE; //调用closesocket函数，等待TIME_WAIT超时时间后返回
	setsockopt(sockSrv, SOL_SOCKET, SO_REUSEADDR, (const char*)&bReuseaddr, sizeof(BOOL));
	closesocket(sockSrv);
	WSACleanup();
}

static int SendDataToSocket(PC_CMD_DATA*  SendData)
{
// 	struct fd_set wfds;
// 	struct timeval SelectTimeout;
// 	SelectTimeout.tv_sec = 0;
// 	SelectTimeout.tv_usec = 3000;
// 
// 	FD_ZERO(&wfds); //清空集合  
// 	FD_SET(sockSrv, &wfds); //将fp添加到集合，后面的FD_ISSET和FD_SET没有必然关系，这里是添加检测
// 	int RetInfo = select(0, NULL, &wfds, NULL, &SelectTimeout);
// 	if(RetInfo <= 0)
// 	{
// 		//在等待时间过程中，没有网络数据可读，超时返回。
// 		return 0;
// 	}

	char sendBuf[SEND_TCP_DATA];
	int datalen = sizeof(PC_CMD_DATA);
	memcpy(sendBuf, SendData, datalen);

	int Ret = send(sockSrv, sendBuf, datalen, 0);
	//int ret = WSAGetLastError();
	if (Ret == SOCKET_ERROR)
	{
		PRINT("发送的SOCKET数据失败或者发送超时！\n");
		return -1;
	}

	PRINT("发送的SOCKET数据为：%s\n", sendBuf);
	return 0;
}

static int RceiveDataFromSocket(IN  WORD           TimeOut,
								IN  PC_CMD_TYPE    pcCmdType,
								OUT PC_CMD_REPLY   *RecvData)
{
	if(sockSrv == INVALID_SOCKET) //套接字已关闭
		return 0;

	struct fd_set rfds;
	struct timeval SelectTimeout;
	SelectTimeout.tv_sec = TimeOut / TIME_OUT; //单位为：s
	SelectTimeout.tv_usec = 0; //单位为：ms

	FD_ZERO(&rfds); //清空集合  
	FD_SET(sockSrv, &rfds); //将fp添加到集合，后面的FD_ISSET和FD_SET没有必然关系，这里是添加检测
	int RetInfo = select(0, &rfds, NULL, NULL, &SelectTimeout); //函数调用成功则返回套接字数量的总和，如果超时则返回0，失败则返回SOCKET_ERROR。
	if(RetInfo <= 0)
	{
		//在等待时间过程中，没有网络数据可读，超时返回。
		return 0;
	}

// 	int nRecvLen = 0;
// 	int Length = 256;
	char recvBuf[TCP_DATA];
	memset(recvBuf, 0, TCP_DATA);
	int aaa=sizeof(PC_CMD_REPLY);
	int RetLen = recv(sockSrv, recvBuf, TCP_DATA, 0);
	int ret = WSAGetLastError();
	if (RetLen == SOCKET_ERROR)
	{
		PRINT("接收SOCKET数据超时！\n");
		return 0;
	}

// 	while (nRecvLen < Length)  //由于网络环境可能一次接收不到一个完整的包，此处做完整性处理
// 	{
// 		int RetLen = recv(sockSrv, recvBuf+nRecvLen, 1024, 0);
// 		if (RetLen == SOCKET_ERROR)
// 		{
// 			PRINT("接收SOCKET数据超时！\n");
// 			return 0;
// 		}
// 		nRecvLen += RetLen;
// 	}

	memcpy(RecvData, recvBuf, sizeof(PC_CMD_REPLY));
	if (RecvData->cmdType != pcCmdType ||
		RecvData->SIG_START != SIG_START_NUM ||
		RecvData->SIG_END != SIG_END_NUM )
	{
		PRINT("接收数据类型不对\n");
		return 0;
	}

	PRINT("接收到的SOCKET数据为：%s\n", recvBuf);
	return 1;
}


static int dispatch_cmd(PC_CMD_REPLY *pcCmdReply, PC_CMD_TYPE pcCmdType, unsigned int TimeOut)  //封装收命令
{
	int Ret;

	if (CommunicationFlag == FALSE)  //用USB口接收数据
	{
		char *VID = "0x1fc9";
		char *PID = "0x107";
		Ret = ReceiveDataFromUsb(VID, PID, TimeOut, pcCmdType, pcCmdReply);
	}

	if (CommunicationFlag == TRUE)  //用网口接收数据
	{
		Ret = RceiveDataFromSocket(TimeOut, pcCmdType, pcCmdReply);
	}

	return Ret;//返回值为1成功，返回值为0失败
}


static int send_cmd(PC_CMD_DATA* pData)  //封装发命令
{
	int Ret;

	if (CommunicationFlag == FALSE)//为0用USB传输数据，为1用网口传输数据
	{
		char *VID = "0x1fc9";
		char *PID = "0x107";
		Ret = SendDataToUsb(VID, PID, pData);
	}

	if (CommunicationFlag == TRUE)//用网口传输数据
	{
		//发送命令前，清空接收缓存区垃圾数据
		PC_CMD_REPLY pcCmdReply;
		memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
		dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_COLLECT, 100); //命令类型随机取
		
		Ret = SendDataToSocket(pData); //返回值为0表示成功，不等于0表示失败
	}

	return Ret;//返回值为0成功，返回值为-1失败
}


RF_STATUS TestSendCmd(PC_CMD_DATA* pData)
{
	int Ret;

	if (CommunicationFlag == FALSE)//为0用USB传输数据，为1用网口传输数据
	{
		char *VID = "0x1fc9";
		char *PID = "0x107";
		Ret = SendDataToUsb(VID, PID, pData);
	}

	if (CommunicationFlag == TRUE)//用网口传输数据
	{
		Ret = SendDataToSocket(pData); //返回值为0表示成功，不等于0表示失败
	}

	return Ret;//返回值为0成功，返回值为-1失败
}

RF_STATUS TestRecvCmd(PC_CMD_REPLY *pcCmdReply, PC_CMD_TYPE pcCmdType, unsigned int TimeOut)
{
	int Ret;

	if (CommunicationFlag == FALSE)  //用USB口接收数据
	{
		char *VID = "0x1fc9";
		char *PID = "0x107";
		Ret = ReceiveDataFromUsb(VID, PID, TimeOut, pcCmdType, pcCmdReply);
	}

	if (CommunicationFlag == TRUE)  //用网口接收数据
	{
		Ret = RceiveDataFromSocket(TimeOut, pcCmdType, pcCmdReply);
	}

	return Ret;//返回值为1成功，返回值为0失败
}

RF_STATUS TestRecvSensitivityCmd(BYTE RandomNum[128], int TimeOut)
{
	if (CommunicationFlag == FALSE)  //用USB口接收数据
	{
		OVERLAPPED  ReadOverLapped;

		memset(&ReadOverLapped, 0, sizeof(OVERLAPPED));
		ReadOverLapped.hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
		
		int bResult = 0;
		DWORD nBytesGet = 0;

		ReadFile(hCom, RandomNum, 64, NULL, &ReadOverLapped);
		if (WaitForSingleObject(ReadOverLapped.hEvent, TimeOut) == WAIT_TIMEOUT) //超时判断
		{
			PRINT("读取USB数据失败\n");
			return 0;
		}

		bResult = GetOverlappedResult(hCom, &ReadOverLapped, &nBytesGet, FALSE);
		if (!bResult && nBytesGet == 0)
		{
			PRINT("接收数据失败！！！\n");
			return 0; //返回0表示失败
		}

		return 1; //返回1表示成功
	}

	if (CommunicationFlag == TRUE)  //用网口接收数据
	{
		int RetLen = recv(sockSrv, (char *)RandomNum, 128, 0);

		if(RetLen == SOCKET_ERROR)
		{
			PRINT("接收SOCKET数据超时！\n");
			return 0;
		}

		return RetLen;
	}

	return 0;
}

/*----------------------------------------API----------------------------------------*/
//封装命令初始化
static void initCmdData(PC_CMD_DATA& pcCmdData,PC_CMD_TYPE cmdType)
{
	memset(&pcCmdData,0,sizeof(PC_CMD_DATA));
	pcCmdData.cmdType = cmdType;
	pcCmdData.sigEnd = SIG_END_NUM;
	pcCmdData.sigStart = SIG_START_NUM;
}

static void delayMs(unsigned int tMs)
{
	LARGE_INTEGER t;
	double dFrequency;
	long long count;			//定时所需要的计数值
	long long cntStart,cntEnd;
	QueryPerformanceCounter(&t);
	cntStart = t.QuadPart;

	QueryPerformanceFrequency(&t);
	dFrequency = (double)t.QuadPart;
	/********************************************
	*
	*    定时时间   T(s)            f*tMs
	*count = --------------- = -----------------
	*   计数器周期 T(1/f)(s)        1000(化单位为s)
	*
	********************************************/
	count = (long long)(dFrequency*tMs/1000.0);
	cntStart += count;
	while (1)
	{
		QueryPerformanceCounter(&t);
		cntEnd = t.QuadPart;
		if (cntEnd > cntStart)
			break;
	}
}

//唤醒
RF_STATUS rfWakeUpTag(IN int wakeupNum)
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	initCmdData(pcCmdData, PC_CMD_TYPE_WAKEUP_TAG);
	pcCmdData.uParam.WAKEUP_TAG_PARAM.wakeupNum = wakeupNum;

	int Info = send_cmd(&pcCmdData);
	if (0 != Info)
	{
		PRINT ("唤醒发送错误！\n");
		return IF_TYPE_WAKEUP_TAG | WAKEUP_TAG_STATE_FAILED;
	}

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_WAKEUP_TAG, 20*TIME_OUT))
	{
		PRINT("唤醒回复超时\n");
		return IF_TYPE_WAKEUP_TAG | WAKEUP_TAG_STATE_TIMEOUT;
	}

	switch(pcCmdReply.uReplyParam.wakeupParam.WakeupState)
	{
	case WAKEUP_TAG_STATE_SUCCESS:
		return IF_TYPE_WAKEUP_TAG | WAKEUP_TAG_STATE_SUCCESS;
	case WAKEUP_TAG_STATE_FAILED:
		return IF_TYPE_WAKEUP_TAG | WAKEUP_TAG_STATE_FAILED;
	default:
		return IF_TYPE_WAKEUP_TAG | WAKEUP_ONE_TAG_STATE_TIMEOUT;
	}
}

/******************************************************
*函数名称：rfCollectTag
*函数功能：盘存，收集所有标签
*函数参数：4个
*返回状态：
*/
RF_STATUS rfCollectTag(	OUT  TAG_INFO*  tagInfoArray,	//标签信息数据指针
						IN	 DWORD		tagNums,		//数组大小
						OUT	 WORD*		collectTagNums,	//收集到的标签个数
						IN	 BYTE		retryTimes,		//设置收集期次数
						IN	 DWORD		readyCode		//就绪口令
						)
{
	if (retryTimes == 0)
	{
		PRINT ("收集次数不能小于等于零");
		return IF_TYPE_COLLECT_TAG | COLLECT_TAG_STATE_SEND_FAILED;
	}
	int Info;
	unsigned char Tid[9];
	unsigned char Uii[33];
	unsigned int tagNumNow = 0;
	*collectTagNums = 0;

	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	initCmdData(pcCmdData, PC_CMD_TYPE_COLLECT);
	pcCmdData.uParam.INVENTORY_TAG_PARAM.readyToken = readyCode;
	pcCmdData.uParam.INVENTORY_TAG_PARAM.retryTimes = retryTimes;

	Info = send_cmd(&pcCmdData);
	if (0 != Info)
	{
		PRINT ("盘存发送错误！\n");
		return IF_TYPE_COLLECT_TAG | COLLECT_TAG_STATE_SEND_FAILED;
	}

	while (1)
	{
		if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_COLLECT, /*20*TIME_OUT*/INFINITE))
		{
			PRINT("盘存标签读阅读器回复超时\n");
			return IF_TYPE_COLLECT_TAG | COLLECT_TAG_STATE_TIMEOUT;
		}
		tagNumNow += pcCmdReply.uReplyParam.collectParam.curNumOfTag;

		for(int i = 0; i < pcCmdReply.uReplyParam.collectParam.curNumOfTag; i++)
		{
			memset(Tid, 0, 9);
			memset(Uii, 0, 33);
			tagInfoArray[(*collectTagNums)++] = pcCmdReply.uReplyParam.collectParam.tagInfo[i];
			memcpy(Tid, tagInfoArray[*collectTagNums - 1].TID, 8);
			memcpy(Uii, tagInfoArray[*collectTagNums - 1].UII, 32);

			PRINT("标签ID:%s\n标签电量:%d\nUII:%s\n", Tid, tagInfoArray[*collectTagNums - 1].tagState, Uii);
			if (*collectTagNums > tagNums)	//防止标签个数超过传入的参数容量
			{
				PRINT("收到标签数超过用户输入的最大值，盘存失败！\n");
				break;
			}
		}
		if (tagNumNow == pcCmdReply.uReplyParam.collectParam.numsOfAllTag)
		{
			PRINT("盘存命令完成！\n");
			PRINT("共收到 %d 个标签，需求 %d 个标签.\n", tagNumNow, pcCmdReply.uReplyParam.collectParam.numsOfAllTag);
			break;
		}else{
			PRINT ("共收到 %d 个标签，需要 %d 个标签.\n", tagNumNow, pcCmdReply.uReplyParam.collectParam.numsOfAllTag);
		}
	}
	if (0 == *collectTagNums)//没盘存到标签
	{
		PRINT ("但是没盘存到标签\n");
		return IF_TYPE_COLLECT_TAG | COLLECT_TAG_STATE_EMPTY;
	}

	return IF_TYPE_COLLECT_TAG | COLLECT_TAG_STATE_SUCCESS;
}

/******************************************************
*函数名称：rfCollectSpecContTag
*函数功能：盘存指定内容标签，收集所有标签
*函数参数：
*返回状态：
*/
RF_STATUS rfCollectSpecContTag( IN	WORD	fileID,			//文件标示符
								IN	BYTE	specCont[70],	//指定内容
								IN	char	datalen,		//指定内容的长度(<80字节)
								OUT TAG_INFO*  tagInfoArray,//标签信息数据指针
								OUT	DWORD*	   collectTagNums,//收集到的标签个数
								IN	BYTE	retryTimes,		  //设置收集期次数
								IN	DWORD	readyCode		  //就绪口令
								)
{
	if (datalen > 70)
	{
		PRINT ("指定内容的长度不能超过70字节");
		return IF_TYPE_COLLECT_SPEC_CONT | COLLECT_TAG_STATE_SEND_FAILED;
	}
	if (retryTimes == 0)
	{
		PRINT ("收集次数不能小于等于零");
		return IF_TYPE_COLLECT_SPEC_CONT | COLLECT_TAG_STATE_SEND_FAILED;
	}

	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	memset(&pcCmdReply,0,sizeof(PC_CMD_REPLY));
	SEARCH_ELEMENT searchElement;
	memset(&searchElement,0,sizeof(SEARCH_ELEMENT));
	unsigned char Tid[9];
	unsigned char Uii[33];
	unsigned int tagNumNow = 0;
	//isSafeSessionOpened = FALSE;
	*collectTagNums = 0;
	initCmdData(pcCmdData,PC_CMD_TYPE_COLLECT_SPEC_CONT);
	pcCmdData.uParam.INVENTORY_SPECCONT_TAG_PARAM.retryTimes = retryTimes;
	pcCmdData.uParam.INVENTORY_SPECCONT_TAG_PARAM.fileID = fileID;
	pcCmdData.uParam.INVENTORY_SPECCONT_TAG_PARAM.readyToken = readyCode;
	pcCmdData.uParam.INVENTORY_SPECCONT_TAG_PARAM.datalen = datalen + 4;
	pcCmdData.uParam.INVENTORY_SPECCONT_TAG_PARAM.searchElement.logicalOperator = LOGICAL_C;
	pcCmdData.uParam.INVENTORY_SPECCONT_TAG_PARAM.searchElement.fieldOffset = 0;
	pcCmdData.uParam.INVENTORY_SPECCONT_TAG_PARAM.searchElement.relationOperator = RELATION_EQAUL;
	pcCmdData.uParam.INVENTORY_SPECCONT_TAG_PARAM.searchElement.dataLen = datalen;
	memcpy(pcCmdData.uParam.INVENTORY_SPECCONT_TAG_PARAM.searchElement.dataBuf,specCont,datalen);
	//memcpy(pcCmdData.uParam.INVENTORY_SPECCONT_TAG_PARAM.specCont,specCont,datalen);

	int Info = send_cmd(&pcCmdData);
	if (0 != Info)
	{
		PRINT ("指定盘存发送错误！\n");
		return IF_TYPE_COLLECT_SPEC_CONT | COLLECT_TAG_STATE_SEND_FAILED;
	}
	Sleep(3000);  //为什么sleep？ppwu 12-28
	while (1)
	{
		if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_COLLECT, 22000))
		{
			PRINT("指定盘存标签读阅读器回复超时\n");
			return IF_TYPE_COLLECT_SPEC_CONT| COLLECT_TAG_STATE_TIMEOUT;
		}
		if (COLLECT_TAG_STATE_EMPTY == pcCmdReply.uReplyParam.collectParam.collectTagState)
		{
			PRINT("检索到没有符合指定内容的标签！");
			return IF_TYPE_COLLECT_SPEC_CONT | COLLECT_TAG_STATE_EMPTY;
		}

		tagNumNow += pcCmdReply.uReplyParam.collectParam.curNumOfTag;

		for(int i = 0;i < pcCmdReply.uReplyParam.collectParam.curNumOfTag; i++)
		{
			memset(Tid,0,9);
			memset(Uii,0,33);
			tagInfoArray[(*collectTagNums)++] = pcCmdReply.uReplyParam.collectParam.tagInfo[i];
			//memcpy(Tid,tagInfoArray[*collectTagNums - 1].TID,8);
			//memcpy(Uii,tagInfoArray[*collectTagNums - 1].UII,32);
			//PRINT("标签ID:%s\n标签电量:%d\nUII:%s\n",Tid,tagInfoArray[*collectTagNums - 1].tagState,Uii);
		}
		if (tagNumNow == pcCmdReply.uReplyParam.collectParam.numsOfAllTag)
		{
			PRINT("指定盘存命令完成！\n");
			break;
		}
	}
	if (0 == *collectTagNums)//没盘存到标签
	{
		PRINT ("但是没盘存到标签\n");
		return IF_TYPE_COLLECT_SPEC_CONT | COLLECT_TAG_STATE_EMPTY;
	}

	return IF_TYPE_COLLECT_SPEC_CONT | COLLECT_TAG_STATE_SUCCESS;
}

/******************************************************
*函数名称：rfWakeupOneTag
*函数功能：唤醒单个标签。
*函数参数：
*返回状态：
*/
RF_STATUS rfWakeupOneTag(IN  const BYTE* TID,
						 IN	 DWORD		 readyCode,		//就绪口令
						 OUT TAG_INFO*   tagInfoArray	//标签信息数据指针
						 )
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	int Info;
	initCmdData(pcCmdData,PC_CMD_TYPE_WAKEUP_ONE_TAG);
	memcpy(pcCmdData.uParam.WAKEUP_ONE_TAG_PARAM.TID,TID,8);
	pcCmdData.uParam.WAKEUP_ONE_TAG_PARAM.readyToken = readyCode;
	Info = send_cmd(&pcCmdData);
	if (0 != Info)
	{
		PRINT ("唤醒单个标签！\n");
		return IF_TYPE_WAKEUP_ONE_TAG | WAKEUP_ONE_TAG_STATE_TIMEOUT;
	}

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_WAKEUP_ONE_TAG, 3*TIME_OUT))
	{
		return IF_TYPE_WAKEUP_ONE_TAG | WAKEUP_ONE_TAG_STATE_TIMEOUT;
	}
	switch(pcCmdReply.uReplyParam.wakeupOneTagParam.wakeupOneTagState)
	{
	case WAKEUP_ONE_TAG_STATE_SUCCESS:
		PRINT ("唤醒单个标签成功\n");
		isTIDselected = TRUE;
		memcpy(tagInfoArray,&pcCmdReply.uReplyParam.wakeupOneTagParam.tagInfo,sizeof(TAG_INFO));
		return IF_TYPE_WAKEUP_ONE_TAG | WAKEUP_ONE_TAG_STATE_SUCCESS;
	case WAKEUP_ONE_TAG_STATE_NOT_FOUND:
		PRINT ("唤醒单个标签未找着\n");
		return IF_TYPE_WAKEUP_ONE_TAG | WAKEUP_ONE_TAG_STATE_NOT_FOUND;
	default:
		return IF_TYPE_WAKEUP_ONE_TAG | WAKEUP_ONE_TAG_STATE_TIMEOUT;
	}
}

/******************************************************
*函数名称：rfSelectTag
*函数功能：选择标签，使标签处于被选中状态。
*函数参数：
*返回状态：
*/
RF_STATUS rfSelectTag(IN  BYTE  getTID[8])//标签TID,8字节
{
	int Info;
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_SELECT_TAG);
	memcpy(pcCmdData.uParam.SELECT_TAG_PRAM.TID,getTID,8);

	Info = send_cmd(&pcCmdData);
	if (0 != Info)
	{
		PRINT ("选择标签发送错误！\n");
		return IF_TYPE_SELECT_TAG | SELECT_TAG_STATE_SEND_FAILED;
	}
	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_SELECT_TAG, 3*TIME_OUT))
	{
		PRINT ("选择标签超时\n");
		return IF_TYPE_SELECT_TAG | SELECT_TAG_STATE_TIMEOUT;
	}

	switch (pcCmdReply.uReplyParam.selectTagParam.selectTagState)
	{
	case SELECT_TAG_STATE_SUCCESS:
		memcpy(TID,getTID,8);
		isTIDselected = TRUE;	//全局变量
		PRINT("选择标签成功\n");
		return IF_TYPE_SELECT_TAG | SELECT_TAG_STATE_SUCCESS;
	case SELECT_TAG_STATE_NOT_EXISTING:
		PRINT("选择标签不存在\n");
		return IF_TYPE_SELECT_TAG | SELECT_TAG_STATE_NOT_EXISTING;
	case SELECT_TAG_STATE_ALREADY_SELECTED:
		PRINT("标签已经被选中\n");
		return IF_TYPE_SELECT_TAG | SELECT_TAG_STATE_ALREADY_SELECTED;
	default:
		PRINT("选择标签未知错误~~~~~~selectTagState为%d\n",pcCmdReply.uReplyParam.selectTagParam.selectTagState);
		return IF_TYPE_SELECT_TAG | pcCmdReply.uReplyParam.selectTagParam.selectTagState;
	}
}

/******************************************************
*函数名称：rfOpenMonitorSensor
*函数功能：打开/关闭蜂鸣器。
*函数参数：
*返回状态：
*/
RF_STATUS rfOpenMonitorSensor(IN 	WORD 	FileId,		//文件ID
							  IN 	BYTE 	SensorId,	//传感器ID
							  IN 	BYTE 	SensorState	//传感器状态
							  )
{
	int Info;
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_MONITOR_SENSOR_EN);
	pcCmdData.uParam.MONITOR_SENSOREN_PARAM.FileId = FileId;
	pcCmdData.uParam.MONITOR_SENSOREN_PARAM.SensorId = SensorId;
	if (SensorState)
	{
		SensorState = 0;
		pcCmdData.uParam.MONITOR_SENSOREN_PARAM.SensorState = 0x00;
	}else{
		SensorState = 1;
		pcCmdData.uParam.MONITOR_SENSOREN_PARAM.SensorState = 0x01;
	}
	Info = send_cmd(&pcCmdData);

	if (0 == dispatch_cmd(&pcCmdReply,PC_CMD_TYPE_MONITOR_SENSOR_EN,3*TIME_OUT))
	{
		PRINT ("打开蜂鸣器超时\n");
		return IF_TYPE_OPEN_SENSOR | MONITOR_SENSOREN_STATE_TIMEOUT;
	}

	switch (pcCmdReply.uReplyParam.monitorSensorEnParam.sensorEnState)
	{
	case MONITOR_SENSOREN_STATE_SUCCESS:
		PRINT ("打开/关闭蜂鸣器成功\n");
		return IF_TYPE_OPEN_SENSOR | pcCmdReply.uReplyParam.monitorSensorEnParam.sensorEnState;
	case MONITOR_SENSOREN_STATE_UNKNOWN:
		PRINT ("打开/关闭蜂鸣器未知\n");
		return IF_TYPE_OPEN_SENSOR | pcCmdReply.uReplyParam.monitorSensorEnParam.sensorEnState;
	case MONITOR_SENSOREN_STATE_SENSOR_IDMISMATCH:
		PRINT ("打开/关闭蜂鸣器ID丢失匹配\n");
		return IF_TYPE_OPEN_SENSOR | pcCmdReply.uReplyParam.monitorSensorEnParam.sensorEnState;
	case MONITOR_SENSOREN_STATE_TIMEOUT:
		PRINT ("打开/关闭蜂鸣器超时\n");
		return IF_TYPE_OPEN_SENSOR | pcCmdReply.uReplyParam.monitorSensorEnParam.sensorEnState;
	case MONITOR_SENSOREN_STATE_FAILED:
		PRINT ("打开/关闭蜂鸣器失败\n");
		return IF_TYPE_OPEN_SENSOR | pcCmdReply.uReplyParam.monitorSensorEnParam.sensorEnState;
	default:
		return IF_TYPE_OPEN_SENSOR | pcCmdReply.uReplyParam.monitorSensorEnParam.sensorEnState;
	}
}

/******************************************************
*函数名称：rfSleepAllTag
*函数功能：当使用完标签后，休眠所有标签。
*函数参数：
*返回状态：
*/
RF_STATUS rfSleepAllTag(IN 	BYTE 	sleepTime,		//睡眠时间(单位:1s)
						IN	DWORD 	sleepCode,		//睡眠口令
						IN 	BYTE 	listenTime		//监听时间（保留值：11）
						)
{
	int Info;
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_SLEEP_ALL_TAG);
	pcCmdData.uParam.SLEEP_ALL_TAG_PARAM.sleepCode = sleepCode;
	pcCmdData.uParam.SLEEP_ALL_TAG_PARAM.sleepTime = sleepTime;
	pcCmdData.uParam.SLEEP_ALL_TAG_PARAM.listenTime = listenTime;
	Info = send_cmd(&pcCmdData);
	if (Info != 0)
	{
		PRINT("休眠所有标签命令发送失败\n");
		return IF_TYPE_SLEEP_ALL_TAG | SLEEP_TAG_FAILED;
	}

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_SLEEP_ALL_TAG, 3*TIME_OUT))
	{
		PRINT ("休眠所有标签超时\n");
		return IF_TYPE_SLEEP_ALL_TAG | SLEEP_TAG_TIMEOUT;
	}
	PRINT ("sleep All tag success.sleepTime:0x%x sleepCode:0x%x listenTime:0x%x\n",sleepTime,sleepCode,listenTime);

	return IF_TYPE_SLEEP_ALL_TAG | SLEEP_TAG_SUCCESS;
}

/******************************************************
*函数名称：rfSleepExOnesTag
*函数功能：休眠除某一类标签外所有标签
*函数参数：
*返回状态：
*/
RFIDLIBDLL RF_STATUS rfSleepExOnesTag(IN BYTE  sleepTime,		//睡眠时间
									  IN DWORD sleepCode,		//睡眠口令
									  IN BYTE  listenTime,		//监听时间（保留值：11）
									  IN WORD  fileID,			//文件标识符
									  IN BYTE  offset,			//偏移
									  IN BYTE  dataLen,		//数据长度
									  IN BYTE  dataBuffer[]	//数据内容（不超过80字节）
									  )
{
	if (dataLen > 80)
	{
		PRINT("休眠除某一类标签外所有标签失败，dataLen不能大于80\n");
		return IF_TYPE_SLEEP_EX_ONES_TAG | SLEEP_EX_ONES_TAG_STATE_FAILED;
	}
	int Info;
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_SLEEP_EX_ONES_TAG);
	pcCmdData.uParam.SLEEP_EX_ONES_TAG_PARAM.sleepCode = sleepCode;
	pcCmdData.uParam.SLEEP_EX_ONES_TAG_PARAM.sleepTime = sleepTime;
	pcCmdData.uParam.SLEEP_EX_ONES_TAG_PARAM.listenTime = listenTime;
	pcCmdData.uParam.SLEEP_EX_ONES_TAG_PARAM.fileID = fileID;
	pcCmdData.uParam.SLEEP_EX_ONES_TAG_PARAM.offset = offset;
	pcCmdData.uParam.SLEEP_EX_ONES_TAG_PARAM.dataLen = dataLen;
	memcpy(pcCmdData.uParam.SLEEP_EX_ONES_TAG_PARAM.dataBuffer,dataBuffer,dataLen);

	Info = send_cmd(&pcCmdData);
	if (Info != 0)
	{
		PRINT("发送休眠除某一类标签外所有标签命令错误\n");
		return IF_TYPE_SLEEP_EX_ONES_TAG | SLEEP_EX_ONES_TAG_STATE_FAILED;
	}

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_SLEEP_EX_ONES_TAG, 3*TIME_OUT))
	{
		PRINT ("休眠除某一类标签外所有标签超时\n");
		return IF_TYPE_SLEEP_EX_ONES_TAG | SLEEP_EX_ONES_TAG_STATE_TIMEOUT;
	}
	PRINT ("休眠除某一类标签外所有标签成功\n");
	return IF_TYPE_SLEEP_EX_ONES_TAG | SLEEP_EX_ONES_TAG_STATE_SUCCESS;
}

/******************************************************
*函数名称：rfSleepExOneTag
*函数功能：休眠除某个标签外所有标签
*函数参数：
*返回状态：
*/
RFIDLIBDLL RF_STATUS rfSleepExOneTag( IN	BYTE	TID[8],
									  IN 	BYTE 	sleepTime,		//睡眠时间
									  IN	DWORD 	sleepCode,		//睡眠口令
									  IN 	BYTE 	listenTime		//监听时间（保留值：11）
									  )
{
	int Info;
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_SLEEP_EX_ONE_TAG);
	memcpy(pcCmdData.uParam.SLEEP_EX_ONE_TAG_PARAM.TID,TID,8);
	pcCmdData.uParam.SLEEP_EX_ONE_TAG_PARAM.sleepCode = sleepCode;
	pcCmdData.uParam.SLEEP_EX_ONE_TAG_PARAM.sleepTime = sleepTime;
	pcCmdData.uParam.SLEEP_EX_ONE_TAG_PARAM.listenTime = listenTime;
	Info = send_cmd(&pcCmdData);
	if (Info != 0)
	{
		PRINT("发送休眠某个标签以外命令错误\n");
		return IF_TYPE_SLEEP_EX_ONE_TAG | SLEEP_EX_ONE_TAG_STATE_FAILED;
	}

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_SLEEP_EX_ONE_TAG, 3*TIME_OUT))
	{
		PRINT ("休眠除某个标签外所有标签超时\n");
		return IF_TYPE_SLEEP_EX_ONE_TAG | SLEEP_EX_ONE_TAG_STATE_TIMEOUT;
	}
	PRINT ("休眠除某个标签外所有标签成功\n");
	return IF_TYPE_SLEEP_EX_ONE_TAG | SLEEP_EX_ONE_TAG_STATE_SUCCESS;
}

/******************************************************
*函数名称：rfListFile
*函数功能：列表文件，以列表的形式显示一个文件
*函数参数：
*返回状态：
*/
RF_STATUS rfListFile( IN	BYTE 		listMode,		//列表模式:02:当前选中文件前一个文件,03:当前选择文件后一个文件,04:指定偏移量位置文件
					  IN	BYTE 		dfRecPointer,	//04模式时,指定的文件偏移量
					  IN 	DWORD 		fileNums1,		//输入缓冲区fileAttr_R的大小,单位为FILE_ATTRIBUTE_R大小
					  OUT 	FILE_ATTRIBUTE_R* 	fileAttr_R,	//文件属性缓冲区指针
					  OUT 	DWORD* 		fileNums2		//返回实际列表文件时文件个数
					 )
{
	if ((selectedFile.securityType & TOKEN_LIST_MASK) == TOKEN_LIST_STATE_PWD)  //需要密码
	{
		if (false == pwdState.isListPwdPassed)
		{
			PRINT ("列表文件，请输入密码并保证密码正确\n");
			return IF_TYPE_LIST_FILE | LIST_FILE_STATE_NEED_PWD_VERIFY;
		}
	}
	if ((selectedFile.securityType & TOKEN_LIST_MASK) == TOKEN_LIST_STATE_ATH)  //需要校验
	{
		if ((rfSafeSession() != (IF_TYPE_SAFE_SESSION | SAFE_SESSION_STATE_SUCCESS)) &&
			(rfSafeSession() != (IF_TYPE_SAFE_SESSION | SAFE_SESSION_STATE_ALREADY_IN)))
		{
			PRINT ("列表文件，安全会话不正确，请先进入安全会话\n");
			return IF_TYPE_LIST_FILE | LIST_FILE_STATE_NEED_ATH_VERIFY;
		}
	}
	if ((selectedFile.securityType & TOKEN_LIST_MASK) == TOKEN_LIST_STATE_NEV)
	{
		PRINT("该目录文件list权限为NEV，不能列表\n");
		return IF_TYPE_LIST_FILE | LIST_FILE_STATE_FAILED;
	}

	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	
	*fileNums2 = 0;
	int cnt = 0;	//循环cnt(5)次列表文件失败则退出并返回
	initCmdData(pcCmdData,PC_CMD_TYPE_LIST_FILE);
	pcCmdData.uParam.LIST_FILE_PARAM.dfRecPointer = 0;//dfRecPointer;
	pcCmdData.uParam.LIST_FILE_PARAM.lsitMode = listMode;
	pcCmdData.uParam.LIST_FILE_PARAM.opMode = DERECTORY_LIST_MODE_CURRENT;
	int Info = send_cmd(&pcCmdData);

	do
	{
		if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_LIST_FILE, 15*TIME_OUT))
		{
			PRINT ("列表文件超时\n");
			return IF_TYPE_LIST_FILE | LIST_FILE_STATE_TIMEOUT;
		}
		if (pcCmdReply.uReplyParam.listFileParam.listState == LIST_FILE_STATE_NO_TID)
		{
			printf("列表前请选择标签\n");
			return IF_TYPE_LIST_FILE | LIST_FILE_STATE_NO_TID;
		}
		
		if (pcCmdReply.uReplyParam.listFileParam.listState == LIST_FILE_STATE_FILE_ALREDAY_LAST_FILE)
		{
			
			PRINT ("列表文件成功\n");
			listFileNums++;
			return IF_TYPE_LIST_FILE | LIST_FILE_STATE_FILE_ALREDAY_LAST_FILE;
		}
		switch (pcCmdReply.uReplyParam.listFileParam.listState)
		{
		case LIST_FILE_STATE_SUCCESS:
			fileAttr_R[(*fileNums2)] = pcCmdReply.uReplyParam.listFileParam.fileAttr_R;
			listFileID[(*fileNums2)] = pcCmdReply.uReplyParam.listFileParam.fileAttr_R.fileID;
			(*fileNums2)++;
			listFileNums++;
			break;
		case LIST_FILE_STATE_NEED_PWD_VERIFY:
			PRINT ("列表文件需密码验证\n");
			return IF_TYPE_LIST_FILE | LIST_FILE_STATE_NEED_PWD_VERIFY;
		case LIST_FILE_STATE_NEED_ATH_VERIFY:
			PRINT ("列表文件需实体验证\n");
			return IF_TYPE_LIST_FILE | LIST_FILE_STATE_NEED_ATH_VERIFY;
		case LSIT_FILE_STATE_FILE_NOT_EXISTING:
			PRINT ("列表文件不存在\n");
			return IF_TYPE_LIST_FILE | LSIT_FILE_STATE_FILE_NOT_EXISTING;
		case LIST_FILE_STATE_FAILED:
			PRINT ("列表文件失败\n");
			cnt++;
			if (cnt >= 5)
			{
				return IF_TYPE_LIST_FILE | LIST_FILE_STATE_FAILED;
			}
		case LIST_FILE_STATE_TIMEOUT:
			PRINT("列表文件超时\n");
			return IF_TYPE_LIST_FILE | LIST_FILE_STATE_TIMEOUT;
		default:
			return IF_TYPE_LIST_FILE | LIST_FILE_STATE_FAILED;
		}

		if (fileNums1 < *fileNums2)
		{
			PRINT ("列表文件缓冲区不够\n");
			return IF_TYPE_LIST_FILE | LIST_FILE_STATE_BUFF_NOT_ENOUGH;
		}
		initCmdData(pcCmdData,PC_CMD_TYPE_LIST_FILE);
		pcCmdData.uParam.LIST_FILE_PARAM.dfRecPointer = 0;//dfRecPointer;
		pcCmdData.uParam.LIST_FILE_PARAM.lsitMode = listMode;
		pcCmdData.uParam.LIST_FILE_PARAM.opMode = DERECTORY_LSIT_MODE_NEXT;
		Info = send_cmd(&pcCmdData);
	}while(1);
}


/******************************************************
*函数名称：rfListFileNew
*函数功能：新列表文件
*函数参数：
*返回状态：
*备注：by ppwu 12-27
*/
RF_STATUS rfListFileNew(IN  BYTE  listMode,		//列表模式:02:当前选中文件前一个文件,03:当前选择文件后一个文件,04:指定偏移量位置文件
						IN	BYTE  dfRecPointer,	//04模式时,指定的文件偏移量
						IN  BYTE  opMode,
						OUT FILE_ATTRIBUTE_R*  fileAttr_R//文件属性缓冲区指针
						)
{
	if ((selectedFile.securityType & TOKEN_LIST_MASK) == TOKEN_LIST_STATE_PWD)  //需要密码
	{
		if (false == pwdState.isListPwdPassed)
		{
			PRINT ("列表文件，请输入密码并保证密码正确\n");
			return IF_TYPE_LIST_FILE | LIST_FILE_STATE_NEED_PWD_VERIFY;
		}
	}
	if ((selectedFile.securityType & TOKEN_LIST_MASK) == TOKEN_LIST_STATE_ATH)  //需要校验
	{
		if ((rfSafeSession() != (IF_TYPE_SAFE_SESSION | SAFE_SESSION_STATE_SUCCESS)) &&
			(rfSafeSession() != (IF_TYPE_SAFE_SESSION | SAFE_SESSION_STATE_ALREADY_IN)))
		{
			PRINT ("列表文件，安全会话不正确，请先进入安全会话\n");
			return IF_TYPE_LIST_FILE | LIST_FILE_STATE_NEED_ATH_VERIFY;
		}
	}
	if ((selectedFile.securityType & TOKEN_LIST_MASK) == TOKEN_LIST_STATE_NEV)
	{
		PRINT("该目录文件list权限为NEV，不能列表\n");
		return IF_TYPE_LIST_FILE | LIST_FILE_STATE_FAILED;
	}

	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;

	initCmdData(pcCmdData,PC_CMD_TYPE_LIST_FILE);
	pcCmdData.uParam.LIST_FILE_PARAM.dfRecPointer = 0;//dfRecPointer;
	pcCmdData.uParam.LIST_FILE_PARAM.lsitMode = listMode;
	pcCmdData.uParam.LIST_FILE_PARAM.opMode = opMode;

	int Info = send_cmd(&pcCmdData);
	if(Info == -1)
	{
		//失败
		return IF_TYPE_LIST_FILE | LIST_FILE_STATE_FAILED;
	}

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_LIST_FILE, INFINITE/*3*TIME_OUT*/))
	{
		PRINT ("列表文件超时\n");
		return IF_TYPE_LIST_FILE | LIST_FILE_STATE_TIMEOUT;
	}
	if (pcCmdReply.uReplyParam.listFileParam.listState == LIST_FILE_STATE_NO_TID)
	{
		printf("列表前请选择标签\n");
		return IF_TYPE_LIST_FILE | LIST_FILE_STATE_NO_TID;
	}

	if (pcCmdReply.uReplyParam.listFileParam.listState == LIST_FILE_STATE_FILE_ALREDAY_LAST_FILE)
	{

		PRINT ("列表文件成功\n");
		listFileNums++;
		return IF_TYPE_LIST_FILE | LIST_FILE_STATE_FILE_ALREDAY_LAST_FILE;
	}
	switch (pcCmdReply.uReplyParam.listFileParam.listState)
	{
	case LIST_FILE_STATE_SUCCESS:
		memcpy(fileAttr_R, &pcCmdReply.uReplyParam.listFileParam.fileAttr_R, sizeof(FILE_ATTRIBUTE_R));
		listFileID[listFileNums] = pcCmdReply.uReplyParam.listFileParam.fileAttr_R.fileID;
		listFileNums++;
		return IF_TYPE_LIST_FILE | LIST_FILE_STATE_SUCCESS;
	case LIST_FILE_STATE_NEED_PWD_VERIFY:
		PRINT ("列表文件需密码验证\n");
		return IF_TYPE_LIST_FILE | LIST_FILE_STATE_NEED_PWD_VERIFY;
	case LIST_FILE_STATE_NEED_ATH_VERIFY:
		PRINT ("列表文件需实体验证\n");
		return IF_TYPE_LIST_FILE | LIST_FILE_STATE_NEED_ATH_VERIFY;
	case LSIT_FILE_STATE_FILE_NOT_EXISTING:
		PRINT ("列表文件不存在\n");
		return IF_TYPE_LIST_FILE | LSIT_FILE_STATE_FILE_NOT_EXISTING;
	case LIST_FILE_STATE_FAILED:
		PRINT ("列表文件失败\n");
		return IF_TYPE_LIST_FILE | LIST_FILE_STATE_FAILED;
	case LIST_FILE_STATE_TIMEOUT:
		PRINT("列表文件超时\n");
		return IF_TYPE_LIST_FILE | LIST_FILE_STATE_TIMEOUT;
	default:
		return IF_TYPE_LIST_FILE | LIST_FILE_STATE_FAILED;
	}
}

/******************************************************
*函数名称：rfCreateFile
*函数功能：创建文件。如果创建大于64K的文件，要保证一下两个条件：
*				1、大文件ID必须是4的倍数；
*				2、ID + 1、ID+ 2、ID + 3这几个文件ID必须保留给大文件使用，下次创建时候注意不要用这些ID
*函数参数：
*返回状态：
*/
RF_STATUS rfCreateFile(IN  FILE_ATTRIBUTE_W*  fileAttr_W)	//文件属性:fileAttr_W指针
{
	selectedFile.fileID = NULL;
	if (fileAttr_W == NULL)
	{
		PRINT("输入的指针为空\n");
		return IF_TYPE_CREATE_FILE | CREATE_FILE_STATE_IN_NULL;
	}
	if (fileAttr_W->fileAttr_R.fileID < 0x3f00)
	{
		PRINT("创建的文件ID不能小于0x3f00\n");
		return IF_TYPE_CREATE_FILE | CREATE_FILE_ATTRIBUTE_ERROR;
	}
	if ((fileAttr_W->fileAttr_R.fileTypeValid & FILE_TYPE_MASK) == FILE_TYPE_DF)
	{
		if ((fileAttr_W->fileAttr_R.fileTypeValid & FILE_INVALID_MASK) == FILE_TYPE_INVALID)
		{
			PRINT("创建的目录文件类型为无效目录\n");
			return IF_TYPE_CREATE_FILE | CREATE_FILE_INVALID;
		}
		if (fileAttr_W->fileAttr_R.uFileAttr.MFDF.numsOfMaxFile > 65532)
		{
			PRINT("创建的目录文件最大文件数不能超过65532");
			return IF_TYPE_CREATE_FILE | CREATE_FILE_SPACE_NOT_ENOUPH;
		}
		fileAttr_W->fileAttr_R.uFileAttr.MFDF.numsOfRealFile = 0;
		//fileAttr_W->fileAttr_R.uFileAttr.MFDF.numsOfMaxFile += 3;

	}
	if (((fileAttr_W->fileAttr_R.fileTypeValid & FILE_TYPE_MASK) == FILE_TYPE_TRANS) &&
		(fileAttr_W->fileAttr_R.uFileAttr.transBinEF.realFileLen > 0))
	{
		PRINT("创建的透明文件实际长度大于0，创建透明文件失败\n");
		return IF_TYPE_CREATE_FILE | CREATE_FILE_STATE_FAILED;
	}
	if (((fileAttr_W->fileAttr_R.fileTypeValid & FILE_TYPE_MASK) == FILE_TYPE_RECORD) &&
		(fileAttr_W->fileAttr_R.uFileAttr.recEF.maxRecordNums < fileAttr_W->fileAttr_R.uFileAttr.recEF.realRecordNums))
	{
		PRINT("创建的记录文件实际记录数大于最大记录数，创建记录文件失败\n");
		return IF_TYPE_CREATE_FILE | CREATE_FILE_STATE_FAILED;
	}
	for (unsigned int i=0; i < listFileNums; i++)
	{
		if (fileAttr_W->fileAttr_R.fileID == listFileID[i])
		{
			PRINT ("该文件已存在，不能再次创建\n");
			//return IF_TYPE_CREATE_FILE | CREATE_FILE_STATE_EXISTING;
		}
	}
	int Info;
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_CREATE_FILE);

	//PRINT("the len of file to create is:%d\n",fileAttr_W->fileAttr_R.uFileAttr.transBinEF.maxFileLen);
	pcCmdData.uParam.CREATE_FILE_PARAM.fileAttr.fileAttr_R.fileID = fileAttr_W->fileAttr_R.fileID;
	pcCmdData.uParam.CREATE_FILE_PARAM.fileAttr.fileAttr_R.fileTypeValid = fileAttr_W->fileAttr_R.fileTypeValid;
	pcCmdData.uParam.CREATE_FILE_PARAM.fileAttr.fileAttr_R.uFileAttr = fileAttr_W->fileAttr_R.uFileAttr;
	pcCmdData.uParam.CREATE_FILE_PARAM.fileAttr.fileAttr_R.securityType = fileAttr_W->fileAttr_R.securityType;
	pcCmdData.uParam.CREATE_FILE_PARAM.fileAttr.access_tokens = fileAttr_W->access_tokens;
	Info = send_cmd(&pcCmdData);
	if (Info != 0)
	{
		PRINT("创建文件发送失败\n");
		return IF_TYPE_CREATE_FILE | CREATE_FILE_STATE_FAILED;
	}

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_CREATE_FILE, 3*TIME_OUT))	//超时
	{
		return IF_TYPE_CREATE_FILE | CREATE_FILE_STATE_FAILED;
	}

	switch (pcCmdReply.uReplyParam.createFileParam.createFileState)
	{
	case CREATE_FILE_STATE_SUCCESS:
		PRINT ("创建文件成功,ID = %d\n",fileAttr_W->fileAttr_R.fileID);

		return IF_TYPE_CREATE_FILE | CREATE_FILE_STATE_SUCCESS;
	case CREATE_FILE_STATE_FAILED:
		PRINT ("创建文件失败\n");
		return IF_TYPE_CREATE_FILE | CREATE_FILE_STATE_FAILED;
	case CREATE_FILE_STATE_TIMEOUT:
		PRINT ("创建文件超时\n");
		return IF_TYPE_CREATE_FILE | CREATE_FILE_STATE_TIMEOUT;
	case CREATE_FILE_STATE_EXISTING:	//文件已经存在
		PRINT ("文件已存在\n");
		return IF_TYPE_CREATE_FILE | CREATE_FILE_STATE_EXISTING;
	default:
		return IF_TYPE_CREATE_FILE | pcCmdReply.uReplyParam.createFileParam.createFileState;
	}
}

/******************************************************
*函数名称：rfSelectFile
*函数功能：选择文件。
*函数参数：
*返回状态：
*/
RF_STATUS rfSelectFile(IN  WORD  fileID,
					   OUT FILE_ATTRIBUTE_R*  fileAttr_R	//返回选择的文件属性
					   )
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcRepData;
	int Info;

	initCmdData(pcCmdData,PC_CMD_TYPE_SELECT_FILE);
	pcCmdData.uParam.SELECT_FILE_PARAM.fileID = fileID;
	//memcpy(&pcCmdData.uParam.SELECT_FILE_PARAM.TID,TID,TID_LEN);

	Info = send_cmd(&pcCmdData);
	memset((void*)&pcRepData, 0, sizeof(PC_CMD_REPLY));
	if (0 == dispatch_cmd(&pcRepData, PC_CMD_TYPE_SELECT_FILE, 3*TIME_OUT))//超时
	{
		return IF_TYPE_SELECT_FILE | SELECT_FILE_STATE_TIMEOUT;
	}


	switch (pcRepData.uReplyParam.selectFileParam.selectFileState)
	{
	case SELECT_FILE_STATE_SUCCESS:
		PRINT ("选择文件成功,file ID = %x\n",pcRepData.uReplyParam.selectFileParam.fileAttr_R.fileID);
		PRINT("security type:%x\n",pcRepData.uReplyParam.selectFileParam.fileAttr_R.securityType);
		*fileAttr_R = pcRepData.uReplyParam.selectFileParam.fileAttr_R;
		selectedFile = pcRepData.uReplyParam.selectFileParam.fileAttr_R;
		return IF_TYPE_SELECT_FILE | SELECT_FILE_STATE_SUCCESS;
	case SELECT_FILE_STATE_NEED_PWD_VERIFY:
		PRINT ("选择文件需要密码验证\n");
		return IF_TYPE_SELECT_FILE | SELECT_FILE_STATE_NEED_PWD_VERIFY;
	case SELECT_FILE_STATE_NEED_ATH_VERIFY:
		PRINT ("选择文件需要实体验证\n");
		return IF_TYPE_SELECT_FILE | SELECT_FILE_STATE_NEED_ATH_VERIFY;
	case SELECT_FILE_STATE_NOT_EXISTING:
		PRINT ("所选文件不存在\n");
		return IF_TYPE_SELECT_FILE | SELECT_FILE_STATE_NOT_EXISTING;
	case SELECT_FILE_STATE_DENIED:
		PRINT ("选择文件,访问被拒绝,该文件不开放选择权限\n");
		return IF_TYPE_SELECT_FILE | SELECT_FILE_STATE_DENIED;
	case SELECT_FILE_STATE_TIMEOUT:
		PRINT("选择文件文件超时\n");
		return IF_TYPE_SELECT_FILE | SELECT_FILE_STATE_TIMEOUT;
	default:
		return IF_TYPE_SELECT_FILE | pcRepData.uReplyParam.selectFileParam.selectFileState;
	}
}

/******************************************************
*函数名称：rfReadRecFile
*函数功能：读记录文件。
*函数参数：
*返回状态：
*/
RF_STATUS rfReadRecFile(IN 	BYTE 	recPointer,			//记录偏移量
						IN 	BYTE 	opMode,				//02:当前记录文件内下一条记录,03:当前选中文件中的上一条记录,04:记录偏移由recOffset指定
						IN 	BYTE 	recOffset,			//记录内偏移
						IN 	BYTE 	dataLen,			//读取长度
						OUT WORD* 	retLen,				//返回读到的数据长度
						OUT BYTE 	buff[]				//读文件返回缓冲区
						)
{
	if (selectedFile.fileID == NULL)	//没选文件
	{
		PRINT ("读记录文件，没选文件\n");
		return IF_TYPE_READ_RECORD_FILE | READ_FILE_STATE_FAILED;
	}

	printf ("State:%d\n",selectedFile.securityType & TOKEN_RDSEARCH_MASK);

	if ((selectedFile.securityType & TOKEN_RDSEARCH_MASK) == TOKEN_RDSEARCH_STATE_PWD)  //需要密码
	{
		if (false == pwdState.isReadPwdPassed)
		{
			PRINT ("读记录文件，请输入密码并保证密码正确\n");
			return IF_TYPE_READ_RECORD_FILE | READ_FILE_STATE_NEED_PWD_VERIFY;
		}
	}

	if ((selectedFile.securityType & TOKEN_RDSEARCH_MASK) == TOKEN_RDSEARCH_STATE_NEV)  //权限为NEV
	{
		PRINT ("读记录文件，权限为NEV，访问被拒绝\n");
		return IF_TYPE_READ_RECORD_FILE | READ_FILE_STATE_NEED_NEV_VERIFY;
	}

	*retLen = 0;
	BOOL flag = TRUE;
	while(flag)
	{
		PC_CMD_DATA pcCmdData;
		initCmdData(pcCmdData, PC_CMD_TYPE_READ_FILE);
		pcCmdData.uParam.READ_FILE_PARAM.fileType = FILE_TYPE_RECORD;
		pcCmdData.uParam.READ_FILE_PARAM.uReadPram.recEF.opMode = opMode;
		pcCmdData.uParam.READ_FILE_PARAM.uReadPram.recEF.recOffset = recOffset + *retLen;
		pcCmdData.uParam.READ_FILE_PARAM.uReadPram.recEF.recPointer = recPointer;
		if((selectedFile.uFileAttr.recEF.recordLen - *retLen) > 108)
			pcCmdData.uParam.READ_FILE_PARAM.uReadPram.recEF.datLen = 108;
		else
			pcCmdData.uParam.READ_FILE_PARAM.uReadPram.recEF.datLen = selectedFile.uFileAttr.recEF.recordLen - *retLen;
		int ret = send_cmd(&pcCmdData);

		PC_CMD_REPLY pcRepData;
		memset(&pcRepData, 0, sizeof(PC_CMD_REPLY));
		if (0 == dispatch_cmd(&pcRepData, PC_CMD_TYPE_READ_FILE, 5*TIME_OUT))
		{
			PRINT ("读记录文件 超时\n");
			return IF_TYPE_READ_RECORD_FILE | READ_FILE_STATE_FAILED;
		}
		switch (pcRepData.uReplyParam.readFileParam.readFileState)
		{
		case READ_FILE_STATE_SUCCESS: //未完，待续...
			{
				PRINT ("读记录文件成功\n");
				memcpy (buff + *retLen, pcRepData.uReplyParam.readFileParam.buff,
					pcRepData.uReplyParam.readFileParam.retLen);
				*retLen += pcRepData.uReplyParam.readFileParam.retLen;
				if(selectedFile.uFileAttr.recEF.recordLen == *retLen)
					flag = FALSE;
			}
			break;
		case READ_FILE_STATE_NOT_EXISTING:
			PRINT ("欲读取该记录不存在\n");
			return IF_TYPE_READ_RECORD_FILE | pcRepData.uReplyParam.readFileParam.readFileState;
		case READ_FILE_STATE_FAILED:
			PRINT ("读记录文件失败\n");
			return IF_TYPE_READ_RECORD_FILE | pcRepData.uReplyParam.readFileParam.readFileState;
		case READ_FILE_STATE_LAST_RECORD:
			PRINT ("读记录文件已到最后一位\n");
			return IF_TYPE_READ_RECORD_FILE | pcRepData.uReplyParam.readFileParam.readFileState;
		case READ_FILE_STATE_FINISH:
			PRINT ("读记录文件完成\n");
			return IF_TYPE_READ_RECORD_FILE | pcRepData.uReplyParam.readFileParam.readFileState;
		case READ_FILE_STATE_NEED_PWD_VERIFY:
			PRINT ("读记录文件需先输入密码\n");
			return IF_TYPE_READ_RECORD_FILE	| pcRepData.uReplyParam.readFileParam.readFileState;
		case READ_FILE_STATE_NEED_ATH_VERIFY:
			PRINT ("读记录文件需先进入安全会话\n");
			return IF_TYPE_READ_RECORD_FILE	| pcRepData.uReplyParam.readFileParam.readFileState;
		case READ_FILE_STATE_NEED_NEV_VERIFY:
			PRINT ("糟糕，该记录文件属性为NEV，不允许读\n");
			return IF_TYPE_READ_RECORD_FILE	| pcRepData.uReplyParam.readFileParam.readFileState;
		default:
			PRINT ("未知\n");
			return IF_TYPE_READ_RECORD_FILE | pcRepData.uReplyParam.readFileParam.readFileState;
		}
	}
	return IF_TYPE_READ_RECORD_FILE | READ_FILE_STATE_SUCCESS;
}

/******************************************************
*函数名称：rfReadTransFile
*函数功能：读透明文件
*函数参数：
*返回状态：
*备注:改版过 by 2013-12-27
*/
RF_STATUS rfReadTransFile(IN  WORD 	fileOffset,	//文件偏移量
						  IN  WORD  NumToRead,  //从偏移量fileoffset开始读NumToRead个字节，若为0，默认读到文件尾
						  OUT WORD* RetNumLen,	//返回读到的数据长度
						  OUT BYTE 	buff[]		//读文件返回缓冲区
						 )
{
	if (selectedFile.fileID == NULL)	//没选文件
	{
		PRINT ("读透明文件，没选文件\n",selectedFile.fileID);
		return IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_FAILED;
	}

	if ((selectedFile.securityType & TOKEN_RDSEARCH_MASK) == TOKEN_RDSEARCH_STATE_PWD)  //需要密码
	{
		if (false == pwdState.isReadPwdPassed)
		{
			PRINT ("读透明文件，请输入密码并保证密码正确\n");
			return IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_NEED_PWD_VERIFY;
		}
	}

	if ((selectedFile.securityType & TOKEN_RDSEARCH_MASK) == TOKEN_RDSEARCH_STATE_NEV)  //权限为NEV
	{
		PRINT ("读透明文件，权限为NEV，访问被拒绝\n");
		return IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_NEED_NEV_VERIFY;
	}

	//超过8192字节长度，重新设置读取长度
	if (fileOffset > 8192)
	{
		return IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_FAILED;
	}

	BOOL flag = TRUE;
	*RetNumLen = 0;

	while(flag)
	{
		PC_CMD_DATA pcCmdData;
		initCmdData(pcCmdData,PC_CMD_TYPE_READ_FILE);
		pcCmdData.uParam.READ_FILE_PARAM.fileType = FILE_TYPE_TRANS;
		pcCmdData.uParam.READ_FILE_PARAM.uReadPram.transBinFile.fileOffset = fileOffset + *RetNumLen;
		if((selectedFile.uFileAttr.transBinEF.realFileLen - *RetNumLen) > 109)
			pcCmdData.uParam.READ_FILE_PARAM.uReadPram.transBinFile.dataLen = 109;
		else
			pcCmdData.uParam.READ_FILE_PARAM.uReadPram.transBinFile.dataLen = selectedFile.uFileAttr.transBinEF.realFileLen - *RetNumLen;
		int ret = send_cmd(&pcCmdData);

		PC_CMD_REPLY pcRepData;
		memset(&pcRepData, 0, sizeof(PC_CMD_REPLY));

		if (0 == dispatch_cmd(&pcRepData, PC_CMD_TYPE_READ_FILE, 3*TIME_OUT))//timeout
		{
			PRINT("读透明文件超时\n");
			return IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_TIMEOUT;
		}

		switch (pcRepData.uReplyParam.readFileParam.readFileState)
		{
		case READ_FILE_STATE_SUCCESS:	//未完，继续读……
			{
				PRINT ("读透明文件成功\n");
				memcpy (buff + *RetNumLen, pcRepData.uReplyParam.readFileParam.buff,
							pcRepData.uReplyParam.readFileParam.retLen);
				*RetNumLen += pcRepData.uReplyParam.readFileParam.retLen;

				if(selectedFile.uFileAttr.transBinEF.realFileLen == *RetNumLen)
					flag = FALSE;
			}
			break;
		case READ_FILE_STATE_NOT_EXISTING:
			PRINT ("读透明文件不存在\n");
			return IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_NOT_EXISTING;
		case READ_FILE_STATE_FAILED:
			PRINT ("读透明文件失败\n");
			return IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_FAILED;
		case READ_FILE_STATE_NEED_PWD_VERIFY:
			PRINT ("读透明文件需先输入密码\n");
			return IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_NEED_PWD_VERIFY;
		case READ_FILE_STATE_NEED_ATH_VERIFY:
			PRINT ("读透明文件需先进入安全会话\n");
			return IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_NEED_ATH_VERIFY;
		case READ_FILE_STATE_NEED_NEV_VERIFY:
			PRINT ("糟糕，该透明文件属性为NEV，不允许读\n");
			return IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_NEED_NEV_VERIFY;
		default:
			return IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_NEED_NEV_VERIFY;
		}
	}

	PRINT("读透明文件完成\n");
	return IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_FINISH;
}

/******************************************************
*函数名称：rfWriteFile
*函数功能：写文件,分为首地址写文件和偏移量写地址，首地址即偏移量为0.
*函数参数：
*返回状态：
*/
RF_STATUS rfWriteFile(IN  char	FileType,		//文件类型，0为透明，1为记录
					  IN  WORD 	FileOffset,		//文件偏移量
					  IN  WORD 	WriteLength,	//写文件的长度
					  IN  char* pData			//待写入文件的数据缓冲区
					  )
{
	if (selectedFile.fileID == NULL)	//没选文件
	{
		PRINT ("写文件，没选文件\n");
		return IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED;
	}
	if ((selectedFile.securityType & TOKEN_UPDATE_MASK) == TOKEN_UPDATE_STATE_PWD)  //需要密码
	{
		if (false == pwdState.isUpdatePwdPassed)
		{
			PRINT ("写文件，请输入密码并保证密码正确\n");
			return IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_PWD_VERIFY;
		}
	}
	if ((selectedFile.securityType & TOKEN_UPDATE_MASK) == TOKEN_UPDATE_STATE_NEV)  //权限为NEV
	{
		PRINT ("写文件，权限为NEV，访问被拒绝\n");
		return IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_NEV_VERIFY;
	}

	PC_CMD_DATA  pcCmdData;
	PC_CMD_REPLY  pcRepData;
	//写透明文件
	if (0 == FileType)
	{
		if ((selectedFile.securityType & TOKEN_UPDATE_MASK) == TOKEN_UPDATE_STATE_ATH)  //需要校验
		{
			if ((rfSafeSession() != (IF_TYPE_SAFE_SESSION | SAFE_SESSION_STATE_SUCCESS)) &&
				(rfSafeSession() != (IF_TYPE_SAFE_SESSION | SAFE_SESSION_STATE_ALREADY_IN)))
			{
				PRINT ("写透明文件，安全会话不正确，请先进入安全会话\n");
				return IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_ATH_VERIFY;
			}
		}

		if (WriteLength <= CMD_WRITE_TRANS_MAX_LEN)
		{
			memset(&pcRepData, 0, sizeof(PC_CMD_REPLY));
			memset(&pcCmdData, 0, sizeof(PC_CMD_DATA));
			pcCmdData.sigStart = SIG_START_NUM;
			pcCmdData.sigEnd = SIG_END_NUM;
			pcCmdData.cmdType = PC_CMD_TYPE_WRITE_FILE;
			//初始化透明文件属性参数
			pcCmdData.uParam.WRITE_FILE_PARAM.fileType = FILE_TYPE_TRANS;
			pcCmdData.uParam.WRITE_FILE_PARAM.uWriteParam.transBinEF.fileOffset = FileOffset;
			pcCmdData.uParam.WRITE_FILE_PARAM.uWriteParam.transBinEF.pkgIndex = 1;
			pcCmdData.uParam.WRITE_FILE_PARAM.uWriteParam.transBinEF.dataLen = WriteLength;
			memcpy(pcCmdData.uParam.WRITE_FILE_PARAM.uWriteParam.transBinEF.dataBuffer, pData, WriteLength);

			int ret = send_cmd(&pcCmdData);
			if (ret != 0)
			{
				PRINT("写文件发送数据失败\n");
				return IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED;
			}

			if (dispatch_cmd(&pcRepData, PC_CMD_TYPE_WRITE_FILE, 3*TIME_OUT) == 0)
			{
				PRINT("--写文件超时 重试--\n");
				return IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_TIMEOUT;
			}

			switch(pcRepData.uReplyParam.writeFileParam.writeFileState)
			{
			case WRITE_FILE_STATE_SUCCESS:
				return IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_SUCCESS;
			case WRITE_FILE_STATE_FAILED:
				return IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED;
			case WRITE_FILE_STATE_TIMEOUT:
				return IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_TIMEOUT;
			default:
				return IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED;
			}
		}

		if (WriteLength > CMD_WRITE_TRANS_MAX_LEN) //分包发送
		{
			int length = WriteLength;
			int packetNum = 0;
			BYTE endFlag = 0;//判断是否是最后一包，1表示发完了

			while(length)
			{
				packetNum++;
				memset(&pcCmdData, 0, sizeof(PC_CMD_DATA));
				pcCmdData.sigStart = SIG_START_NUM;
				pcCmdData.sigEnd = SIG_END_NUM;
				pcCmdData.cmdType = PC_CMD_TYPE_WRITE_FILE;
				//初始化透明文件属性参数
				pcCmdData.uParam.WRITE_FILE_PARAM.fileType = FILE_TYPE_TRANS;

				if (length/109 > 0)
				{
					pcCmdData.uParam.WRITE_FILE_PARAM.uWriteParam.transBinEF.dataLen = 109;
					pcCmdData.uParam.WRITE_FILE_PARAM.uWriteParam.transBinEF.fileOffset = FileOffset + (packetNum-1)*109;
					memcpy(pcCmdData.uParam.WRITE_FILE_PARAM.uWriteParam.transBinEF.dataBuffer,
								(pData+(packetNum-1)*109), 109);
					length -= 109;
				}
				else
				{
					endFlag = 1;
					pcCmdData.uParam.WRITE_FILE_PARAM.uWriteParam.transBinEF.dataLen = length % 109;
					pcCmdData.uParam.WRITE_FILE_PARAM.uWriteParam.transBinEF.fileOffset = FileOffset + (packetNum-1)*109;
					memcpy(pcCmdData.uParam.WRITE_FILE_PARAM.uWriteParam.transBinEF.dataBuffer,
								(pData+(packetNum-1)*109), length%109);
					length -= length % 109;
				}

				int ret = send_cmd(&pcCmdData);
				if (ret != 0)
				{
					PRINT("写文件发送数据失败\n");
					return IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED;
				}

				memset(&pcRepData, 0, sizeof(PC_CMD_REPLY));
				if(dispatch_cmd(&pcRepData, PC_CMD_TYPE_WRITE_FILE, 3*TIME_OUT) == 0)
				{
					PRINT("--写文件超时 重试--\n");
					return IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_TIMEOUT;
				}

				switch(pcRepData.uReplyParam.writeFileParam.writeFileState)
				{
				case WRITE_FILE_STATE_SUCCESS:
					{
						if(endFlag == 1)
							return IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_SUCCESS;
					}
					break;
				case WRITE_FILE_STATE_FAILED:
					return IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED;
				case WRITE_FILE_STATE_TIMEOUT:
					return IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_TIMEOUT;
				default:
					return IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED;
				}
			}
		}
	}
	//写记录文件
	else
	{
		memset(&pcRepData, 0, sizeof(PC_CMD_REPLY));
		memset(&pcCmdData, 0, sizeof(PC_CMD_DATA));
		pcCmdData.sigStart = SIG_START_NUM;
		pcCmdData.sigEnd = SIG_END_NUM;
		pcCmdData.cmdType = PC_CMD_TYPE_WRITE_FILE;

		pcCmdData.uParam.WRITE_FILE_PARAM.fileType = FILE_TYPE_RECORD;
		pcCmdData.uParam.WRITE_FILE_PARAM.uWriteParam.recEF.recPointer = (unsigned char)FileOffset;
		pcCmdData.uParam.WRITE_FILE_PARAM.uWriteParam.recEF.opMode = 0x04;//更新原始文件中指定记录
		if(WriteLength > 108)
		{
			PRINT ("--数据长度太大，不能超过108字节！--");
			return IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED;
		}
		pcCmdData.uParam.WRITE_FILE_PARAM.uWriteParam.recEF.dataLen = WriteLength;
		memcpy(pcCmdData.uParam.WRITE_FILE_PARAM.uWriteParam.recEF.dataBuffer, pData, WriteLength);

		int ret = send_cmd(&pcCmdData);

		if (dispatch_cmd(&pcRepData, PC_CMD_TYPE_WRITE_FILE, 5*TIME_OUT) == 0)
		{
			PRINT("接收失败\n");
			return IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_TIMEOUT;
		}

		switch(pcRepData.uReplyParam.writeFileParam.writeFileState)
		{
		case WRITE_FILE_STATE_SUCCESS:
			PRINT("--写文件成功--\n");
			return IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_SUCCESS;
		case WRITE_FILE_STATE_ATH_VERIFY:
			PRINT ("写本条记录文件需安全会话\n");
			return IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_ATH_VERIFY;
		default:
			PRINT ("写文件失败 @ rfWriteFile \n");
			return IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED;
		}
	}
}


/******************************************************
*函数名称：rfVerifyPwdFile
*函数功能：密码验证。
*函数参数：
*返回状态：
*/
RF_STATUS rfVerifyPwdFile(IN BYTE  pwdType,	//验证密码类型
						  IN DWORD passWord	//密码
						  )
{
	int Info;
	int FLAG = 0;
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcRepData;
	initCmdData(pcCmdData,PC_CMD_TYPE_VERIFY_PWD_FILE);
	pcCmdData.uParam.VERIFY_PWD_FILE_PARAM.passWord = passWord;
	pcCmdData.uParam.VERIFY_PWD_FILE_PARAM.pwdType = pwdType;
	Info = send_cmd(&pcCmdData);
	if (0 == dispatch_cmd(&pcRepData, PC_CMD_TYPE_VERIFY_PWD_FILE, 3*TIME_OUT))
	{
		PRINT ("Send error in rfVerifyPwdFile\n");
		return IF_TYPE_VERIFY_PWD | VERIFY_PWD_FILE_STATE_TIMEOUT;
	}

	if (pcRepData.uReplyParam.verifyPwdFileParam.verifyState == VERIFY_PWD_FILE_STATE_FAILED)
	{
		PRINT ("Oops,Password verify failed!Check your pasword or deliver yourself to the police.\n");
		FLAG = 1;
	}

	switch (pwdType)
	{
	case TOKEN_TYPE_LIST:
		if (FLAG)
		{
			pwdState.isListPwdPassed = false;
			return IF_TYPE_VERIFY_PWD | VERIFY_PWD_FILE_STATE_FAILED;
		}

		pwdState.isListPwdPassed = true;
		break;
	case TOKEN_TYPE_VALID:
		if (FLAG)
		{
			pwdState.isValidPwdPassed = false;
			return IF_TYPE_VERIFY_PWD | VERIFY_PWD_FILE_STATE_FAILED;
		}

		pwdState.isValidPwdPassed = true;
		break;
	case TOKEN_TYPE_RDSEARCH:
		if (FLAG)
		{
			pwdState.isReadPwdPassed = false;
			return IF_TYPE_VERIFY_PWD | VERIFY_PWD_FILE_STATE_FAILED;
		}

		pwdState.isReadPwdPassed = true;
		break;
	case TOKEN_TYPE_SELECT:
		if (FLAG)
		{
			pwdState.isSelectPwdPassed = false;
			return IF_TYPE_VERIFY_PWD | VERIFY_PWD_FILE_STATE_FAILED;
		}

		pwdState.isSelectPwdPassed = true;
		break;
	case TOKEN_TYPE_UPDATE:
		if (FLAG)
		{
			pwdState.isUpdatePwdPassed = false;
			return IF_TYPE_VERIFY_PWD | VERIFY_PWD_FILE_STATE_FAILED;
		}

		printf ("Update password passed.\n");
		pwdState.isUpdatePwdPassed = true;
		break;
	case TOKEN_TYPE_KILLTAG:
		if (FLAG)
		{
			pwdState.isUpdatePwdPassed = false;
			return IF_TYPE_VERIFY_PWD | VERIFY_PWD_FILE_STATE_FAILED;
		}

		pwdState.isUpdatePwdPassed = true;
		break;
	}

	pubPwd = passWord;

	return IF_TYPE_VERIFY_PWD | VERIFY_PWD_FILE_STATE_SUCCESS;
}

/******************************************************
*函数名称：rfFileValidEn
*函数功能：使文件有效或无效置零。
*函数参数：
*返回状态：
×修改：
	  添加形参TID
*/
RF_STATUS rfFileValidEn(IN BYTE newFileState)  //新的文件状态
{
	int Info;
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcRepData;
	initCmdData(pcCmdData,PC_CMD_TYPE_VALID_FILE);
	if (selectedFile.fileID == NULL)
	{
		PRINT ("Select file first and do next! @ rfFileValidEn\n");
		return IF_TYPE_VALID_FILE | VALID_FILE_STATE_FILE_NOT_EXIST;
	}//没选文件
	if ((selectedFile.fileTypeValid & FILE_TYPE_MASK) == FILE_TYPE_DF)
	{
		PRINT("不能将目录文件置为无效或有效\n");
		return IF_TYPE_VALID_FILE | VALID_FILE_STATE_NOT_ALLOWED;
	}

	PRINT ("Input :%d,Before:%d\n",newFileState,selectedFile.fileTypeValid);

	if ( (selectedFile.fileTypeValid & FILE_INVALID_MASK) == !newFileState)
	{
		PRINT ("You know,you cant do same operate twice!\n");
		return IF_TYPE_VALID_FILE | VALID_FILE_STATE_ALREADY_INVALID;
	}//重复操作

 	if ((selectedFile.securityType & TOKEN_VALID_MASK) == TOKEN_VALID_STATE_NEV)
	{
		PRINT ("Cant Change the state of file\n");
		return IF_TYPE_VALID_FILE | VALID_FILE_STATE_NEV_VERIFY;
	}

	if ((selectedFile.securityType & TOKEN_VALID_MASK) == TOKEN_VALID_STATE_PWD)
	{
		if (false == pwdState.isValidPwdPassed)
		{
			PRINT ("Chcek your Password first! @ rfFileValidEn\n");
			return IF_TYPE_VALID_FILE | VALID_FILE_STATE_PWD_VERIFY;
		}
		pcCmdData.uParam.VERIFY_PWD_FILE_PARAM.passWord = pubPwd;
//Q:用哪个密码，传入的还是公共的
		pcCmdData.uParam.VERIFY_PWD_FILE_PARAM.pwdType = TOKEN_TYPE_VALID;
		pcCmdData.cmdType = PC_CMD_TYPE_VALID_FILE;
		pcCmdData.sigStart = SIG_START_NUM;
		pcCmdData.sigEnd = SIG_END_NUM;
	}//需要密码
	int tmpSafeSession;
	if ((selectedFile.securityType & TOKEN_VALID_MASK) == TOKEN_VALID_STATE_ATH)
	{
		tmpSafeSession = rfSafeSession();

		if ((tmpSafeSession != (IF_TYPE_SAFE_SESSION | SAFE_SESSION_STATE_SUCCESS)) &&
				(tmpSafeSession != (IF_TYPE_SAFE_SESSION | SAFE_SESSION_STATE_ALREADY_IN)))
		{
			PRINT ("Need ATH Verify.  @ rfFileValidEn\n");
			return IF_TYPE_VALID_FILE | VALID_FILE_STATE_ATH_VERIFY;
		}
	}

	pcCmdData.uParam.VALID_FILE_PARAM.newFileState = newFileState;
	Info = send_cmd(&pcCmdData);
	if (0 == dispatch_cmd(&pcRepData, PC_CMD_TYPE_VALID_FILE, 3*TIME_OUT))
	{
		return IF_TYPE_VALID_FILE | VALID_FILE_STATE_TIMEOUT;
	}

	switch (pcRepData.uReplyParam.validFileParam.validFileState)
	{
		case VALID_FILE_STATE_SUCCESS:
			{
				PRINT ("Change file state successful.\n");

				if (selectedFile.fileTypeValid == 1)
				{
					selectedFile.fileTypeValid &= (~FILE_INVALID_MASK);
				}
				else
				{
					selectedFile.fileTypeValid |= FILE_INVALID_MASK;
				}
			}
			return IF_TYPE_VALID_FILE | VALID_FILE_STATE_SUCCESS;
		case VALID_FILE_STATE_TIMEOUT:
			{
				PRINT ("Time out~Good lucky!\n");

				return IF_TYPE_VALID_FILE | VALID_FILE_STATE_TIMEOUT;
			}
		case VALID_FILE_STATE_FILE_NOT_EXIST:
			{
				PRINT ("Not found file,it hiddened in the dark.\n");
				return IF_TYPE_VALID_FILE | VALID_FILE_STATE_FILE_NOT_EXIST;
			}
		case VALID_FILE_STATE_NOT_AUTHORISED:
			{
				PRINT ("Sorry you cant modify file state.\n");
				return IF_TYPE_VALID_FILE | VALID_FILE_STATE_NOT_AUTHORISED;
			}

		case VALID_FILE_STATE_ALREADY_INVALID:
			{
				PRINT ("File already invalid.\n");
				return IF_TYPE_VALID_FILE | VALID_FILE_STATE_ALREADY_INVALID;
			}
		case VALID_FILE_STATE_PWD_VERIFY:
			PRINT("需PWD\n");
			return IF_TYPE_VALID_FILE | VALID_FILE_STATE_PWD_VERIFY;
		case VALID_FILE_STATE_ATH_VERIFY:
			PRINT("需ATH\n");
			return IF_TYPE_VALID_FILE | VALID_FILE_STATE_ATH_VERIFY;
		case VALID_FILE_STATE_NEV_VERIFY:
			PRINT("NEV，操作不允许\n");
			return IF_TYPE_VALID_FILE | VALID_FILE_STATE_NEV_VERIFY;
		default:
			PRINT ("What happened? @ rfFileValidEn\n");
			return IF_TYPE_VALID_FILE | VALID_FILE_STATE_UNKNOWN_ERR;
	}
}


/******************************************************
*函数名称：rfInitTag
*函数功能：初始化标签。
*函数参数：
*返回状态：
*/
RF_STATUS rfInitTag( IN	TAG_ATTRIBUTE*	tagAttr)	//标签属性
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	int Info;
	initCmdData(pcCmdData,PC_CMD_TYPE_INIT_TAG);
	pcCmdData.uParam.TAG_ATTR_PARAM.paraType = 0;	//0：基本参数 1：ECC_key参数
	memcpy(pcCmdData.uParam.TAG_ATTR_PARAM.ATTR.tagAttr.TID,tagAttr->TID,8);
	memcpy(pcCmdData.uParam.TAG_ATTR_PARAM.ATTR.tagAttr.UII,tagAttr->UII,32);
	pcCmdData.uParam.TAG_ATTR_PARAM.ATTR.tagAttr.memoryVolume = tagAttr->memoryVolume;
	pcCmdData.uParam.TAG_ATTR_PARAM.ATTR.tagAttr.inqMaxNum = tagAttr->inqMaxNum;
	pcCmdData.uParam.TAG_ATTR_PARAM.ATTR.tagAttr.inqMaxMatchNum = tagAttr->inqMaxMatchNum;
	memcpy(pcCmdData.uParam.TAG_ATTR_PARAM.ATTR.tagAttr.MIK,tagAttr->MIK,32);
	memcpy(pcCmdData.uParam.TAG_ATTR_PARAM.ATTR.tagAttr.MEK,tagAttr->MEK,32);
	Info = send_cmd(&pcCmdData);
	if (Info != 0)
	{
		PRINT("发送设置标签基本信息命令失败\n");
		return IF_TYPE_INIT_TAG | INIT_TAG_STATE_FAILED;
	}
	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_INIT_TAG, 3*TIME_OUT))
	{
		return IF_TYPE_INIT_TAG | INIT_TAG_STATE_TIMEOUT;
	}

	switch(pcCmdReply.uReplyParam.initTagStateParam.initTagState)
	{
	case INIT_TAG_STATE_SUCCESS:
		PRINT ("初始化标签成功\n");
		return IF_TYPE_INIT_TAG | INIT_TAG_STATE_SUCCESS;
	case INIT_TAG_STATE_FAILED:
		PRINT ("初始化标签失败\n");
		return IF_TYPE_INIT_TAG | INIT_TAG_STATE_FAILED;
	default:
		return IF_TYPE_INIT_TAG | INIT_TAG_STATE_TIMEOUT;
	}
}

/******************************************************
*函数名称：rfSetReaderInfo
*函数功能：设置阅读器信息。
*函数参数：
*返回状态：
*/
RF_STATUS rfSetReaderInfo(IN  READER_INFO_W*  readerInfo_w)		//设置阅读器信息
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;

	initCmdData(pcCmdData,PC_CMD_TYPE_SET_READER_INFO);
	pcCmdData.uParam.SET_READER_INFO_PARAM.paraType = 0;	//0：基本参数 1：ECC_key参数
	memcpy(pcCmdData.uParam.SET_READER_INFO_PARAM.u.readerInfo_w.RID,readerInfo_w->RID,3);
	memcpy(pcCmdData.uParam.SET_READER_INFO_PARAM.u.readerInfo_w.ReadName,readerInfo_w->ReadName,32);
	pcCmdData.uParam.SET_READER_INFO_PARAM.u.readerInfo_w.baudRate = readerInfo_w->baudRate;

	int Info = send_cmd(&pcCmdData);
	if (Info != 0)
	{
		PRINT("发送设置阅读器信息命令失败\n");
		return IF_TYPE_SET_RADER_INFO | SET_READER_INFO_STATE_FAILED;
	}
	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_SET_READER_INFO, 3*TIME_OUT))
	{
		return IF_TYPE_SET_RADER_INFO | SET_READER_INFO_TIMEOUT;
	}
	switch(pcCmdReply.uReplyParam.setReaderInfoParam.setReaderInfoState)
	{
	case SET_READER_INFO_STATE_SUCCESS:
		PRINT ("设置阅读器基本信息成功\n");
		return IF_TYPE_SET_RADER_INFO | SET_READER_INFO_STATE_SUCCESS;
	case SET_READER_INFO_STATE_FAILED:
		PRINT ("设置阅读器基本信息失败\n");
		return IF_TYPE_SET_RADER_INFO | SET_READER_INFO_STATE_FAILED;
	default:
		return IF_TYPE_SET_RADER_INFO | SET_READER_INFO_TIMEOUT;
	}
}

/******************************************************
*函数名称：rfGetReaderInfo
*函数功能：获取阅读器信息。
*函数参数：
*返回状态：
*/
RF_STATUS rfGetReaderInfo(OUT  READER_INFO*  readerInfo)		//获取阅读器信息
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;

	initCmdData(pcCmdData,PC_CMD_TYPE_GET_READER_INFO);
	int lengthPCDATA = sizeof(pcCmdData);
	int Info = send_cmd(&pcCmdData);

	if(Info != 0)
		return IF_TYPE_GET_RADER_INFO | GET_READER_INFO_STATE_FAILED;

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_GET_READER_INFO, 3*TIME_OUT))
	{
		return IF_TYPE_GET_RADER_INFO | GET_READER_INFO_TIMEOUT;
	}

	switch(pcCmdReply.uReplyParam.getReaderInfoParam.getReaderInfoSate)
	{
	case GET_READER_INFO_STATE_SUCCESS:
		PRINT ("查询阅读器基本信息成功\n");
		memcpy(readerInfo,&pcCmdReply.uReplyParam.getReaderInfoParam.readerInfo,sizeof(READER_INFO));
		return IF_TYPE_GET_RADER_INFO | GET_READER_INFO_STATE_SUCCESS;
	case GET_READER_INFO_STATE_FAILED:
		PRINT ("查询阅读器基本信息失败\n");
		return IF_TYPE_GET_RADER_INFO | GET_READER_INFO_STATE_FAILED;
	default:
		return IF_TYPE_GET_RADER_INFO | GET_READER_INFO_TIMEOUT;
	}
}


/******************************************************
*函数名称：rfGetTagInfo
*函数功能：获取标签基本信息
*函数参数：
*返回状态：
*备注：
*/
RFIDLIBDLL RF_STATUS rfGetTagInfo(OUT TAG_ATTRIBUTE_R*	tagInfo)
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData, PC_CMD_TYPE_GET_TAG_INFO);

	int Info = send_cmd(&pcCmdData);
	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_GET_TAG_INFO, 3*TIME_OUT))
	{
		return IF_TYPE_GET_TAG_INFO | GET_TAG_INFO_STATE_TIMEOUT;
	}

	memcpy(tagInfo, &pcCmdReply.uReplyParam.getTagInfoParam.tagAttribute, sizeof(TAG_ATTRIBUTE_R));

	switch(pcCmdReply.uReplyParam.getTagInfoParam.getTagInfoState)
	{
	case GET_TAG_INFO_STATE_SUCCESS:
		PRINT ("查询阅读器基本信息成功\n");
		return IF_TYPE_GET_RADER_INFO | GET_TAG_INFO_STATE_SUCCESS;
	case GET_TAG_INFO_STATE_FAILED:
		PRINT ("查询阅读器基本信息失败\n");
		return IF_TYPE_GET_RADER_INFO | GET_TAG_INFO_STATE_FAILED;
	default:
		return IF_TYPE_GET_RADER_INFO | GET_TAG_INFO_STATE_TIMEOUT;
	}
}


/******************************************************
*函数名称：rfSafeSession
*函数功能：安全会话。
*函数参数：
*返回状态：
Noitce : using global TID but not test
*/
RF_STATUS rfSafeSession ()
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcRepData;
	int Info;
	initCmdData(pcCmdData,PC_CMD_TYPE_SAFE_SESSION);

	if (isTIDselected != TRUE)
	{
		PRINT ("please SELECT A TAG before do safe session! @ rfSafeSession\n");

		return IF_TYPE_SAFE_SESSION | SAFE_SESSION_STATE_FAILED;
	}

	// 	if (isSafeSessionOpened == TRUE)
	// 	{
	// 		PRINT ("Hey,It looks like you have already opened safe-session,DONT do it twice OK? @ rfSafeSession\n");
	// 		return IF_TYPE_SAFE_SESSION | SAFE_SESSION_STATE_ALREADY_IN;
	// 	}

	memcpy(&pcCmdData.uParam.SAFE_SESSION_PARAM.TID,TID,8);

	Info = send_cmd(&pcCmdData);

	if (0 == dispatch_cmd(&pcRepData,PC_CMD_TYPE_SAFE_SESSION,INFINITE) )
	{
		PRINT ("Dispatch Error  Timeout. @ rfSafeSession\n");
		return IF_TYPE_SAFE_SESSION | SAFE_SESSION_STATE_TIMEOUT;
	}
	if (pcRepData.cmdType!= PC_CMD_TYPE_SAFE_SESSION ||
		pcRepData.SIG_START != SIG_START_NUM ||
		pcRepData.SIG_END != SIG_END_NUM)
	{
		printf("??????????????????????????????????????????");
	}else{
		PRINT("get reply ok!\n");
	}
	switch(pcRepData.uReplyParam.safeSessionStateParam.safeSessionState)
	{
	case SAFE_SESSION_STATE_SUCCESS:
		{
			PRINT ("Safe session Operate OK! @ rfSafeSession\n");
			//isSafeSessionOpened = TRUE;
			return IF_TYPE_SAFE_SESSION | SAFE_SESSION_STATE_SUCCESS;
		}
	case SAFE_SESSION_STATE_UNKNOWN:
		{
			PRINT ("Unknown error,What happened?! @ rfSafeSession\n");
			return IF_TYPE_SAFE_SESSION | SAFE_SESSION_STATE_UNKNOWN;
		}
	case SAFE_SESSION_STATE_TIMEOUT:
		{
			PRINT ("Errrrrr,Time out,check you device! @ rfSafeSession\n");
			return IF_TYPE_SAFE_SESSION | SAFE_SESSION_STATE_TIMEOUT;
		}
	case SAFE_SESSION_STATE_FAILED:
		{
			PRINT ("Tolinayitsu,It failed,check everything please! @ rfSafeSession\n");
			return IF_TYPE_SAFE_SESSION | SAFE_SESSION_STATE_FAILED;
		}
	case SAFE_SESSION_STATE_ALREADY_IN:
		{
			PRINT ("已进入安全会话  @安全会话API\n");
			return IF_TYPE_SAFE_SESSION | SAFE_SESSION_STATE_ALREADY_IN;
		}
	}
	return 0;
}

/******************************************************
*函数名称：rfAesRegTag
*函数功能：注册AES标签。
*函数参数：
*返回状态：
*/
RF_STATUS rfAesRegTag(IN BYTE* getTID,
					  IN BYTE* getMIK,
					  IN BYTE* getMEK
					 )
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcRepData;
	int Info;
	initCmdData(pcCmdData,PC_CMD_TYPE_REGISTER_TAG);
	memcpy(pcCmdData.uParam.REGISTER_TAG_PARAM.TID,getTID,8);
	memcpy(pcCmdData.uParam.REGISTER_TAG_PARAM.u.AES_KEY.MIK,getMIK,32);
	memcpy(pcCmdData.uParam.REGISTER_TAG_PARAM.u.AES_KEY.MEK,getMEK,32);
	pcCmdData.uParam.REGISTER_TAG_PARAM.keyType = 0;

	Info = send_cmd(&pcCmdData);
	if (0 == dispatch_cmd(&pcRepData,PC_CMD_TYPE_REGISTER_TAG,INFINITE))
	{
		PRINT ("Dispatch Error! @rfRegTag \n");
		return IF_TYPE_REGISTER_TAG | REGISTER_TAG_STATE_FAILED;
	}

	switch(pcRepData.uReplyParam.registerTagStateParam.registerTagState)
	{
	case REGISTER_TAG_STATE_SUCCESS:
		{
			PRINT ("Register Tag OK!\n");
			break;
		}
	case REGISTER_TAG_STATE_FAILED:
		{
			PRINT ("Register Tag failed!check ,check! @ rfRegTag\n");
			return IF_TYPE_REGISTER_TAG | REGISTER_TAG_STATE_FAILED;
		}
	}

	return IF_TYPE_REGISTER_TAG | REGISTER_TAG_STATE_SUCCESS;
}

/******************************************************
*函数名称：rfEccRegTag
*函数功能：注册Ecc标签。
*函数参数：
*返回状态：
*/

RF_STATUS rfEccRegTag(IN BYTE* getTID,
					  IN BYTE* getTagPubkey,
					  IN BYTE  keyLen
					 )
{
	// 	//与标签建立通信
	// 	BYTE ConnectedTag[6];
	// 	memcpy (ConnectedTag,"listen",6);
	// 	send_cmd((PC_CMD_DATA*)ConnectedTag,6);
	// 	BYTE pkg[9];
	// 	for (int i=0; i < 1000; i++)
	// 	{
	// 		DispatchCMD((PC_CMD_REPLY*)pkg);
	// 		if (0 == memcmp (pkg,"connected",9))	//建立通信成功
	// 			break;
	// 		else									//建立通信失败
	// 			return IF_TYPE_REGISTER_TAG | REGISTER_TAG_STATE_TIMEOUT;
	// 	}
	//
	// 	//建立通信成功后进行下一步设置参数
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcRepData;
	int Info;
	initCmdData(pcCmdData,PC_CMD_TYPE_REGISTER_TAG);
	pcCmdData.uParam.REGISTER_TAG_PARAM.keyType = 1;

	memcpy(pcCmdData.uParam.REGISTER_TAG_PARAM.TID,getTID,8);
	if (keyLen == 0 || keyLen == PKI_PRIVATEKEY_LEN)
	{
		memcpy(pcCmdData.uParam.REGISTER_TAG_PARAM.u.ECC_KEY.pubKeyTag,getTagPubkey,PKI_PRIVATEKEY_LEN);
	}else{
		memcpy(pcCmdData.uParam.REGISTER_TAG_PARAM.u.ECC_KEY.pubKeyTag,getTagPubkey,keyLen);
	}

	Info = send_cmd(&pcCmdData);
	Sleep(150);
	Info = send_cmd(&pcCmdData);

	if (0 == dispatch_cmd(&pcRepData,PC_CMD_TYPE_REGISTER_TAG,INFINITE))
	{
		// 		//释放与标签建立的通信
		// 		BYTE UnconnectedTag[4];
		// 		memcpy (UnconnectedTag,"exit",4);
		// 		send_cmd((PC_CMD_DATA*)UnconnectedTag,4);

		PRINT ("Dispatch Error! @rfRegTag \n");
		return IF_TYPE_REGISTER_TAG | REGISTER_TAG_STATE_FAILED;
	}
	// 	//释放与标签建立的通信
	// 	BYTE UnconnectedTag[4];
	// 	memcpy (UnconnectedTag,"exit",4);
	// 	send_cmd((PC_CMD_DATA*)UnconnectedTag,4);

	switch(pcRepData.uReplyParam.registerTagStateParam.registerTagState)
	{
	case REGISTER_TAG_STATE_SUCCESS:
		{
			PRINT ("Register Tag OK!\n");
			break;
		}
	case REGISTER_TAG_STATE_FAILED:
		{
			PRINT ("Register Tag failed!check ,check! @ rfRegTag\n");
			return IF_TYPE_REGISTER_TAG | REGISTER_TAG_STATE_FAILED;
		}
	}

	return IF_TYPE_REGISTER_TAG | REGISTER_TAG_STATE_SUCCESS;
}

/******************************************************
*函数名称：rfCleanAllTag
*函数功能：清除所有的已经注册的标签信息。
*函数参数：
*返回状态：
*/
RF_STATUS rfCleanAllTag ()
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcRepData;
	initCmdData(pcCmdData,PC_CMD_TYPE_CLR_ALL_REGISTER_TAG);
	send_cmd(&pcCmdData);
	if (0 == dispatch_cmd(&pcRepData, PC_CMD_TYPE_CLR_ALL_REGISTER_TAG, 3*TIME_OUT))
	{
		PRINT ("Dispatch Error! @rfCleanTag \n");
		return IF_TYPE_CLR_ALL_TAG | CLR_ALL_REGISTER_TAG_STATE_FAILED;
	}
	switch(pcRepData.uReplyParam.clrAllRegsitedTagParam.clrAllTagInfoState)
	{
	case CLR_ALL_REGISTER_TAG_STATE_SUCCESS:
		{
			PRINT ("clean all tags ok!\n");
			break;
		}
	case CLR_ALL_REGISTER_TAG_STATE_FAILED:
		{
			PRINT ("Clean all tags failed,check check!\n");
			return IF_TYPE_CLR_ALL_TAG | CLR_ALL_REGISTER_TAG_STATE_FAILED;
		}
	}

	return IF_TYPE_CLR_ALL_TAG | CLR_ALL_REGISTER_TAG_STATE_SUCCESS;
}

/******************************************************
*函数名称：rfMultiTagAlert
*函数功能：多标签报警。
*函数参数：
*返回状态：
*/
RF_STATUS rfMultiTagAlert(IN  TID_INFO*	tidArray,	//标签数组
						  IN  DWORD		tagNums,	//标签个数
						  IN  BYTE		openOrClose	//打开或关闭（0开1闭）
						  )
{
	RF_STATUS status;

	for (DWORD i=0; i < tagNums; i++)
	{
		status = rfSelectTag((BYTE*)(tidArray[i].TID));
		if (((IF_TYPE_SELECT_TAG | SELECT_TAG_STATE_SUCCESS) ==status ) ||
			(IF_TYPE_SELECT_TAG | SELECT_TAG_STATE_ALREADY_SELECTED) == status)
		{
			rfOpenMonitorSensor(0x00,0x01,openOrClose);
		}
	}
	return 0;
}


/******************************************************
*函数名称：rfFileDelete
*函数功能：文件删除
*函数参数：
*返回状态：
*备注:
*/
RF_STATUS rfFileDelete()
{
	if (selectedFile.fileID == 0x3f00 ||
		selectedFile.fileID == 0xf000 ||
		selectedFile.fileID == 0xa101 ||
		selectedFile.fileID == 0xa102 ||
		selectedFile.fileID == 0xa103)
	{
		PRINT("系统文件不能删除\n");
		return IF_TYPE_FILE_DELETE | FILE_DELETE_STATE_SYSFILE;
	}

	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_FILE_DELETE);

	send_cmd(&pcCmdData);

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_FILE_DELETE, 3*TIME_OUT))
	{
		PRINT ("DispatchCMD 文件删除超时!\n");
		return IF_TYPE_FILE_DELETE | FILE_DELETE_STATE_TIMEOUT;
	}
	switch (pcCmdReply.uReplyParam.fileDeleteParam.fileDeleteState)
	{
	case FILE_DELETE_STATE_SUCCESS:
		PRINT ("文件删除成功\n");

		return IF_TYPE_FILE_DELETE | pcCmdReply.uReplyParam.fileDeleteParam.fileDeleteState;
	case FILE_DELETE_STATE_TIMEOUT:
		PRINT ("文件删除超时\n");
		return IF_TYPE_FILE_DELETE | pcCmdReply.uReplyParam.fileDeleteParam.fileDeleteState;
	default:
		PRINT ("文件删除未知错误\n");
		return IF_TYPE_FILE_DELETE | FILE_DELETE_STATE_UNKNOWN_ERR;
	}
}

/******************************************************
*函数名称：rfFileSysFormat
*函数功能：文件系统格式化
*函数参数：
*返回状态：
*备注:
*/
RF_STATUS rfFileSysFormat(IN  DWORD  pwd)
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_FILESYS_FORMAT);
	pcCmdData.uParam.FILESYS_FORMAT_PARAM.formatPwd = pwd;

	send_cmd(&pcCmdData);

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_FILESYS_FORMAT, 3*TIME_OUT))
	{
		PRINT ("DispatchCMD 文件系统格式化超时!\n");
		return IF_TYPE_FILESYS_FORMAT | FILESYS_FORMAT_STATE_TIMEOUT;
	}
	switch (pcCmdReply.uReplyParam.fileDeleteParam.fileDeleteState)
	{
	case FILESYS_FORMAT_STATE_SUCCESS:
		PRINT ("文件系统格式化成功\n");
		return IF_TYPE_FILESYS_FORMAT | FILESYS_FORMAT_STATE_SUCCESS;
	case FILESYS_FORMAT_STATE_TIMEOUT:
		PRINT ("文件系统格式化超时\n");
		return IF_TYPE_FILESYS_FORMAT | FILESYS_FORMAT_STATE_TIMEOUT;
	case FILESYS_FORMAT_STATE_PWD_ERROR:
		PRINT ("文件系统格式化输入的密码错误\n");
		return IF_TYPE_FILESYS_FORMAT | FILESYS_FORMAT_STATE_PWD_ERROR;
	}
	return 0;
}

/******************************************************
*函数名称：rfImpoReaderPriKey
*函数功能：导入阅读器私钥，将阅读器公钥保存至本地
*函数参数：
*返回状态：
*备注:
*/
RFIDLIBDLL RF_STATUS rfImpoReaderPriKey(OUT	BYTE	pubKeyReader[PKI_PUBLICKEY_LEN],	//阅读器公钥
										OUT	DWORD*	pubLenReader						//阅读器公钥长度
										)
{
	int Info;
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_SET_READER_INFO);
	BYTE	priKeyReader[PKI_PRIVATEKEY_LEN]={0};	//阅读器私钥
	DWORD	priLenReader;						//阅读器私钥长度
	//BYTE	ecdh[0x50];							//阅读器ECDH
	unsigned char tmpRand[8];					//随机数变量
	//	FILE *fp;									//用于保存文件
	srand((unsigned int)(time(NULL)));			//产生随机数种子（基于时间）
	tmpRand[0] = rand();
	tmpRand[1] = rand();
	tmpRand[2] = rand();
	tmpRand[3] = rand();
	tmpRand[4] = rand();
	tmpRand[5] = rand();
	tmpRand[6] = rand();
	tmpRand[7] = rand();

	//	makeECDH(tmpRand,ecdh);				//生成ECDH

	makeKey(tmpRand,pubKeyReader,pubLenReader,priKeyReader,&priLenReader);		//生成阅读器密钥

	pcCmdData.uParam.SET_READER_INFO_PARAM.paraType = 1; // 0：基本参数 1：ECC_key参数
	memcpy(pcCmdData.uParam.SET_READER_INFO_PARAM.u.priKeyReader,priKeyReader,PKI_PRIVATEKEY_LEN);

	//Info = send_cmd(&pcCmdData,sizeof(pcCmdData));			//第一遍的目的主要是打开通信
	Info = send_cmd(&pcCmdData);
	if (0 == dispatch_cmd(&pcCmdReply,PC_CMD_TYPE_SET_READER_INFO,INFINITE))
	{
		return IF_TYPE_SET_RADER_INFO | SET_READER_INFO_TIMEOUT;
	}
	switch(pcCmdReply.uReplyParam.setReaderInfoParam.setReaderInfoState)
	{
	case SET_READER_INFO_STATE_SUCCESS:
		PRINT ("rfImpoReaderPriKey成功\n");
		return IF_TYPE_SET_RADER_INFO | SET_READER_INFO_STATE_SUCCESS;
	case SET_READER_INFO_STATE_FAILED:
		PRINT ("rfImpoReaderPriKey失败\n");
		return IF_TYPE_SET_RADER_INFO | SET_READER_INFO_STATE_FAILED;
	default:
		return IF_TYPE_SET_RADER_INFO | SET_READER_INFO_TIMEOUT;
	}

	// 	fp = fopen ("ReaderPubKey","w+");
	// 	fwrite (pubKeyReader,sizeof(U8),*pubLenReader + sizeof(U32),fp);			//将阅读器公钥写入本地文件
	// 	fclose (fp);
}

/******************************************************
*函数名称：rfImpoTagPriReaderPub
*函数功能：导入标签私钥，将标签公钥保存至本地，导入阅读器公钥
*函数参数：
*返回状态：
*备注:
*/
RFIDLIBDLL RF_STATUS rfImpoTagPriReaderPub(IN  BYTE	  TID[8],							//输入标签ID
										   IN  BYTE	  pubKeyReader[PKI_PUBLICKEY_LEN],	//阅读器公钥
										   OUT BYTE	  pubKeyTag[PKI_PUBLICKEY_LEN],		//标签公钥
										   OUT DWORD* pubLenTag							//标签公钥长度
										   )
{
	int Info;
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_INIT_TAG);
	BYTE	priKeyTag[PKI_PRIVATEKEY_LEN]={0};		//标签私钥
	DWORD	priLenTag;							//标签私钥长度

	unsigned char tmpRand[8];					//随机数变量
	//	FILE *fp;									//用于保存文件
	srand((unsigned int)(time(NULL)));			//产生随机数种子（基于时间）
	tmpRand[0] = rand();
	tmpRand[1] = rand();
	tmpRand[2] = rand();
	tmpRand[3] = rand();
	tmpRand[4] = rand();
	tmpRand[5] = rand();
	tmpRand[6] = rand();
	tmpRand[7] = rand();

	makeKey(tmpRand,pubKeyTag,pubLenTag,priKeyTag,&priLenTag);		//生成标签密钥

	pcCmdData.uParam.TAG_ATTR_PARAM.paraType = 1; //0：基本参数 1：ECC_key参数
	memcpy(pcCmdData.uParam.TAG_ATTR_PARAM.ATTR.ECC_KEY.priKeyTag,priKeyTag,PKI_PUBLICKEY_LEN);
	memcpy(pcCmdData.uParam.TAG_ATTR_PARAM.ATTR.ECC_KEY.pubKeyReader,pubKeyReader,PKI_PUBLICKEY_LEN);

	// 	Info = send_cmd(&pcCmdData,sizeof(pcCmdData)); //第一遍的目的是建立通信
	// 	PRINT ("建立通信.\n");
	// 	Sleep(500);
	// 	Info = send_cmd(&pcCmdData,sizeof(pcCmdData));
	// 	Sleep(500);
	// 	Info = send_cmd(&pcCmdData,sizeof(pcCmdData));
	// 	initCmdData(pcCmdData,PC_CMD_TYPE_DISCONNECT_TAG);			//最后要断开通信
	// 	PRINT("关闭通信.\n");
	// 	Sleep(1000);
	Info = send_cmd(&pcCmdData);
	Sleep(1200);
	Info = send_cmd(&pcCmdData);
	if (0 == dispatch_cmd(&pcCmdReply,PC_CMD_TYPE_INIT_TAG,INFINITE))
	{
		return IF_TYPE_INIT_TAG | INIT_TAG_STATE_TIMEOUT;
	}
	switch(pcCmdReply.uReplyParam.initTagStateParam.initTagState)
	{
	case INIT_TAG_STATE_SUCCESS:
		PRINT ("rfImpoTagPriReaderPub成功\n");
		return IF_TYPE_INIT_TAG | INIT_TAG_STATE_SUCCESS;
	case INIT_TAG_STATE_FAILED:
		PRINT ("rfImpoTagPriReaderPub失败\n");
		return IF_TYPE_INIT_TAG | INIT_TAG_STATE_FAILED;
	default:
		return IF_TYPE_INIT_TAG | INIT_TAG_STATE_TIMEOUT;
	}

	//fp = fopen ((LPCTSTR)TID,"w+");
	//fwrite (pubKeyTag,sizeof(U8),*pubLenTag + sizeof(U32),fp);			//标签将公钥写入本地文件
	//fclose (fp);
}

/******************************************************
*函数名称：rfSetChannel
*函数功能：设置阅读器或标签信道
*函数参数：
*返回状态：
*备注:
*/
RFIDLIBDLL RF_STATUS rfSetChannel(IN  BYTE	newChannel,	//范围从0到15
								  IN  BYTE	readerOrTag	//0:Reader,1:Tag
								  )
{
	if (0 == readerOrTag)		//设置Reader信道
	{
		if ((newChannel > 15) || (newChannel <0))
		{
			PRINT ("注意：设置阅读器信道超出0-15范围\n");
			return IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_OVERRANGE;
		}

		PC_CMD_DATA pcCmdData;
		PC_CMD_REPLY pcCmdReply;
		memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
		initCmdData(pcCmdData,PC_CMD_TYPE_SELLECT_CHANNEL);
		pcCmdData.uParam.SELECTED_CHANNEL.Channel = newChannel;

		int Ret = send_cmd(&pcCmdData);
		if (Ret != 0)
		{
			PRINT("发送设置阅读器信道命令失败\n");
			return IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_FAILED;
		}
		if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_SELLECT_CHANNEL, 3*TIME_OUT))
		{
			PRINT ("设置阅读器信道未收到回复，超时\n");
			return IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_TIMEOUT;
		}

		switch(pcCmdReply.uReplyParam.setChannelParam.setChannelState)
		{
		case SET_CHANNEL_SUCCESS:
			PRINT ("设置阅读器信道成功\n");
			return IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_SUCCESS;
		case SET_CHANNEL_FAILED:
			PRINT ("设置阅读器信道失败\n");
			return IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_FAILED;
		default:
			return IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_FAILED;
		}
	}
	else if (1 == readerOrTag)		//设置Tag信道
	{
		if ((newChannel > 15) || (newChannel <0))
		{
			PRINT ("注意：设置标签信道超出0-15范围\n");
			return IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_OVERRANGE;
		}

		PC_CMD_DATA pcCmdData;
		PC_CMD_REPLY pcCmdReply;
		initCmdData(pcCmdData,PC_CMD_TYPE_SET_CHANNEL_TAG);
		memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
		pcCmdData.uParam.SELECTED_CHANNEL.Channel = newChannel;

		int Ret = send_cmd(&pcCmdData);
		if (Ret != 0)
		{
			PRINT("发送设置标签信道命令失败");
			return IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_FAILED;
		}
		if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_SET_CHANNEL_TAG, 3*TIME_OUT))
		{
			PRINT ("设置标签信道未收到回复，超时\n");
			return IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_TIMEOUT;
		}

		switch(pcCmdReply.uReplyParam.setChannelParam.setChannelState)
		{
		case SET_CHANNEL_SUCCESS:
			PRINT ("设置标签信道成功\n");
			return IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_SUCCESS;
		case SET_CHANNEL_FAILED:
			PRINT ("设置标签信道失败\n");
			return IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_FAILED;
		default:
			return IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_FAILED;
		}
	}
	return 0;
}

/******************************************************
*函数名称：rfSetPower
*函数功能：设置发射功率
*函数参数：
*返回状态：
*备注:
*/
RFIDLIBDLL RF_STATUS rfSetPower(IN	BYTE  newPower, 	//阅读器范围从0到6，标签范围从0到8
								IN	BYTE  readerOrTag	//0:Reader,1:Tag
								)
{
	if (0 == readerOrTag)		//设置Reader功率
	{
		if ((newPower > 6) || (newPower < 0))
		{
			PRINT ("注意：设置阅读器功率超出0-6范围\n");
			return IF_TYPE_SET_POWER | SET_POWER_STATE_OVERRANGE;
		}
		int Info;
		PC_CMD_DATA pcCmdData;
		PC_CMD_REPLY pcCmdReply;
		initCmdData(pcCmdData,PC_CMD_TYPE_SET_POWER);
		pcCmdData.uParam.SELECTED_POWER.Power = newPower;

		Info = send_cmd(&pcCmdData);
		if (Info != 0)
		{
			PRINT("发送设置阅读器功率命令失败\n");
			return IF_TYPE_SET_POWER | SET_POWER_STATE_FAILED;
		}
		if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_SET_POWER, 3*TIME_OUT))
		{
			PRINT ("设置阅读器功率未收到回复，超时\n");
			return IF_TYPE_SET_POWER | SET_POWER_STATE_TIMEOUT;
		}

		switch(pcCmdReply.uReplyParam.setPowerParam.setPowerState)
		{
		case SET_POWER_STATE_SUCCESS:
			PRINT ("设置阅读器功率成功\n");
			return IF_TYPE_SET_POWER | SET_POWER_STATE_SUCCESS;
		case SET_POWER_STATE_FAILED:
			PRINT ("设置阅读器功率失败\n");
			return IF_TYPE_SET_POWER | SET_POWER_STATE_FAILED;
		default:
			PRINT ("设置阅读器功率超时\n");
			return IF_TYPE_SET_POWER | SET_POWER_STATE_FAILED;
		}
	}
	else if (1 == readerOrTag)		//设置Tag功率
	{
		if ((newPower > 8) || (newPower < 0))
		{
			PRINT ("注意：设置标签功率超出0-8范围\n");
			return IF_TYPE_SET_POWER_TAG | SET_POWER_STATE_OVERRANGE;
		}
		int Info;
		PC_CMD_DATA pcCmdData;
		PC_CMD_REPLY pcCmdReply;
		initCmdData(pcCmdData,PC_CMD_TYPE_SET_POWER_TAG);
		pcCmdData.uParam.SELECTED_POWER.Power = newPower;

		Info = send_cmd(&pcCmdData);
		if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_SET_POWER_TAG, 3*TIME_OUT))
		{
			PRINT ("设置标签功率未收到回复，超时\n");
			return IF_TYPE_SET_POWER_TAG | SET_POWER_STATE_TIMEOUT;
		}

		switch(pcCmdReply.uReplyParam.setPowerParam.setPowerState)
		{
		case SET_POWER_STATE_SUCCESS:
			PRINT ("设置标签功率成功\n");
			return IF_TYPE_SET_POWER_TAG | SET_POWER_STATE_SUCCESS;
		case SET_POWER_STATE_FAILED:
			PRINT ("设置标签功率失败\n");
			return IF_TYPE_SET_POWER_TAG | SET_POWER_STATE_FAILED;
		default:
			PRINT ("设置标签功率超时\n");
			return IF_TYPE_SET_POWER_TAG | SET_POWER_STATE_FAILED;
		}
	}
	return 0;
}


/******************************************************
*函数名称：rfKillTag
*函数功能：标签灭活
*函数参数：
*返回状态：
*备注:
*/
RFIDLIBDLL RF_STATUS rfKillTag(IN  DWORD  killTagToken)		//标签灭活
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_KILL_TAG);
	pcCmdData.uParam.KILL_TAG_PARAM.killTagToken = killTagToken;

	int Info = send_cmd(&pcCmdData);

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_KILL_TAG, 3*TIME_OUT))
	{
		PRINT ("DispatchCMD 标签灭活超时\n");
		return IF_TYPE_KILL_TAG | KILL_TAG_TIMEOUT;
	}
	switch (pcCmdReply.uReplyParam.killTagParam.killTagstate)
	{
	case KILL_TAG_SUCCESS:
		PRINT ("标签灭活成功\n");
		return IF_TYPE_KILL_TAG | KILL_TAG_SUCCESS;
	case KILL_TAG_FAILED:
		PRINT ("标签灭活失败\n");
		return IF_TYPE_KILL_TAG | KILL_TAG_FAILED;
	case KILL_TAG_TIMEOUT:
		PRINT ("标签灭活超时\n");
		return IF_TYPE_KILL_TAG | KILL_TAG_TIMEOUT;
	case 0x0902:
		PRINT("标签灭活口令为零，不能灭活\n");
		return IF_TYPE_KILL_TAG | 3;
	}
	return 0;
}

/******************************************************
*函数名称：rfWritecacheSync
*函数功能：写缓存同步
*函数参数：
*返回状态：
*备注:
*/
RFIDLIBDLL RF_STATUS rfWritecacheSync()
{
	int Info;
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_WRITECACHE_SYNC);
	Info = send_cmd(&pcCmdData);
	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_WRITECACHE_SYNC, 3*TIME_OUT))
	{
		return IF_TYPE_WRITECACHE_SYNC | WRITECACHE_SYNC_TIMEOUT;
	}
	switch(pcCmdReply.uReplyParam.writecacheSyncParam.writecacheSyncState)
	{
	case WRITECACHE_SYNC_SUCCESS:
		PRINT ("写缓存同步成功\n");
		return IF_TYPE_WRITECACHE_SYNC | WRITECACHE_SYNC_SUCCESS;
	case WRITECACHE_SYNC_FAILED:
		PRINT ("写缓存同步失败\n");
		return IF_TYPE_WRITECACHE_SYNC | WRITECACHE_SYNC_FAILED;
	case WRITECACHE_SYNC_TIMEOUT:
		return IF_TYPE_WRITECACHE_SYNC | WRITECACHE_SYNC_TIMEOUT;
	}
	return 0;
}

/******************************************************
*函数名称：rfWriteCache
*函数功能：把写透明文件的数据存入阅读区Flash中
*函数参数：
*返回状态：
*备注:
*/
RFIDLIBDLL RF_STATUS rfWriteCache(IN  WORD 	FileOffset,	  //文件偏移量
								  IN  DWORD WriteLength,  //写文件的长度
								  IN  char* pData		  //待写入文件的数据缓冲区
								  )
{
	if (selectedFile.fileID == NULL)
	{
		PRINT ("Select file first and do next! @ rfWriteCache\n");
		return IF_TYPE_WRITE_CACHE | WRITE_CACHE_FAILED;
	}//没选文件

	if ((selectedFile.securityType & TOKEN_UPDATE_MASK) == TOKEN_UPDATE_STATE_PWD)
	{
		if (false == pwdState.isUpdatePwdPassed)
		{
			PRINT ("Check your Password first! @ rfWriteCache\n");
			return IF_TYPE_WRITE_CACHE | WRITE_CACHE_FAILED;
		}
	}//需要密码

	if ((selectedFile.securityType & TOKEN_UPDATE_MASK) == TOKEN_UPDATE_STATE_ATH)
	{
		if ((rfSafeSession() != (IF_TYPE_SAFE_SESSION | SAFE_SESSION_STATE_SUCCESS)) &&
			(rfSafeSession() != (IF_TYPE_SAFE_SESSION | SAFE_SESSION_STATE_ALREADY_IN)))
		{
			PRINT ("Need Verify but not pass verify.  @ rfWriteCache\n");
			return IF_TYPE_WRITE_CACHE | WRITE_CACHE_FAILED;
		}
	}//需要校验

	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	int BufOffset = 0;
	WORD writedLength = 0;
	int Info;
	int length = WriteLength;
	int count = 0;
	int index = 1;//包的索引号，从1开始

	initCmdData(pcCmdData,PC_CMD_TYPE_WRITE_CACHE);
	pcCmdData.uParam.WRITE_CACHE_PARAM.fileOffset = FileOffset;
	pcCmdData.uParam.WRITE_CACHE_PARAM.dataLen = 0;
	memset (pcCmdData.uParam.WRITE_CACHE_PARAM.dataBuffer,0,sizeof(pcCmdData.uParam.WRITE_CACHE_PARAM.dataBuffer));
	Info = send_cmd(&pcCmdData);			//试探包

	while(length >= MAX_WRITE_CACHE_LEN)
	{
		count++;
		pcCmdData.uParam.WRITE_CACHE_PARAM.dataLen = MAX_WRITE_CACHE_LEN;

		memcpy(pcCmdData.uParam.WRITE_CACHE_PARAM.dataBuffer,pData+BufOffset,MAX_WRITE_CACHE_LEN);

		Info = send_cmd(&pcCmdData);

		// 		if (DispatchCMD (&pcCmdReply) == 0)
		// 		{
		// 			return IF_TYPE_WRITE_CACHE | WRITE_CACHE_TIMEOUT;
		// 		}
		//
		// 		if (WRITE_CACHE_SUCCESS != pcCmdReply.uReplyParam.writeCacheParam.writeCacheState)
		// 		{
		// 			PRINT ("Write File failed @ rfWriteFile \n");
		// 			return IF_TYPE_WRITE_CACHE | pcCmdReply.uReplyParam.writeCacheParam.writeCacheState;
		// 		}
		//Sleep(20);
		pcCmdData.uParam.WRITE_CACHE_PARAM.fileOffset += MAX_WRITE_CACHE_LEN;
		BufOffset += MAX_WRITE_CACHE_LEN;
		length -= MAX_WRITE_CACHE_LEN;

		//PRINT ("当前发送的包号为%d\n",index++);
		Sleep(1);
	}

	PRINT ("!Total sent %d packages,check to sent last package.\n",count);

	initCmdData(pcCmdData,PC_CMD_TYPE_SEND_CACHE);
	pcCmdData.uParam.WRITE_CACHE_PARAM.fileOffset += MAX_WRITE_CACHE_LEN;
	pcCmdData.uParam.WRITE_CACHE_PARAM.dataLen = length;
	PRINT("Send last %d bytes.\n",length);
	memcpy(pcCmdData.uParam.WRITE_CACHE_PARAM.dataBuffer,pData+BufOffset,length);
	Info = send_cmd(&pcCmdData);
	Sleep(1);

	//	PRINT("%d bytes left to send!\n",length);
	if (0 == dispatch_cmd(&pcCmdReply,PC_CMD_TYPE_WRITE_CACHE,INFINITE))
	{
		return IF_TYPE_WRITE_CACHE | WRITE_CACHE_TIMEOUT;
	}
	switch(pcCmdReply.uReplyParam.writeCacheParam.writeCacheState)
	{
	case WRITE_CACHE_SUCCESS:
		PRINT ("写缓存成功\n");
		return IF_TYPE_WRITE_CACHE | WRITE_CACHE_SUCCESS;
	case WRITE_CACHE_FAILED:
		PRINT ("写缓存失败\n");
		return IF_TYPE_WRITE_CACHE | WRITE_CACHE_FAILED;
	case WRITE_CACHE_TIMEOUT:
		PRINT ("写缓存超时\n");
		return IF_TYPE_WRITE_CACHE | WRITE_CACHE_TIMEOUT;
	}
	return 0;
}

/******************************************************
*函数名称：rfWriteLargeCache
*函数功能：把写大文件（大于64K）的数据存入阅读区Flash中
*函数参数：
*返回状态：
*备注:
*/
RFIDLIBDLL RF_STATUS rfWriteLargeCache(IN  DWORD  fileOffset,	//文件偏移量
									   IN  DWORD  dataLen,		//数据长度
									   OUT DWORD* retLen,		//返回写入的数据长度
									   IN  BYTE   buff[]		//写入的buffer
									   )
{
	if (dataLen > 240*1024)
	{
		return IF_TYPE_WRITE_CACHE | WRITE_CACHE_OVERRANGE;
	}
	int subFileIndex;
	int creatFileNums;//需创建的文件个数
	FILE_ATTRIBUTE_W fileToSelect;
	DWORD lengthToWrite;
	RF_STATUS ret;
	WORD tmp_fileID;

	*retLen = 0;
	memset(&fileToSelect, 0, sizeof FILE_ATTRIBUTE_W);
	tmp_fileID = selectedFile.fileID;

	PRINT ("Ready to write %d bytes.\n",dataLen);

	subFileIndex = 0;
	while (fileOffset >= MAX_FILE_LENGTH_IN_TAG)//定位读取起始位置
	{
		subFileIndex++;
		fileOffset -= MAX_FILE_LENGTH_IN_TAG;
	}
	creatFileNums = dataLen/MAX_FILE_LENGTH_IN_TAG;	//第一个文件已创建
	for (int i=0; i < creatFileNums; i++)//创建剩下的文件
	{
		subFileIndex++;
		fileToSelect.fileAttr_R.securityType = 0;//始终允许访问
		fileToSelect.fileAttr_R.fileID = selectedFile.fileID + subFileIndex;
		fileToSelect.fileAttr_R.fileTypeValid = 0x01;//有效、透明
		fileToSelect.fileAttr_R.uFileAttr.transBinEF.maxFileLen = MAX_FILE_LENGTH_IN_TAG;
		fileToSelect.fileAttr_R.uFileAttr.transBinEF.constMaxRecord = 1;
		fileToSelect.fileAttr_R.uFileAttr.transBinEF.realFileLen = 0;
		fileToSelect.access_tokens.transBinEF.read_token = 0;
		fileToSelect.access_tokens.transBinEF.update_token = 0;
		fileToSelect.access_tokens.transBinEF.valid_token = 0;
		rfCreateFile(&fileToSelect);
	}


	// 	ret = rfSelectFile((BYTE*)TID, selectedFile.fileID + subFileIndex, &fileToSelect.fileAttr_R);
	// 	if (SELECT_FILE_STATE_SUCCESS != (ret & 0xFF))//选择文件出错
	// 	{
	// 		PRINT("选择文件出错\n");
	// 		return IF_TYPE_SELECT_FILE | SELECT_FILE_STATE_TIMEOUT;
	// 	}

	do
	{
		if (fileOffset + dataLen > MAX_FILE_LENGTH_IN_TAG)//如果写操作跨越多个标签文件，这一次只写入一个文件
		{
			lengthToWrite = dataLen - (fileOffset + dataLen - MAX_FILE_LENGTH_IN_TAG);
		}
		else
		{
			lengthToWrite = dataLen;
		}

		PRINT ("Send %d bytes to Reader.\n",lengthToWrite);

		ret = rfWriteCache((WORD)fileOffset, lengthToWrite,  (char*)(buff + *retLen));
		if (WRITE_CACHE_SUCCESS != (ret & 0xFF))//写入出错
		{
			PRINT("写入出错\n");
			return IF_TYPE_WRITE_CACHE | (ret & 0xFF);
		}

		fileOffset = 0;
		dataLen -= lengthToWrite;
		*retLen += lengthToWrite;
	}
	while (dataLen > 0);

	PRINT("写大文件缓存成功\n");
	//rfSelectFile((BYTE*)TID, tmp_fileID, &selectedFile);
	return IF_TYPE_WRITE_CACHE | WRITE_CACHE_SUCCESS;
}

/******************************************************
*函数名称：rfSendCache
*函数功能：把阅读器Flash中的数据发送出去（给标签）
*函数参数：
*返回状态：
*备注:
*/
RFIDLIBDLL RF_STATUS rfSendCache()
{
	int Info;
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_SEND_CACHE);
	Info = send_cmd(&pcCmdData);
	if (0 == dispatch_cmd(&pcCmdReply,PC_CMD_TYPE_SEND_CACHE,60000))
	{
		PRINT ("rfSendCache没收到\n");
		return IF_TYPE_SEND_CACHE | SEND_CACHE_TIMEOUT;
	}
	switch(pcCmdReply.uReplyParam.sendCacheParam.sendCacheState)
	{
	case SEND_CACHE_SUCCESS:
		PRINT ("发送缓存成功\n");
		return IF_TYPE_SEND_CACHE | SEND_CACHE_SUCCESS;
	case SEND_CACHE_FAILED:
		PRINT ("发送缓存失败\n");
		return IF_TYPE_SEND_CACHE | SEND_CACHE_FAILED;
	case SEND_CACHE_TIMEOUT:
		PRINT ("发送缓存超时\n");
		return IF_TYPE_SEND_CACHE | SEND_CACHE_TIMEOUT;
	}
	return 0;
}

/******************************************************
*函数名称：rfCleanCache
*函数功能：清除缓存
*函数参数：
*返回状态：
*备注:
*/
RFIDLIBDLL RF_STATUS rfCleanCache()
{
	int Info;
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_CLEAN_CACHE);
	Info = send_cmd(&pcCmdData);
	if (0 == dispatch_cmd(&pcCmdReply,PC_CMD_TYPE_CLEAN_CACHE,60000))
	{
		PRINT ("rfCleanCache没收到\n");
		return IF_TYPE_CLEAN_CACHE | CLEAN_CACHE_TIMEOUT;
	}
	switch(pcCmdReply.uReplyParam.cleanCacheParam.cleanCacheState)
	{
	case CLEAN_CACHE_SUCCESS:
		PRINT ("清除缓存成功\n");
		return IF_TYPE_CLEAN_CACHE | CLEAN_CACHE_SUCCESS;
	case CLEAN_CACHE_FAILED:
		PRINT ("清除缓存失败\n");
		return IF_TYPE_CLEAN_CACHE | CLEAN_CACHE_FAILED;
	case CLEAN_CACHE_TIMEOUT:
		PRINT ("清除缓存超时\n");
		return IF_TYPE_CLEAN_CACHE | CLEAN_CACHE_TIMEOUT;
	}
	return 0;
}

/******************************************************
*函数名称：rfSendPurewave
*函数功能：设置纯载波
*函数参数：
*返回状态：
*备注:
*/
RFIDLIBDLL RF_STATUS rfSendPurewave()
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_SEND_PUREWAVE);

	int Info = send_cmd(&pcCmdData);

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_SEND_PUREWAVE, 3*TIME_OUT))
	{
		PRINT ("DispatchCMD 发射纯载波超时\n");
		return IF_TYPE_SEND_PUREWAVE | SEND_PUREVAVE_STATE_TIMEOUT;
	}

	switch (pcCmdReply.uReplyParam.sendPurewaveParam.sendPurewaveState)
	{
	case SEND_PUREWAVE_STATE_SUCCESS:
		PRINT ("发射纯载波成功\n");
		return IF_TYPE_SEND_PUREWAVE | SEND_PUREWAVE_STATE_SUCCESS;
	case SEND_PUREWAVE_STATE_FAILED:
		PRINT ("发射纯载波失败\n");
		return IF_TYPE_SEND_PUREWAVE | SEND_PUREWAVE_STATE_FAILED;
	case SEND_PUREVAVE_STATE_TIMEOUT:
		PRINT ("发射纯载波超时\n");
		return IF_TYPE_SEND_PUREWAVE | SEND_PUREVAVE_STATE_TIMEOUT;
	}
	return 0;
}

/******************************************************
*函数名称：rfStopPurewave
*函数功能：停止纯载波
*函数参数：
*返回状态：
*备注:
*/
RFIDLIBDLL RF_STATUS rfStopPurewave()
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_STOP_PUREWAVE);

	int Info = send_cmd(&pcCmdData);

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_STOP_PUREWAVE, 3*TIME_OUT))
	{
		PRINT ("DispatchCMD 停止纯载波超时\n");
		return IF_TYPE_STOP_PUREWAVE | STOP_PUREWAVE_STATE_TIMEOUT;
	}
	switch (pcCmdReply.uReplyParam.stopPurewaveParam.stopPurewaveState)
	{
	case STOP_PUREWAVE_STATE_SUCCESS:
		PRINT ("停止纯载波成功\n");
		return IF_TYPE_STOP_PUREWAVE | STOP_PUREWAVE_STATE_SUCCESS;
	case STOP_PUREWAVE_STATE_FAILED:
		PRINT ("停止纯载波失败\n");
		return IF_TYPE_STOP_PUREWAVE | STOP_PUREWAVE_STATE_FAILED;
	case STOP_PUREWAVE_STATE_TIMEOUT:
		PRINT ("停止纯载波超时\n");
		return IF_TYPE_STOP_PUREWAVE | STOP_PUREWAVE_STATE_TIMEOUT;
	}
	return 0;
}

/******************************************************
*函数名称：rfSendModulInfo
*函数功能：发射调制信息
*函数参数：
*返回状态：
*备注:
*/
RFIDLIBDLL RF_STATUS rfSendModulInfo()
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_SEND_MODUL_INFO);

	int Info = send_cmd(&pcCmdData);

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_SEND_MODUL_INFO, 3*TIME_OUT))
	{
		PRINT ("DispatchCMD 发射调制信息超时\n");
		return IF_TYPE_SEND_MODUL_INFO | SEND_MODUL_INFO_STATE_TIMEOUT;
	}
	switch (pcCmdReply.uReplyParam.sendModulInfoParam.sendModulInfoState)
	{
	case SEND_MODUL_INFO_STATE_SUCCESS:
		PRINT ("发射调制信息成功\n");
		return IF_TYPE_SEND_MODUL_INFO | SEND_MODUL_INFO_STATE_SUCCESS;
	case SEND_MODUL_INFO_STATE_FAILED:
		PRINT ("发射调制信息失败\n");
		return IF_TYPE_SEND_MODUL_INFO | SEND_MODUL_INFO_STATE_FAILED;
	case SEND_MODUL_INFO_STATE_TIMEOUT:
		PRINT ("发射调制信息超时\n");
		return IF_TYPE_SEND_MODUL_INFO | SEND_MODUL_INFO_STATE_TIMEOUT;
	}
	return 0;
}

/******************************************************
*函数名称：rfStopModulInfo
*函数功能：停止调制信息
*函数参数：
*返回状态：
*备注:
*/
RFIDLIBDLL RF_STATUS rfStopModulInfo()
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_STOP_MODUL_INFO);

	int Info = send_cmd(&pcCmdData);

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_STOP_MODUL_INFO, 3*TIME_OUT))
	{
		PRINT ("DispatchCMD 停止调制信息超时\n");
		return IF_TYPE_STOP_MODUL_INFO | STOP_MODUL_INFO_STATE_TIMEOUT;
	}
	switch (pcCmdReply.uReplyParam.stopModulInfoParam.stopModulInfoState)
	{
	case STOP_MODUL_INFO_STATE_SUCCESS:
		PRINT ("发射调制信息成功\n");
		return IF_TYPE_STOP_MODUL_INFO | STOP_MODUL_INFO_STATE_SUCCESS;
	case STOP_MODUL_INFO_STATE_FAILED:
		PRINT ("发射调制信息失败\n");
		return IF_TYPE_STOP_MODUL_INFO | STOP_MODUL_INFO_STATE_FAILED;
	case STOP_MODUL_INFO_STATE_TIMEOUT:
		PRINT ("发射调制信息超时\n");
		return IF_TYPE_STOP_MODUL_INFO | STOP_MODUL_INFO_STATE_TIMEOUT;
	}
	return 0;
}

/******************************************************
*函数名称：rfSetRecvModel
*函数功能：设置接收模式
*函数参数：
*返回状态：
*备注:
*/
RFIDLIBDLL RF_STATUS rfSetRecvModel()
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_SET_RECV_MODEL);

	int Info = send_cmd(&pcCmdData);

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_SET_RECV_MODEL, 3*TIME_OUT))
	{
		PRINT ("DispatchCMD 设置接收模式超时\n");
		return IF_TYPE_SET_RECV_MODEL | SET_RECV_MODEL_STATE_TIMEOUT;
	}
	switch (pcCmdReply.uReplyParam.setRecvModelParam.setRecvModelState)
	{
	case SET_RECV_MODEL_STATE_SUCCESS:
		PRINT ("设置接收模式成功\n");
		return IF_TYPE_SET_RECV_MODEL | SET_RECV_MODEL_STATE_SUCCESS;
	case SET_RECV_MODEL_STATE_FAILED:
		PRINT ("设置接收模式失败\n");
		return IF_TYPE_SET_RECV_MODEL | SET_RECV_MODEL_STATE_FAILED;
	case SET_RECV_MODEL_STATE_TIMEOUT:
		PRINT ("设置接收模式超时\n");
		return IF_TYPE_SET_RECV_MODEL | SET_RECV_MODEL_STATE_TIMEOUT;
	}
	return 0;
}

/******************************************************
*函数名称：rfStopRecvModel
*函数功能：停止接收模式
*函数参数：
*返回状态：
*备注:
*/
RFIDLIBDLL RF_STATUS rfStopRecvModel()
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_STOP_RECV_MODEL);

	int Info = send_cmd(&pcCmdData);
	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_STOP_RECV_MODEL, 3*TIME_OUT))
	{
		PRINT ("DispatchCMD 停止接收模式超时\n");
		return IF_TYPE_STOP_RECV_MODEL | STOP_RECV_MODEL_STATE_TIMEOUT;
	}
	switch (pcCmdReply.uReplyParam.stopRecvModelParam.stopRecvModelState)
	{
	case SET_RECV_MODEL_STATE_SUCCESS:
		PRINT ("停止接收模式成功\n");
		return IF_TYPE_STOP_RECV_MODEL | SET_RECV_MODEL_STATE_SUCCESS;
	case SET_RECV_MODEL_STATE_FAILED:
		PRINT ("停止接收模式失败\n");
		return IF_TYPE_STOP_RECV_MODEL | SET_RECV_MODEL_STATE_FAILED;
	case STOP_RECV_MODEL_STATE_TIMEOUT:
		PRINT ("停止接收模式超时\n");
		return IF_TYPE_STOP_RECV_MODEL | STOP_RECV_MODEL_STATE_TIMEOUT;
	}
	return 0;
}


/******************************************************
*函数名称：rfMakeKey
*函数功能：随机生成阅读器与标签的公钥、私钥
*函数参数：
*返回状态：
*备注:
*/
RFIDLIBDLL RF_STATUS rfMakeKey(	OUT	BYTE	w_pubKeyReader[PKI_PUBLICKEY_LEN],	//阅读器公钥
								OUT	DWORD*	w_pubLenReader,						//阅读器公钥长度
								OUT	BYTE	w_priKeyReader[PKI_PRIVATEKEY_LEN],	//阅读器私钥
								OUT DWORD*	w_priLenReader,						//阅读器私钥长度
								OUT BYTE	w_pubKeyTag[PKI_PUBLICKEY_LEN],		//标签公钥
								OUT DWORD*	w_pubLenTag,						//标签公钥长度
								OUT BYTE	w_priKeyTag[PKI_PRIVATEKEY_LEN],	//标签私钥
								OUT DWORD*	w_priLenTag							//标签私钥长度
								)
{
	unsigned char tmpRand[8];				//随机数变量
	FILE *fp;								//用于保存文件

	unsigned char ecdh[0x42] = {0};			//ECDH变量
	unsigned char pubKeyReader[69] = {0};	//阅读器公钥变量
	unsigned char priKeyReader[99] = {0};	//阅读器私钥变量
	unsigned long pubLenReader = 0;			//阅读器公钥长度
	unsigned long priLenReader = 0;			//阅读器私钥长度

	unsigned char pubKeyTag[69] = {0};		//标签公钥变量
	unsigned char priKeyTag[99] = {0};		//标签私钥变量
	unsigned long pubLenTag = 0;			//标签公钥长度
	unsigned long priLenTag = 0;			//标签私钥长度

	//*********************先生成阅读器密钥*******************************
	srand((unsigned int)(time(NULL)));			//产生随机数种子（基于时间）
	tmpRand[0] = rand();
	tmpRand[1] = rand();
	tmpRand[2] = rand();
	tmpRand[3] = rand();
	tmpRand[4] = rand();
	tmpRand[5] = rand();
	tmpRand[6] = rand();
	tmpRand[7] = rand();

	//makeECDH(tmpRand,ecdh);				//生成ECDH

	makeKey(tmpRand,pubKeyReader,&pubLenReader,priKeyReader,&priLenReader);		//生成阅读器密钥

	fp = fopen ("ReaderPubKey","wb");
	fwrite (pubKeyReader,sizeof(U8),pubLenReader + sizeof(U32),fp);			//将阅读器公钥写入本地文件
	fclose (fp);

	fp = fopen ("ReaderPriKey","wb");
	fwrite (priKeyReader,sizeof(U8),priLenReader + sizeof(U32),fp);			//将阅读器私钥写入本地文件
	fclose (fp);


	//*********************再生成标签密钥*******************************
	srand((unsigned int)(time(NULL)));			//产生随机数种子（基于时间）
	tmpRand[0] = rand();
	tmpRand[1] = rand();
	tmpRand[2] = rand();
	tmpRand[3] = rand();
	tmpRand[4] = rand();
	tmpRand[5] = rand();
	tmpRand[6] = rand();
	tmpRand[7] = rand();

	makeKey (tmpRand,pubKeyTag,&pubLenTag,priKeyTag,&priLenTag);		//生成标签密钥

	fp = fopen ("TagPubKey","wb");
	fwrite (pubKeyTag,sizeof(U8),pubLenTag + sizeof(U32),fp);			//将标签公钥写入本地文件
	fclose (fp);

	fp = fopen ("TagPriKey","wb");
	fwrite (priKeyTag,sizeof(U8),priLenTag + sizeof(U32),fp);			//将标签私钥写入本地文件
	fclose (fp);

	//-------传阅读器参数给调用者----------
	memcpy(w_pubKeyReader,pubKeyReader,PKI_PUBLICKEY_LEN);
	memcpy(w_priKeyReader,priKeyReader,PKI_PRIVATEKEY_LEN);
	*w_pubLenReader = pubLenReader + sizeof(U32);
	*w_priLenReader = priLenReader + sizeof(U32);
	//--------传标签参数给调用者---------
	memcpy(w_pubKeyTag,pubKeyTag,PKI_PUBLICKEY_LEN);
	memcpy(w_priKeyTag,priKeyTag,PKI_PRIVATEKEY_LEN);
	*w_pubLenTag = pubLenTag + sizeof(U32);
	*w_priLenTag = priLenTag + sizeof(U32);

	return 0;
}

/******************************************************
*函数名称：rfWirelessSendKey
*函数功能：空中发送阅读器与标签的公钥、私钥
*函数参数：
*返回状态：
*备注:
*/
RFIDLIBDLL RF_STATUS rfWirelessSendKey(	IN	BYTE	w_pubKeyReader[PKI_PUBLICKEY_LEN],	//阅读器公钥
										IN	DWORD	w_pubLenReader,						//阅读器公钥长度
										IN	BYTE	w_priKeyReader[PKI_PRIVATEKEY_LEN],	//阅读器私钥
										IN	DWORD	w_priLenReader,						//阅读器私钥长度
										IN	BYTE	w_pubKeyTag[PKI_PUBLICKEY_LEN],		//标签公钥
										IN	DWORD	w_pubLenTag,						//标签公钥长度
										IN	BYTE	w_priKeyTag[PKI_PRIVATEKEY_LEN],	//标签私钥
										IN	DWORD	w_priLenTag							//标签私钥长度
										)
{
	int Info;
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_SET_KEY);
	pcCmdData.uParam.WIRELESS_SET_KEY.paraType = READER_SET_KEY;
	memcpy(pcCmdData.uParam.WIRELESS_SET_KEY.ECC_KEY.pubKey,w_pubKeyReader,w_pubLenReader);
	memcpy(pcCmdData.uParam.WIRELESS_SET_KEY.ECC_KEY.priKey,w_priKeyReader,w_priLenReader);
	Info = send_cmd(&pcCmdData);
	if (0 == dispatch_cmd(&pcCmdReply,PC_CMD_TYPE_SET_KEY,INFINITE))
	{
		PRINT ("DispatchCMD 烧写阅读器超时\n");
		return IF_TYPE_WIRELESS_SET_KEY | WIRELESS_SET_KEY_STATE_TIMEOUT;
	}
	if ((pcCmdReply.cmdType != PC_CMD_TYPE_SET_KEY) ||
		(pcCmdReply.uReplyParam.wirelessSetKeyParam.wirelessSetKeyState == WIRELESS_SET_KEY_STATE_FAILED))
	{
		PRINT ("空中烧写Reader密钥失败，不进行Tag密钥烧写\n");
		return IF_TYPE_WIRELESS_SET_KEY | WIRELESS_SET_KEY_STATE_FAILED;
	}
	PRINT ("空中烧写Reader密钥成功，继续进行Tag密钥烧写......\n");

	Sleep(10);

	initCmdData(pcCmdData,PC_CMD_TYPE_SET_KEY);
	memset(&pcCmdReply,0,sizeof(PC_CMD_REPLY));
	pcCmdData.uParam.WIRELESS_SET_KEY.paraType = TAG_SET_KEY;
	memcpy(pcCmdData.uParam.WIRELESS_SET_KEY.ECC_KEY.pubKey,w_pubKeyTag,w_pubLenTag);
	memcpy(pcCmdData.uParam.WIRELESS_SET_KEY.ECC_KEY.priKey,w_priKeyTag,w_priLenTag);
	Info = send_cmd(&pcCmdData);
	if (0 == dispatch_cmd(&pcCmdReply,PC_CMD_TYPE_SET_KEY,INFINITE))
	{
		PRINT ("DispatchCMD 烧写标签超时\n");
		return IF_TYPE_WIRELESS_SET_KEY | WIRELESS_SET_KEY_STATE_TIMEOUT;
	}
	if ((pcCmdReply.cmdType != PC_CMD_TYPE_SET_KEY) ||
		(pcCmdReply.uReplyParam.wirelessSetKeyParam.wirelessSetKeyState == WIRELESS_SET_KEY_STATE_FAILED))
	{
		PRINT ("空中烧写Tag密钥失败\n");
		return IF_TYPE_WIRELESS_SET_KEY | WIRELESS_SET_KEY_STATE_FAILED;
	}
	PRINT ("空中烧写密钥成功！\n");

	return IF_TYPE_WIRELESS_SET_KEY | WIRELESS_SET_KEY_STATE_SUCCESS;
}

/******************************************************
*函数名称：rfSearchRecords
*函数功能：检索记录
*函数参数：
*返回状态：
*备注:
*/
RFIDLIBDLL RF_STATUS rfSearchRecords(IN  SEARCH_ELEMENT*  searchElement,
									 IN  DWORD	elementNums,
									 OUT WORD*	retRecords
									 )
{
	unsigned char* wPt = (unsigned char*)searchElement;		//保存头指针
	SEARCH_ELEMENT* head = searchElement;
	for (WORD i=0; i < elementNums; i++)
	{

		searchElement =(SEARCH_ELEMENT*) wPt;
		
		if ((searchElement->relationOperator != 0x3d) &&
			(searchElement->relationOperator != 0x47) &&
			(searchElement->relationOperator != 0x4c) &&
			(searchElement->relationOperator != 0x3c) &&
			(searchElement->relationOperator != 0x3e) &&
			(searchElement->relationOperator != 0x21))
		{
			PRINT ("i = %d 检索记录失败,关系应为=、G、L、>、<、！的其中一个\n",i);
			return IF_TYPE_SEARCH_RECORDS | SEARCH_RECORDS_STATE_FAILED;
		}
		if ((searchElement->logicalOperator != 0x41) &&
			(searchElement->logicalOperator != 0x43) &&
			(searchElement->logicalOperator != 0x4f))
		{
			PRINT ("检索记录失败,逻辑应为C、A、O的其中一个\n");
			return IF_TYPE_SEARCH_RECORDS | SEARCH_RECORDS_STATE_FAILED;
		}

		wPt += sizeof(SEARCH_ELEMENT);

	}

	int Info;
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_SEARCH_RECORDS);
	int j = 0;

	PSEARCH_ELEMENT pSearchElement,pSearchOrignal;
	searchElement = head;						//置回头指针
	pSearchElement = (PSEARCH_ELEMENT)malloc(300);
	memset(pSearchElement,0,300);
	pSearchOrignal = pSearchElement;
	for(DWORD i=0; i < elementNums; i++)
	{
		*pSearchElement = *searchElement;
		memcpy(pSearchElement->dataBuf,searchElement->dataBuf,searchElement->dataLen);
		char* tp = (char*)&(pSearchElement->dataBuf) + pSearchElement->dataLen;
		pSearchElement = (PSEARCH_ELEMENT)tp;
		searchElement++;
	}
	j = (unsigned char*)pSearchElement - (unsigned char*)pSearchOrignal;
	pcCmdData.uParam.SEARCH_RECORDS.dataLen = j;
	memcpy(pcCmdData.uParam.SEARCH_RECORDS.dataBuf,pSearchOrignal,80);
	PRINT ("查询元素为:%x\n",pcCmdData.uParam.SEARCH_RECORDS.dataBuf);
	PRINT ("查询长度为:%d\n",j);
	Info = send_cmd(&pcCmdData);
	if (0 == dispatch_cmd(&pcCmdReply,PC_CMD_TYPE_SEARCH_RECORDS,INFINITE))
	{
		PRINT ("DispatchCMD 检索记录超时\n");
		return IF_TYPE_SEARCH_RECORDS | SEARCH_RECORDS_STATE_TIMEOUT;
	}
	switch (pcCmdReply.uReplyParam.searchRecordsParam.searchRecordsState)
	{
	case SEARCH_RECORDS_STATE_SUCCESS:
		PRINT ("检索记录成功\n");
		*retRecords = pcCmdReply.uReplyParam.searchRecordsParam.retRecords;
		return IF_TYPE_SEARCH_RECORDS | SEARCH_RECORDS_STATE_SUCCESS;
	case SEARCH_RECORDS_STATE_FAILED:
		PRINT ("检索记录失败\n");
		return IF_TYPE_SEARCH_RECORDS | SEARCH_RECORDS_STATE_FAILED;
	case SEARCH_RECORDS_STATE_TIMEOUT:
		PRINT ("检索记录超时\n");
		return IF_TYPE_SEARCH_RECORDS | SEARCH_RECORDS_STATE_TIMEOUT;
	}
	return 0;
}

/******************************************************
*函数名称：rfSearchAlltagRecs
*函数功能：检索所有标签记录
*函数参数：
*返回状态：
*备注:
*/
RFIDLIBDLL RF_STATUS rfSearchAlltagRecs(IN SEARCH_ELEMENT*	searchElement,
										IN DWORD	elementNums
										)
{
	unsigned char* wPt = (unsigned char*)searchElement;	//保存头指针
	SEARCH_ELEMENT* head = searchElement;
	for (DWORD i=0; i < elementNums; i++)
	{

		searchElement =(SEARCH_ELEMENT*) wPt;
		//PRINT("i = %d rELATION OPERATOR:%c\n",i,searchElement->relationOperator);
		if ((searchElement->relationOperator != RELATION_EQAUL) &&
			(searchElement->relationOperator != RELATION_EQUAL_GREATER) &&
			(searchElement->relationOperator != RELATION_EQUAL_LESS) &&
			(searchElement->relationOperator != RELATION_LESS) &&
			(searchElement->relationOperator != RELATION_GREATER) &&
			(searchElement->relationOperator != RELATION_NOT_EQUAL))
		{
			PRINT ("i = %d 检索记录失败,关系应为=、G、L、>、<、！的其中一个\n",i);
			return IF_TYPE_SEARCH_RECORDS | SEARCH_RECORDS_STATE_FAILED;
		}
		if ((searchElement->logicalOperator != LOGICAL_C) &&
			(searchElement->logicalOperator != LOGICAL_A) &&
			(searchElement->logicalOperator != LOGICAL_O))
		{
			PRINT ("检索记录失败,逻辑应为C、A、O的其中一个\n");
			return IF_TYPE_SEARCH_RECORDS | SEARCH_RECORDS_STATE_FAILED;
		}
		//PRINT("search element length:%d\n",sizeof(SEARCH_ELEMENT));
		//searchElement = searchElement + sizeof(SEARCH_ELEMENT);
		wPt += sizeof(SEARCH_ELEMENT);

	}

	int Info;
	PC_CMD_DATA pcCmdData;
	//PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_SEARCH_ALLTAG_RECS);
	int j = 0;

	PSEARCH_ELEMENT pSearchElement,pSearchOrignal;
	searchElement = head;						//置回头指针
	pSearchElement = (PSEARCH_ELEMENT)malloc(300);
	memset(pSearchElement,0,300);
	pSearchOrignal = pSearchElement;
	for (DWORD i=0; i < elementNums; i++)
	{
		*pSearchElement = *searchElement;
		memcpy(pSearchElement->dataBuf,searchElement->dataBuf,searchElement->dataLen);
		char* tp = (char*)&(pSearchElement->dataBuf) + pSearchElement->dataLen;
		pSearchElement = (PSEARCH_ELEMENT)tp;
		searchElement++;
	}
	j = (unsigned char*)pSearchElement - (unsigned char*)pSearchOrignal;
	pcCmdData.uParam.SEARCH_RECORDS.dataLen = j;
	memcpy(pcCmdData.uParam.SEARCH_RECORDS.dataBuf,pSearchOrignal,80);
	PRINT ("查询元素为:%x\n",pcCmdData.uParam.SEARCH_RECORDS.dataBuf);
	PRINT ("查询长度为:%d\n",j);
	Info = send_cmd(&pcCmdData);
	Sleep(2000);
	PRINT ("命令发送完成完成");
	return IF_TYPE_SEARCH_RECORDS | SEARCH_RECORDS_STATE_SUCCESS;
}

/******************************************************
*函数名称：rfSearchAlltagTrans
*函数功能：检索所有标签透明
*函数参数：
*返回状态：
*备注:
*/
RFIDLIBDLL RF_STATUS rfSearchAlltagTrans(IN SEARCH_ELEMENT*	searchElement,
										 IN DWORD	elementNums
										 )
{
	unsigned char* wPt = (unsigned char*)searchElement;		//保存头指针
	SEARCH_ELEMENT* head = searchElement;
	for (DWORD i=0; i < elementNums; i++)
	{

		searchElement =(SEARCH_ELEMENT*) wPt;
		//PRINT("i = %d rELATION OPERATOR:%c\n",i,searchElement->relationOperator);
		if ((searchElement->relationOperator != RELATION_EQAUL) &&
			(searchElement->relationOperator != RELATION_EQUAL_GREATER) &&
			(searchElement->relationOperator != RELATION_EQUAL_LESS) &&
			(searchElement->relationOperator != RELATION_LESS) &&
			(searchElement->relationOperator != RELATION_GREATER) &&
			(searchElement->relationOperator != RELATION_NOT_EQUAL))
		{
			PRINT ("i = %d 检索记录失败,关系应为=、G、L、>、<、！的其中一个\n",i);
			return IF_TYPE_SEARCH_RECORDS | SEARCH_RECORDS_STATE_FAILED;
		}
		if ((searchElement->logicalOperator != LOGICAL_C) &&
			(searchElement->logicalOperator != LOGICAL_A) &&
			(searchElement->logicalOperator != LOGICAL_O))
		{
			PRINT ("检索记录失败,逻辑应为C、A、O的其中一个\n");
			return IF_TYPE_SEARCH_RECORDS | SEARCH_RECORDS_STATE_FAILED;
		}
		//PRINT("search element length:%d\n",sizeof(SEARCH_ELEMENT));
		//searchElement = searchElement + sizeof(SEARCH_ELEMENT);
		wPt += sizeof(SEARCH_ELEMENT);

	}

	int Info;
	PC_CMD_DATA pcCmdData;
	//PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_SEARCH_ALLTAG_TRANS);
	int j = 0;

	PSEARCH_ELEMENT pSearchElement,pSearchOrignal;
	searchElement = head; //置回头指针
	pSearchElement = (PSEARCH_ELEMENT)malloc(300);
	memset(pSearchElement,0,300);
	pSearchOrignal = pSearchElement;
	for (DWORD i=0; i < elementNums; i++)
	{
		*pSearchElement = *searchElement;
		memcpy(pSearchElement->dataBuf,searchElement->dataBuf,searchElement->dataLen);
		char* tp = (char*)&(pSearchElement->dataBuf) + pSearchElement->dataLen;
		pSearchElement = (PSEARCH_ELEMENT)tp;
		searchElement++;
	}
	j = (unsigned char*)pSearchElement - (unsigned char*)pSearchOrignal;
	pcCmdData.uParam.SEARCH_RECORDS.dataLen = j;
	memcpy(pcCmdData.uParam.SEARCH_RECORDS.dataBuf,pSearchOrignal,80);
	PRINT ("查询元素为:%x\n",pcCmdData.uParam.SEARCH_RECORDS.dataBuf);
	PRINT ("查询长度为:%d\n",j);
	Info = send_cmd(&pcCmdData);
	Sleep(2000);
	PRINT ("命令发送完成");
	return IF_TYPE_SEARCH_RECORDS | SEARCH_RECORDS_STATE_SUCCESS;
}

/******************************************************
*函数名称：rfUpdatePwd
*函数功能：更新系统口令
*函数参数：
*返回状态：
*备注:
*/
RFIDLIBDLL RF_STATUS rfUpdatePwd(IN  DWORD	adminPWD,	//提供管理员密码
								 IN  WORD	indexPWD,	//口令索引
								 IN  BYTE	mode,		//index为文件标示符时使用，区分密码类别
								 IN  DWORD	newPWD		//新密码
								 )
{
	if ((indexPWD>4) && (indexPWD<16128))
	{
		PRINT ("更新系统口令口令索引或者模式不合法\n");
		return IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_INVALID_INDEX_OR_MODE;
	}
	if ((indexPWD>=16128) && ((mode<1)||(mode>4)))
	{
		PRINT ("更新系统口令口令索引或者模式不合法\n");
		return IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_INVALID_INDEX_OR_MODE;
	}

	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_UPDATE_PWD);
	pcCmdData.uParam.UPDATE_PWD.adminPWD = adminPWD;
	pcCmdData.uParam.UPDATE_PWD.indexPWD = indexPWD;
	pcCmdData.uParam.UPDATE_PWD.mode = mode;
	pcCmdData.uParam.UPDATE_PWD.newPWD = newPWD;
	memcpy(pcCmdData.uParam.UPDATE_PWD.Tid, TID, strlen(TID));

	int Info = send_cmd(&pcCmdData);

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_UPDATE_PWD, 3*TIME_OUT))
	{
		PRINT ("DispatchCMD 更新系统口令超时\n");
		return IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_TIMEOUT;
	}
	switch (pcCmdReply.uReplyParam.updatePWDParam.upDatePwdState)
	{
	case UPDATE_PWD_STATE_SUCCESS:
		PRINT ("更新系统口令成功\n");
		return IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_SUCCESS;
	case UPDATE_PWD_STATE_FAILED:
		PRINT ("更新系统口令失败\n");
		return IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_FAILED;
	case UPDATE_PWD_STATE_TIMEOUT:
		PRINT ("更新系统口令超时\n");
		return IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_TIMEOUT;
	case UPDATE_PWD_STATE_ADMIN_PWD_NOT_RIGHT:
		PRINT ("更新系统口令管理员密码不匹配\n");
		return IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_ADMIN_PWD_NOT_RIGHT;
	case UPDATE_PWD_STATE_INVALID_NEW_PWD:
		PRINT ("更新系统口令新口令不合法\n");
		return IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_INVALID_NEW_PWD;
	case UPDATE_PWD_STATE_NOT_AUTHORISED:
		PRINT ("更新系统口令不在安全会话\n");
		return IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_NOT_AUTHORISED;
	case UPDATE_PWD_STATE_UNKNOWN:
		PRINT ("更新系统口令未知错误\n");
		return IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_UNKNOWN;
	}
	return 0;
}

/******************************************************
*函数名称：rfMonitorTrConf
*函数功能：监测配置命令
*函数参数：
*返回状态：
*备注:
*/
RFIDLIBDLL RF_STATUS rfMonitorTrConf(IN	 WORD	fileID,			 //文件ID
									 IN	 BYTE	fileOffset,		 //文件偏移
									 IN	 BYTE	dataLen,		 //数据长度
									 IN	 WORD	timeSlotNum,	 //时隙序号
									 IN	 WORD	reportedCycle,	 //上报周期
									 IN  WORD	monitorCycle,	 //监测周期
									 IN	 WORD	timeSlotWidth,	 //时隙宽度
									 IN	 WORD	timeSlotSum,	 //传输时隙总数
									 IN	 BYTE	reportIndication, //收发指示
									 IN  BYTE   *tmpTid
									 )
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	memset(&pcCmdReply, 0, sizeof(pcCmdReply));
	initCmdData(pcCmdData, PC_CMD_TYPE_MONITOR_CONF);

	pcCmdData.uParam.MONITOR_TRCONF_PARAM.FileId = fileID;
	pcCmdData.uParam.MONITOR_TRCONF_PARAM.FileOffset = fileOffset;
	pcCmdData.uParam.MONITOR_TRCONF_PARAM.DataLen = dataLen;
	pcCmdData.uParam.MONITOR_TRCONF_PARAM.TimeSlotNum = timeSlotNum;
	pcCmdData.uParam.MONITOR_TRCONF_PARAM.ReportedCycle = reportedCycle;
	pcCmdData.uParam.MONITOR_TRCONF_PARAM.MonitorCycle = monitorCycle;
	pcCmdData.uParam.MONITOR_TRCONF_PARAM.TimeSlotWidth = timeSlotWidth;
	pcCmdData.uParam.MONITOR_TRCONF_PARAM.TimeSlotSum = timeSlotSum;
	pcCmdData.uParam.MONITOR_TRCONF_PARAM.ReportIndication = reportIndication;
	memcpy(pcCmdData.uParam.MONITOR_TRCONF_PARAM.Tid, tmpTid, 8);
	
	int Info = send_cmd(&pcCmdData);
	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_MONITOR_CONF, 3*TIME_OUT))
	{
		PRINT ("DispatchCMD 监测周期配置超时\n");
		return IF_TYPE_MONITOR_CONF | CONF_MONITOR_TIMEOUT;
	}
	switch (pcCmdReply.uReplyParam.confMonParam.confMonState)
	{
	case CONF_MONITOR_SUCCESS:
		PRINT ("监测周期配置成功\n");
		return IF_TYPE_MONITOR_CONF | CONF_MONITOR_SUCCESS;

	case CONF_MONITOR_NOT_MATCH:
		PRINT ("监测周期配置__传感器标识符不匹配\n");
		return IF_TYPE_MONITOR_CONF | CONF_MONITOR_NOT_MATCH;
	case CONF_MONITOR_OVERRANGE:
		PRINT ("监测周期配置数据越界\n");
		return IF_TYPE_MONITOR_CONF | CONF_MONITOR_OVERRANGE;
	case CONF_MONITOR_FAILED:
		PRINT ("监测周期配置失败\n");
		return IF_TYPE_MONITOR_CONF | CONF_MONITOR_FAILED;
	case CONF_MONITOR_TIMEOUT:
		PRINT ("监测周期配置超时\n");
		return IF_TYPE_MONITOR_CONF | CONF_MONITOR_TIMEOUT;
	default:
		PRINT ("监测周期配置未知错误\n");
		return IF_TYPE_MONITOR_CONF | CONF_MONITOR_UNKNOWN;
	}
}

/******************************************************
*函数名称：rfMonitorTrStart
*函数功能：监测起始命令
*函数参数：
*返回状态：
*备注:
*/
RFIDLIBDLL RF_STATUS rfMonitorTrStart()
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_MONITOR_START);

	int Info = send_cmd(&pcCmdData);
	if (0 != Info)
	{
		PRINT ("监测起始命令发送错误！\n");
		return IF_TYPE_MONITOR_START | MONITOR_START_STATE_TIMEOUT;
	}
	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_MONITOR_START, 3*TIME_OUT))
	{
		PRINT("监测起始命令回复超时\n");
		return IF_TYPE_MONITOR_START| MONITOR_START_STATE_TIMEOUT;
	}
	switch(pcCmdReply.uReplyParam.monitorStartParam.monitorStartState)
	{
	case MONITOR_START_STATE_SUCCESS:
		PRINT ("监测起始成功\n");
		return IF_TYPE_MONITOR_START | MONITOR_START_STATE_SUCCESS;
	case MONITOR_START_STATE_TIMEOUT:
		PRINT ("监测起始超时\n");
		return IF_TYPE_MONITOR_START| MONITOR_START_STATE_TIMEOUT;
	}
	return 0;
}

/******************************************************
*函数名称：rfMonitorTrStop
*函数功能：监测停止命令
*函数参数：
*返回状态：
*备注:
*/
RFIDLIBDLL RF_STATUS rfMonitorTrStop(IN	 WORD  monitorCycle)	//监测周期（单位：秒）
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_MONITOR_STOP);

	int Info = send_cmd(&pcCmdData);
	if (0 != Info)
	{
		PRINT ("监测停止命令发送错误！\n");
		return IF_TYPE_MONITOR_STOP | MONITOR_STOP_STATE_TIMEOUT;
	}
	Sleep(monitorCycle*1000);
	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_MONITOR_STOP, 3*TIME_OUT))
	{
		PRINT("监测停止命令回复超时\n");
		return IF_TYPE_MONITOR_STOP| MONITOR_STOP_STATE_TIMEOUT;
	}
	switch(pcCmdReply.uReplyParam.monitorStopParam.monitorStopState)
	{
	case MONITOR_STOP_STATE_SUCCESS:
		PRINT ("监测停止成功\n");
		return IF_TYPE_MONITOR_STOP | MONITOR_STOP_STATE_SUCCESS;
	case MONITOR_STOP_STATE_TIMEOUT:
		PRINT ("监测停止超时\n");
		return IF_TYPE_MONITOR_STOP| MONITOR_STOP_STATE_TIMEOUT;
	}
	return 0;
}

/******************************************************
*函数名称：rfMonitorTrReport
*函数功能：监测周期上报
*函数参数：
*返回状态：
*备注:
*/
RFIDLIBDLL RF_STATUS rfMonitorTrReport(IN  DWORD		waitTime,  //s
									   OUT CYCLE_INFO*	CycleInfo
									   )
{
	if (CycleInfo == NULL)
	{
		PRINT("CycleInfo为空指针\n");
		return IF_TYPE_MONITOR_REPORT | GET_MONITOR_REPORT_PARAM_ERROR;
	}

	PC_CMD_REPLY pcCmdReply;
	memset(&pcCmdReply,0,sizeof(PC_CMD_REPLY));

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_MONITOR_REPORT, 10*TIME_OUT))
	{
		PRINT ("监测上报读数据超时！\n");
		return IF_TYPE_MONITOR_REPORT | GET_MONITOR_REPORT_TIMEOUT;
	}

	switch(pcCmdReply.uReplyParam.MonRepParam.State)
	{
	case GET_MONITOR_REPORT_SUCCESS:
		*CycleInfo = pcCmdReply.uReplyParam.MonRepParam.cycleInfo;
		break;
	case GET_MONITOR_REPORT_TIMEOUT:
		PRINT ("无上报数据标签！\n");
		*CycleInfo = pcCmdReply.uReplyParam.MonRepParam.cycleInfo;
		return IF_TYPE_MONITOR_REPORT | GET_MONITOR_REPORT_TIMEOUT;
	}
	PRINT ("监测上报成功\n");
	return IF_TYPE_MONITOR_REPORT | GET_MONITOR_REPORT_SUCCESS;
}

/******************************************************
*函数名称：rfUpdateAdminPwd
*函数功能：更新管理员密码
*函数参数：
*返回状态：
*备注:
*/
RFIDLIBDLL RF_STATUS rfUpdateAdminPwd(IN  DWORD  oldPWD, //旧密码
									  IN  DWORD  newPWD	 //新密码
									  )
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_UPDATE_ADMIN_PWD);
	pcCmdData.uParam.UPDATE_ADMIN_PWD.oldPWD = oldPWD;
	pcCmdData.uParam.UPDATE_ADMIN_PWD.newPWD = newPWD;

	int Info = send_cmd(&pcCmdData);
	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_UPDATE_ADMIN_PWD, 3*TIME_OUT))
	{
		PRINT ("DispatchCMD 更新管理员密码超时\n");
		return IF_TYPE_UPDATE_ADMIN_PWD | UPDATE_ADMIN_PWD_STATE_TIMEOUT;
	}
	switch (pcCmdReply.uReplyParam.updateAdminPwdParam.updateAdminPwdState)
	{
	case UPDATE_ADMIN_PWD_STATE_SUCCESS:
		PRINT("更新管理员密码成功\n");
		return IF_TYPE_UPDATE_ADMIN_PWD | UPDATE_ADMIN_PWD_STATE_SUCCESS;
	case UPDATE_ADMIN_PWD_STATE_FAILED:
		PRINT("更新管理员密码失败\n");
		return IF_TYPE_UPDATE_ADMIN_PWD | UPDATE_ADMIN_PWD_STATE_FAILED;
	case UPDATE_ADMIN_PWD_STATE_TIMEOUT:
		PRINT("更新管理员密码超时\n");
		return IF_TYPE_UPDATE_ADMIN_PWD | UPDATE_ADMIN_PWD_STATE_TIMEOUT;
	case UPDATE_ADMIN_PWD_STATE_NOT_AUTHORISED:
		PRINT("更新管理员密码不在安全会话下\n");
		return IF_TYPE_UPDATE_ADMIN_PWD | UPDATE_ADMIN_PWD_STATE_NOT_AUTHORISED;
	case UPDATE_ADMIN_PWD_STATE_PWD_ERROR:
		PRINT("更新管理员密码旧密码错误\n");
		return IF_TYPE_UPDATE_ADMIN_PWD | UPDATE_ADMIN_PWD_STATE_PWD_ERROR;
	}
	return 0;
}

/******************************************************
*函数名称：rfGetTagSafeParam
*函数功能：获取标签安全参数
*函数参数：
*返回状态：
*备注:
*/
RFIDLIBDLL RF_STATUS rfGetTagSafeParam(OUT  WORD*  keyParam,		//密钥参数
									   OUT  WORD*  safePowerParam	//安全能力参数
									   )
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_GET_TAG_SAFE_PARAM);

	int Info = send_cmd(&pcCmdData);
	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_GET_TAG_SAFE_PARAM, 3*TIME_OUT))
	{
		PRINT("DispatchCMD 获取标签安全参数超时\n");
		return IF_TYPE_GET_TAGE_SAFE_PARAM | GET_TAG_SAFE_PARAM_STATE_TIMEOUT;
	}
	switch(pcCmdReply.uReplyParam.getTagSafeParam.getTagSafeParamState)
	{
	case GET_TAG_SAFE_PARAM_STATE_SUCCESS:
		PRINT("获取标签安全参数成功\n");
		*keyParam = pcCmdReply.uReplyParam.getTagSafeParam.keyParam;
		*safePowerParam = pcCmdReply.uReplyParam.getTagSafeParam.safePowerParam;
		return IF_TYPE_GET_TAGE_SAFE_PARAM | GET_TAG_SAFE_PARAM_STATE_SUCCESS;
	case GET_TAG_SAFE_PARAM_STATE_FAILED:
		PRINT("获取标签安全参数失败\n");
		return IF_TYPE_GET_TAGE_SAFE_PARAM | GET_TAG_SAFE_PARAM_STATE_FAILED;
	default:
		PRINT("获取标签安全参数未知错误\n");
		return IF_TYPE_GET_TAGE_SAFE_PARAM | pcCmdReply.uReplyParam.getTagSafeParam.getTagSafeParamState;
	}
}

/******************************************************
*函数名称：rfRequireRandom
*函数功能：请求随机数
*函数参数：
*返回状态：
*备注:
*/
RFIDLIBDLL RF_STATUS rfRequireRandom(IN  BYTE randomLenth,	 //随机数长度（传1-4）
									 OUT BYTE randomNums[8]	 //随机数
									 )
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_REQUIRE_RANDOM);
	pcCmdData.uParam.REQUIRE_RANDOM_PARAM.randomLenth = randomLenth;

	int Info = send_cmd(&pcCmdData);

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_REQUIRE_RANDOM, 3*TIME_OUT))
	{
		PRINT("DispatchCMD 请求随机数超时\n");
		return IF_TYPE_REQUIRE_RANDOM | REQUIRE_RANDOM_STATE_TIMEOUT;
	}
	switch(pcCmdReply.uReplyParam.requireRandomParam.requireRandomState)
	{
	case REQUIRE_RANDOM_STATE_SUCCESS:
		PRINT("请求随机数成功\n");
		memcpy(randomNums,pcCmdReply.uReplyParam.requireRandomParam.randomNums,8);
		return IF_TYPE_REQUIRE_RANDOM | REQUIRE_RANDOM_STATE_SUCCESS;
	case REQUIRE_RANDOM_STATE_FAILED:
		PRINT("请求随机数失败\n");
		return IF_TYPE_REQUIRE_RANDOM | REQUIRE_RANDOM_STATE_FAILED;
	default:
		PRINT("请求随机数未知错误\n");
		return IF_TYPE_REQUIRE_RANDOM | pcCmdReply.uReplyParam.requireRandomParam.requireRandomState;
	}
}

//数据广播
RFIDLIBDLL RF_STATUS rfDataBoardcast( IN  DWORD readyToken,				//就绪口令
									  IN  BYTE  boardcastData[100]		//广播数据
									  )
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_DATA_BOARDCAST);
	pcCmdData.uParam.DATA_BOARDCAST_PARAM.readyToken = readyToken;
	memcpy(pcCmdData.uParam.DATA_BOARDCAST_PARAM.boardcastData,boardcastData,100);

	int Info = send_cmd(&pcCmdData);

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_DATA_BOARDCAST, 5*TIME_OUT))
	{
		PRINT("DispatchCMD 数据广播超时\n");
		return IF_TYPE_DATA_BOARDCAST | DATA_BOARDCAST_STATE_TIMEOUT;
	}
	switch(pcCmdReply.uReplyParam.dataBoardcastParam.dataBoardcastState)
	{
	case DATA_BOARDCAST_STATE_SUCCESS:
		PRINT("数据广播成功\n");
		return IF_TYPE_DATA_BOARDCAST | DATA_BOARDCAST_STATE_SUCCESS;
	default:
		PRINT("数据广播未知错误");
		return IF_TYPE_DATA_BOARDCAST | pcCmdReply.uReplyParam.dataBoardcastParam.dataBoardcastState;
	}
}

/*
若文件存在，则读取文件内容
*/
RFIDLIBDLL RF_STATUS rfWriteOrReadFile( IN	WORD fileId,
										IN	BYTE writeBuffer[10],
										OUT	BYTE readBuffer[10]
										)
{
	int info, datalen;
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData, PC_CMD_TYPE_SEARCH_FILE);
	pcCmdData.uParam.WRITE_OR_READ_FILE.fileId = fileId;
	memcpy(pcCmdData.uParam.WRITE_OR_READ_FILE.writeBuffer, writeBuffer, sizeof(writeBuffer));
	info = send_cmd(&pcCmdData);

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_SEARCH_FILE,INFINITE))
	{
		PRINT("读写指定ID文件错误\n");
		return IF_TYPE_SEARCH_FILE | SEARCH_FILE_STATE_TIMEOUT;
	}
	datalen = sizeof(pcCmdReply.uReplyParam.searchFileParam.readBuffer);
	switch(pcCmdReply.uReplyParam.searchFileParam.RepState)
	{
	case SEARCH_FILE_STATE_SUCESS:
		PRINT("读写指定ID文件成功");
		memcpy(readBuffer, pcCmdReply.uReplyParam.searchFileParam.readBuffer, datalen);
		return IF_TYPE_SEARCH_FILE | SEARCH_FILE_STATE_SUCESS;
	case SEARCH_FILE_STATE_FAILD:
		PRINT("读写指定ID文件失败");
		return IF_TYPE_SEARCH_FILE | SEARCH_FILE_STATE_FAILD;
	default:
		return IF_TYPE_SEARCH_FILE | SEARCH_FILE_STATE_TIMEOUT;
	}
}
