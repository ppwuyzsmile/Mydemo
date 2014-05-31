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

GLOBAL  HANDLE  hCom; //��usbͨ�������ڶ�д�ļ��ľ��ȫ�ֱ���
GLOBAL  SOCKET	sockSrv; //����ͨ���׽���ȫ�ֱ���
GLOBAL  BYTE  CommunicationFlag;
GLOBAL	BOOL  isTIDselected = FALSE;
GLOBAL	char  TID[8]; //���ڱ��浱ǰѡ���TID
GLOBAL	PWD_STATE  pwdState = {false, false, false, false, false}; //List  Vaild Read  Selct
GLOBAL	unsigned short  listFileID[100]; //�б��ļ������ڱ��������ļ�ID������
GLOBAL	DWORD  listFileNums; //���ڱ����б��ļ��ĸ���
GLOBAL	FILE_ATTRIBUTE_R  selectedFile;	//���ڱ��浱ǰѡ����ļ�
GLOBAL	DWORD  pubPwd;

//-------------------------------------

RF_STATUS SearchHidDevice()	//Ѱ�ҿ�����ͨ�ŵ�USB�豸
{
	// ���ұ�ϵͳ��HID���GUID��ʶ
	PRINT("���ڲ���ϵͳ��HID���GUID��ʶ......\n");
	GUID guidHID;
	HidD_GetHidGuid(&guidHID); //ȡ��HID�豸�Ľӿ���GUID
	PRINT("HID���GUID��ʶΪ......\n");

	PRINT("%08x-%04x-%04x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\n",
		guidHID.Data1,guidHID.Data2,guidHID.Data3,guidHID.Data4[0],
		guidHID.Data4[1],guidHID.Data4[2],guidHID.Data4[3],guidHID.Data4[4],
		guidHID.Data4[5],guidHID.Data4[6],guidHID.Data4[7]);

	// ׼�����ҷ���HID�淶��USB�豸
	HDEVINFO hDevInfo = SetupDiGetClassDevs(&guidHID,NULL,0,   //�õ�HID�豸����Ϣ���˺���������guidHIDָ���������豸��һ����Ϣ���ϵľ����
		DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (hDevInfo == INVALID_HANDLE_VALUE)
	{
		PRINT("����USB�豸����......\n");
		return 0;
	}

	// ����USB�豸�ӿ�
	/*UINT nIndex = 0;*/
	PRINT("���ڲ��ҿ��õ�USB�豸......\n");
	SP_DEVICE_INTERFACE_DATA strtInterfaceData;
	strtInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);//by ppwu
	UINT index = 0;
	for (int i=0; i<10; i++)
	{
		BOOL bSuccess = SetupDiEnumDeviceInterfaces(hDevInfo,NULL,&guidHID,index++,&strtInterfaceData);//��ȡһ���豸�ӿ���Ϣ�ĺ������˺������Դ��豸�����л�ȡĳ���豸�ӿ���Ϣ
		if (!bSuccess)
		{
			PRINT("��Ǹ��δ�ҵ����õ�USB�豸......\n");
			SetupDiDestroyDeviceInfoList(hDevInfo);//����һ���豸��Ϣ���ϣ������ͷ����й������ڴ�
			break;
		}
		else
		{
			if (strtInterfaceData.Flags == SPINT_ACTIVE)
			{
				DWORD strSize=0, requiesize=0;
				PSP_DEVICE_INTERFACE_DETAIL_DATA strtDetailData;

				SetupDiGetDeviceInterfaceDetail(hDevInfo,&strtInterfaceData,NULL,0,&strSize,NULL);//��ȡһ��ָ���豸�ӿڵ���ϸ��Ϣ
				requiesize = strSize;

				strtDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(requiesize);
				strtDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

				if (!SetupDiGetDeviceInterfaceDetail(hDevInfo,&strtInterfaceData,
					strtDetailData,strSize,&requiesize,NULL))
				{
					PRINT("�����豸·��ʱ����!\n");
					SetupDiDestroyDeviceInfoList(hDevInfo);//����һ���豸��Ϣ���ϣ������ͷ����й������ڴ�
					break;
				}

				PRINT("�豸��·����%s\n",strtDetailData->DevicePath);
				PRINT("���豸·��......\n");

				hCom = CreateFile(strtDetailData->DevicePath,
					GENERIC_READ | GENERIC_WRITE,
					FILE_SHARE_READ | FILE_SHARE_WRITE,
					NULL,
					OPEN_EXISTING,
					FILE_FLAG_OVERLAPPED,
					NULL);
				if (hCom == INVALID_HANDLE_VALUE)
				{
					PRINT("�޷���ͨ�Ŷ˿�......\n");
					continue;
					//SetupDiDestroyDeviceInfoList(hDevInfo);//����һ���豸��Ϣ���ϣ������ͷ����й������ڴ�
					//break;
				}

				// ��ѯ�豸��ʶ
				HIDD_ATTRIBUTES strtAttrib;
				PRINT("���ڶ�ȡ�豸�ı�ʶ......\n");

				if (!HidD_GetAttributes(hCom, &strtAttrib))	//ȡ��VID��PID����Ϣ
				{
					PRINT("��ѯ�豸״̬ʱ����!\n");
					CloseHandle(hCom);
					SetupDiDestroyDeviceInfoList(hDevInfo);//����һ���豸��Ϣ���ϣ������ͷ����й������ڴ�
					break;
				}
				PRINT("��ȡ�豸ID�ɹ�!\n");

				CommunicationFlag = FALSE;//����USB�շ����ݱ�ʶλ

				// ��ʾ��Ӧ�̱�ʶ
				PRINT("0x%x\n",strtAttrib.VendorID);

				// ��ʾ��Ʒ��ʶ
				PRINT("0x%x\n",strtAttrib.ProductID);

				// ��ʾ��Ʒ�汾��
				PRINT("%d\n",strtAttrib.VersionNumber);

				//index++;
				SetupDiDestroyDeviceInfoList(hDevInfo);
				return 1;

				//�ͷ���Դ
				//CloseHandle(hCom);
				//SetupDiDestroyDeviceInfoList(hDevInfo);
			}
		}
	}
	return 0;
}

void CloseHidDevice()
{
	CommunicationFlag = 3;//�ر�ͨ�ű�ʶλ
	CloseHandle(hCom);
}

static int SendDataToUsb(IN char *VID,
				         IN char *PID,
				         IN PC_CMD_DATA *SendData)	//��USB��������
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
		OVERLAPPED	WriteOverLapped = {0};	//�˴�ע��Ҫ��ʼ��OVERLAPPED������ᱨ����6

		WriteOverLapped.hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
// 		HidD_GetPreparsedData(hCom, &preparsedData);
// 		HidP_GetCaps(preparsedData, &hidCapabilities);			//�������ȡ����������������ȣ�OutputReportByteLength = 64

		sendLen = sizeof(PC_CMD_DATA);
		if (sendLen <= CHAR_NUM-4)
		{
			MULTI_PACKET_SEND_RECV sendPacket;
			sendPacket.usbFlag = 0;//USB�������ݱ�ʶλ������Ϊ0
			sendPacket.pkgNUM = 0;//��ʾ�ְ����
			sendPacket.endFlag = 1;//��ʾ���ݰ����ͽ�����־��0��ʾδ�����꣬1��ʾ�������
			sendPacket.dataLen = sendLen;

			memcpy(sendPacket.dataBuf, SendData, sendLen);
			WriteFile(hCom, &sendPacket, 64, NULL, &WriteOverLapped);           //��Ϊ��������������Ⱦ���64�����Ͽ�ͷ��ReportID���������ݳ���
			if (WaitForSingleObject(WriteOverLapped.hEvent, TIME_OUT) != WAIT_OBJECT_0) //��ʱ�ж�
			{																			//����63������������֮ǰ���ģ����ݳ���64����һ��ReportID��һ��65��
				PRINT("дUSB����ʧ��\n");													//��Ϊʹ�����첽������Ҫ�ȴ���������¼��Ĳ���
				return -1;
			}

			bResult = GetOverlappedResult(hCom, &WriteOverLapped, &nBytesSend, FALSE);	//�����ж��Ƿ��ͳɹ���ʹ��������������۲�bResult�Ƿ�Ϊ0����Ϊ0����˵������ʧ�ܣ������ˣ�����GetLastError
			if (!bResult && nBytesSend == 0)											//��Ϊ0���ҹ۲����1����˵�����ͳɹ�����nBytesSend(��ʱ����ֵΪ64)Ҳ�ɵ�֪�����ǳɹ���
			{																			//��ʱ��Ӧ�õ���GetLastError��������õĻ����������997����I/O���ڷ��ͣ�,����ʵ�Ƿ��ͳɹ��˵�.
				PRINT("�������ݴ��󣡣���\n");
				return -1;
			}
			PRINT("���ͳɹ�������Ϊ��%s\n", &sendPacket.dataBuf);
			return 0;
		}

		else if (sendLen > CHAR_NUM-4)//�ְ�����
		{
			int bagNUM = 0;
			MULTI_PACKET_SEND_RECV sendPacket;

			while (sendLen > 0)
			{
				sendPacket.usbFlag = 0;//USB�������ݱ�ʶλ������Ϊ0
				sendPacket.pkgNUM = bagNUM++;//��ʾ�ְ����
				sendPacket.dataLen = ((sendLen/60)>0) ? 60:(sendLen%60);

				if (sendLen/60 > 0)
				{
					sendPacket.endFlag = 0;//��ʾ���ݰ����ͽ�����־��0��ʾδ�����꣬1��ʾ�������
					memcpy(sendPacket.dataBuf, ((char*)SendData) + ((bagNUM-1)*60), 60);
					sendLen -= 60;
				}
				else
				{
					sendPacket.endFlag = 1;//�ڶ����ֽ���Ϊ1��ʾ�ְ����ͽ���
					memcpy(sendPacket.dataBuf, ((char*)SendData) + ((bagNUM-1)*60), sendPacket.dataLen);
					sendLen -= sendPacket.dataLen;
				}

				WriteFile(hCom, &sendPacket, 64, NULL, &WriteOverLapped);
				if (WaitForSingleObject(WriteOverLapped.hEvent, TIME_OUT) != WAIT_OBJECT_0) //��ʱ�ж�
				{
					PRINT("дUSB����ʧ��\n");
					return -1;
				}

				bResult = GetOverlappedResult(hCom, &WriteOverLapped, &nBytesSend, FALSE);
				if (!bResult && nBytesSend == 0)
				{
					PRINT("�������ݴ��󣡣���\n");
					return -1;
				}
				//Sleep(5);
			}
			PRINT("���ͳɹ�������Ϊ��%s\n", sendPacket.dataBuf);
			return 0;
		}
	}
	return -1;
}

static int ReceiveDataFromUsb(IN  char  *VID,
							  IN  char  *PID,
						      IN  WORD  numTimeOut,
							  IN  PC_CMD_TYPE  pcCmdType,
						      OUT PC_CMD_REPLY*  RecvData)	//����USB����
{
	char recvBuf[256]; //���ջ�����
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

		memset(&ReadOverLapped, 0, sizeof(OVERLAPPED));	//�˴�ע��Ҫ��ʼ��OVERLAPPED������ᱨ����6
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
			if (WaitForSingleObject(ReadOverLapped.hEvent, TIME_OUT*8) == WAIT_TIMEOUT) //��ʱ�ж�
			{
				PRINT("��ȡUSB����ʧ��\n");
				return 0;
			}

			bResult = GetOverlappedResult(hCom, &ReadOverLapped, &nBytesGet, FALSE);
			if (!bResult && nBytesGet == 0)
			{
				PRINT("��������ʧ�ܣ�����\n");
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
			PRINT("��������֡���ԣ�����\n");
			return 0;
		}
		PRINT("���ճɹ�\n");
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
		PRINT("��ʼ��ʧ�ܣ�\n");
		return 0;
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion != 2))
	{
		WSACleanup();
		return 0;
	}

	//�������ڼ������׽���
	sockSrv = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == sockSrv)
	{
		PRINT("Socket ����ʧ�ܣ�Exit!\n");
		return 0;
	}

	WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	//���÷��ͳ�ʱ
// 	struct timeval sendtimeout;
// 	sendtimeout.tv_sec = 10;//��
// 	sendtimeout.tv_usec = 10;//΢��
	int sendtimeout = TIME_OUT*5; //5s
	if(setsockopt(sockSrv, SOL_SOCKET, SO_SNDTIMEO, (char*)&sendtimeout, sizeof(int)) == SOCKET_ERROR)
	{
		PRINT("set send timeout failed\n");
		return 0;
	}

	//���ý��ճ�ʱ
// 	struct timeval recvtimeout;
// 	recvtimeout.tv_sec = 20;//��
// 	recvtimeout.tv_usec = 10;//΢��
	int recvtimeout = TIME_OUT*20; //20s
	if(setsockopt(sockSrv, SOL_SOCKET, SO_RCVTIMEO, (char*)&recvtimeout, sizeof(int)) == SOCKET_ERROR)
	{
		PRINT("set recv timeout failed\n");
		return 0;
	}

	//start
	/*���ڸ����������ݻ��������Ƿ��б�Ҫ�ĳɷ�����ģʽ*/
	//end

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr(adressIP);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(m_Port);//��ͨ�û�ע��˿ںŷ�Χ1024~49151

	if (connect(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) != 0)
	{
		PRINT("����ʧ��\n");
		return 0;
	}

	PRINT("���ӳɹ����ȴ��շ�����......\n");
	CommunicationFlag = TRUE;//���������շ����ݱ�ʶλ

	return 1;
}

void CloseConnect()
{
	CommunicationFlag = 3;
//	sockSrv = INVALID_SOCKET;
// 	BOOL bDontLinger = FALSE; //����closesocket���������ȴ�TIME_WAIT��ʱʱ��󷵻�
// 	setsockopt(sockSrv, SOL_SOCKET, SO_DONTLINGER, (const char*)&bDontLinger, sizeof(BOOL));

	BOOL bReuseaddr=TRUE; //����closesocket�������ȴ�TIME_WAIT��ʱʱ��󷵻�
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
// 	FD_ZERO(&wfds); //��ռ���  
// 	FD_SET(sockSrv, &wfds); //��fp��ӵ����ϣ������FD_ISSET��FD_SETû�б�Ȼ��ϵ����������Ӽ��
// 	int RetInfo = select(0, NULL, &wfds, NULL, &SelectTimeout);
// 	if(RetInfo <= 0)
// 	{
// 		//�ڵȴ�ʱ������У�û���������ݿɶ�����ʱ���ء�
// 		return 0;
// 	}

	char sendBuf[SEND_TCP_DATA];
	int datalen = sizeof(PC_CMD_DATA);
	memcpy(sendBuf, SendData, datalen);

	int Ret = send(sockSrv, sendBuf, datalen, 0);
	//int ret = WSAGetLastError();
	if (Ret == SOCKET_ERROR)
	{
		PRINT("���͵�SOCKET����ʧ�ܻ��߷��ͳ�ʱ��\n");
		return -1;
	}

	PRINT("���͵�SOCKET����Ϊ��%s\n", sendBuf);
	return 0;
}

static int RceiveDataFromSocket(IN  WORD           TimeOut,
								IN  PC_CMD_TYPE    pcCmdType,
								OUT PC_CMD_REPLY   *RecvData)
{
	if(sockSrv == INVALID_SOCKET) //�׽����ѹر�
		return 0;

	struct fd_set rfds;
	struct timeval SelectTimeout;
	SelectTimeout.tv_sec = TimeOut / TIME_OUT; //��λΪ��s
	SelectTimeout.tv_usec = 0; //��λΪ��ms

	FD_ZERO(&rfds); //��ռ���  
	FD_SET(sockSrv, &rfds); //��fp��ӵ����ϣ������FD_ISSET��FD_SETû�б�Ȼ��ϵ����������Ӽ��
	int RetInfo = select(0, &rfds, NULL, NULL, &SelectTimeout); //�������óɹ��򷵻��׽����������ܺͣ������ʱ�򷵻�0��ʧ���򷵻�SOCKET_ERROR��
	if(RetInfo <= 0)
	{
		//�ڵȴ�ʱ������У�û���������ݿɶ�����ʱ���ء�
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
		PRINT("����SOCKET���ݳ�ʱ��\n");
		return 0;
	}

// 	while (nRecvLen < Length)  //�������绷������һ�ν��ղ���һ�������İ����˴��������Դ���
// 	{
// 		int RetLen = recv(sockSrv, recvBuf+nRecvLen, 1024, 0);
// 		if (RetLen == SOCKET_ERROR)
// 		{
// 			PRINT("����SOCKET���ݳ�ʱ��\n");
// 			return 0;
// 		}
// 		nRecvLen += RetLen;
// 	}

	memcpy(RecvData, recvBuf, sizeof(PC_CMD_REPLY));
	if (RecvData->cmdType != pcCmdType ||
		RecvData->SIG_START != SIG_START_NUM ||
		RecvData->SIG_END != SIG_END_NUM )
	{
		PRINT("�����������Ͳ���\n");
		return 0;
	}

	PRINT("���յ���SOCKET����Ϊ��%s\n", recvBuf);
	return 1;
}


static int dispatch_cmd(PC_CMD_REPLY *pcCmdReply, PC_CMD_TYPE pcCmdType, unsigned int TimeOut)  //��װ������
{
	int Ret;

	if (CommunicationFlag == FALSE)  //��USB�ڽ�������
	{
		char *VID = "0x1fc9";
		char *PID = "0x107";
		Ret = ReceiveDataFromUsb(VID, PID, TimeOut, pcCmdType, pcCmdReply);
	}

	if (CommunicationFlag == TRUE)  //�����ڽ�������
	{
		Ret = RceiveDataFromSocket(TimeOut, pcCmdType, pcCmdReply);
	}

	return Ret;//����ֵΪ1�ɹ�������ֵΪ0ʧ��
}


static int send_cmd(PC_CMD_DATA* pData)  //��װ������
{
	int Ret;

	if (CommunicationFlag == FALSE)//Ϊ0��USB�������ݣ�Ϊ1�����ڴ�������
	{
		char *VID = "0x1fc9";
		char *PID = "0x107";
		Ret = SendDataToUsb(VID, PID, pData);
	}

	if (CommunicationFlag == TRUE)//�����ڴ�������
	{
		//��������ǰ����ս��ջ�������������
		PC_CMD_REPLY pcCmdReply;
		memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
		dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_COLLECT, 100); //�����������ȡ
		
		Ret = SendDataToSocket(pData); //����ֵΪ0��ʾ�ɹ���������0��ʾʧ��
	}

	return Ret;//����ֵΪ0�ɹ�������ֵΪ-1ʧ��
}


RF_STATUS TestSendCmd(PC_CMD_DATA* pData)
{
	int Ret;

	if (CommunicationFlag == FALSE)//Ϊ0��USB�������ݣ�Ϊ1�����ڴ�������
	{
		char *VID = "0x1fc9";
		char *PID = "0x107";
		Ret = SendDataToUsb(VID, PID, pData);
	}

	if (CommunicationFlag == TRUE)//�����ڴ�������
	{
		Ret = SendDataToSocket(pData); //����ֵΪ0��ʾ�ɹ���������0��ʾʧ��
	}

	return Ret;//����ֵΪ0�ɹ�������ֵΪ-1ʧ��
}

RF_STATUS TestRecvCmd(PC_CMD_REPLY *pcCmdReply, PC_CMD_TYPE pcCmdType, unsigned int TimeOut)
{
	int Ret;

	if (CommunicationFlag == FALSE)  //��USB�ڽ�������
	{
		char *VID = "0x1fc9";
		char *PID = "0x107";
		Ret = ReceiveDataFromUsb(VID, PID, TimeOut, pcCmdType, pcCmdReply);
	}

	if (CommunicationFlag == TRUE)  //�����ڽ�������
	{
		Ret = RceiveDataFromSocket(TimeOut, pcCmdType, pcCmdReply);
	}

	return Ret;//����ֵΪ1�ɹ�������ֵΪ0ʧ��
}

RF_STATUS TestRecvSensitivityCmd(BYTE RandomNum[128], int TimeOut)
{
	if (CommunicationFlag == FALSE)  //��USB�ڽ�������
	{
		OVERLAPPED  ReadOverLapped;

		memset(&ReadOverLapped, 0, sizeof(OVERLAPPED));
		ReadOverLapped.hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
		
		int bResult = 0;
		DWORD nBytesGet = 0;

		ReadFile(hCom, RandomNum, 64, NULL, &ReadOverLapped);
		if (WaitForSingleObject(ReadOverLapped.hEvent, TimeOut) == WAIT_TIMEOUT) //��ʱ�ж�
		{
			PRINT("��ȡUSB����ʧ��\n");
			return 0;
		}

		bResult = GetOverlappedResult(hCom, &ReadOverLapped, &nBytesGet, FALSE);
		if (!bResult && nBytesGet == 0)
		{
			PRINT("��������ʧ�ܣ�����\n");
			return 0; //����0��ʾʧ��
		}

		return 1; //����1��ʾ�ɹ�
	}

	if (CommunicationFlag == TRUE)  //�����ڽ�������
	{
		int RetLen = recv(sockSrv, (char *)RandomNum, 128, 0);

		if(RetLen == SOCKET_ERROR)
		{
			PRINT("����SOCKET���ݳ�ʱ��\n");
			return 0;
		}

		return RetLen;
	}

	return 0;
}

/*----------------------------------------API----------------------------------------*/
//��װ�����ʼ��
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
	long long count;			//��ʱ����Ҫ�ļ���ֵ
	long long cntStart,cntEnd;
	QueryPerformanceCounter(&t);
	cntStart = t.QuadPart;

	QueryPerformanceFrequency(&t);
	dFrequency = (double)t.QuadPart;
	/********************************************
	*
	*    ��ʱʱ��   T(s)            f*tMs
	*count = --------------- = -----------------
	*   ���������� T(1/f)(s)        1000(����λΪs)
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

//����
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
		PRINT ("���ѷ��ʹ���\n");
		return IF_TYPE_WAKEUP_TAG | WAKEUP_TAG_STATE_FAILED;
	}

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_WAKEUP_TAG, 20*TIME_OUT))
	{
		PRINT("���ѻظ���ʱ\n");
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
*�������ƣ�rfCollectTag
*�������ܣ��̴棬�ռ����б�ǩ
*����������4��
*����״̬��
*/
RF_STATUS rfCollectTag(	OUT  TAG_INFO*  tagInfoArray,	//��ǩ��Ϣ����ָ��
						IN	 DWORD		tagNums,		//�����С
						OUT	 WORD*		collectTagNums,	//�ռ����ı�ǩ����
						IN	 BYTE		retryTimes,		//�����ռ��ڴ���
						IN	 DWORD		readyCode		//��������
						)
{
	if (retryTimes == 0)
	{
		PRINT ("�ռ���������С�ڵ�����");
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
		PRINT ("�̴淢�ʹ���\n");
		return IF_TYPE_COLLECT_TAG | COLLECT_TAG_STATE_SEND_FAILED;
	}

	while (1)
	{
		if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_COLLECT, /*20*TIME_OUT*/INFINITE))
		{
			PRINT("�̴��ǩ���Ķ����ظ���ʱ\n");
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

			PRINT("��ǩID:%s\n��ǩ����:%d\nUII:%s\n", Tid, tagInfoArray[*collectTagNums - 1].tagState, Uii);
			if (*collectTagNums > tagNums)	//��ֹ��ǩ������������Ĳ�������
			{
				PRINT("�յ���ǩ�������û���������ֵ���̴�ʧ�ܣ�\n");
				break;
			}
		}
		if (tagNumNow == pcCmdReply.uReplyParam.collectParam.numsOfAllTag)
		{
			PRINT("�̴�������ɣ�\n");
			PRINT("���յ� %d ����ǩ������ %d ����ǩ.\n", tagNumNow, pcCmdReply.uReplyParam.collectParam.numsOfAllTag);
			break;
		}else{
			PRINT ("���յ� %d ����ǩ����Ҫ %d ����ǩ.\n", tagNumNow, pcCmdReply.uReplyParam.collectParam.numsOfAllTag);
		}
	}
	if (0 == *collectTagNums)//û�̴浽��ǩ
	{
		PRINT ("����û�̴浽��ǩ\n");
		return IF_TYPE_COLLECT_TAG | COLLECT_TAG_STATE_EMPTY;
	}

	return IF_TYPE_COLLECT_TAG | COLLECT_TAG_STATE_SUCCESS;
}

/******************************************************
*�������ƣ�rfCollectSpecContTag
*�������ܣ��̴�ָ�����ݱ�ǩ���ռ����б�ǩ
*����������
*����״̬��
*/
RF_STATUS rfCollectSpecContTag( IN	WORD	fileID,			//�ļ���ʾ��
								IN	BYTE	specCont[70],	//ָ������
								IN	char	datalen,		//ָ�����ݵĳ���(<80�ֽ�)
								OUT TAG_INFO*  tagInfoArray,//��ǩ��Ϣ����ָ��
								OUT	DWORD*	   collectTagNums,//�ռ����ı�ǩ����
								IN	BYTE	retryTimes,		  //�����ռ��ڴ���
								IN	DWORD	readyCode		  //��������
								)
{
	if (datalen > 70)
	{
		PRINT ("ָ�����ݵĳ��Ȳ��ܳ���70�ֽ�");
		return IF_TYPE_COLLECT_SPEC_CONT | COLLECT_TAG_STATE_SEND_FAILED;
	}
	if (retryTimes == 0)
	{
		PRINT ("�ռ���������С�ڵ�����");
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
		PRINT ("ָ���̴淢�ʹ���\n");
		return IF_TYPE_COLLECT_SPEC_CONT | COLLECT_TAG_STATE_SEND_FAILED;
	}
	Sleep(3000);  //Ϊʲôsleep��ppwu 12-28
	while (1)
	{
		if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_COLLECT, 22000))
		{
			PRINT("ָ���̴��ǩ���Ķ����ظ���ʱ\n");
			return IF_TYPE_COLLECT_SPEC_CONT| COLLECT_TAG_STATE_TIMEOUT;
		}
		if (COLLECT_TAG_STATE_EMPTY == pcCmdReply.uReplyParam.collectParam.collectTagState)
		{
			PRINT("������û�з���ָ�����ݵı�ǩ��");
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
			//PRINT("��ǩID:%s\n��ǩ����:%d\nUII:%s\n",Tid,tagInfoArray[*collectTagNums - 1].tagState,Uii);
		}
		if (tagNumNow == pcCmdReply.uReplyParam.collectParam.numsOfAllTag)
		{
			PRINT("ָ���̴�������ɣ�\n");
			break;
		}
	}
	if (0 == *collectTagNums)//û�̴浽��ǩ
	{
		PRINT ("����û�̴浽��ǩ\n");
		return IF_TYPE_COLLECT_SPEC_CONT | COLLECT_TAG_STATE_EMPTY;
	}

	return IF_TYPE_COLLECT_SPEC_CONT | COLLECT_TAG_STATE_SUCCESS;
}

/******************************************************
*�������ƣ�rfWakeupOneTag
*�������ܣ����ѵ�����ǩ��
*����������
*����״̬��
*/
RF_STATUS rfWakeupOneTag(IN  const BYTE* TID,
						 IN	 DWORD		 readyCode,		//��������
						 OUT TAG_INFO*   tagInfoArray	//��ǩ��Ϣ����ָ��
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
		PRINT ("���ѵ�����ǩ��\n");
		return IF_TYPE_WAKEUP_ONE_TAG | WAKEUP_ONE_TAG_STATE_TIMEOUT;
	}

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_WAKEUP_ONE_TAG, 3*TIME_OUT))
	{
		return IF_TYPE_WAKEUP_ONE_TAG | WAKEUP_ONE_TAG_STATE_TIMEOUT;
	}
	switch(pcCmdReply.uReplyParam.wakeupOneTagParam.wakeupOneTagState)
	{
	case WAKEUP_ONE_TAG_STATE_SUCCESS:
		PRINT ("���ѵ�����ǩ�ɹ�\n");
		isTIDselected = TRUE;
		memcpy(tagInfoArray,&pcCmdReply.uReplyParam.wakeupOneTagParam.tagInfo,sizeof(TAG_INFO));
		return IF_TYPE_WAKEUP_ONE_TAG | WAKEUP_ONE_TAG_STATE_SUCCESS;
	case WAKEUP_ONE_TAG_STATE_NOT_FOUND:
		PRINT ("���ѵ�����ǩδ����\n");
		return IF_TYPE_WAKEUP_ONE_TAG | WAKEUP_ONE_TAG_STATE_NOT_FOUND;
	default:
		return IF_TYPE_WAKEUP_ONE_TAG | WAKEUP_ONE_TAG_STATE_TIMEOUT;
	}
}

/******************************************************
*�������ƣ�rfSelectTag
*�������ܣ�ѡ���ǩ��ʹ��ǩ���ڱ�ѡ��״̬��
*����������
*����״̬��
*/
RF_STATUS rfSelectTag(IN  BYTE  getTID[8])//��ǩTID,8�ֽ�
{
	int Info;
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_SELECT_TAG);
	memcpy(pcCmdData.uParam.SELECT_TAG_PRAM.TID,getTID,8);

	Info = send_cmd(&pcCmdData);
	if (0 != Info)
	{
		PRINT ("ѡ���ǩ���ʹ���\n");
		return IF_TYPE_SELECT_TAG | SELECT_TAG_STATE_SEND_FAILED;
	}
	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_SELECT_TAG, 3*TIME_OUT))
	{
		PRINT ("ѡ���ǩ��ʱ\n");
		return IF_TYPE_SELECT_TAG | SELECT_TAG_STATE_TIMEOUT;
	}

	switch (pcCmdReply.uReplyParam.selectTagParam.selectTagState)
	{
	case SELECT_TAG_STATE_SUCCESS:
		memcpy(TID,getTID,8);
		isTIDselected = TRUE;	//ȫ�ֱ���
		PRINT("ѡ���ǩ�ɹ�\n");
		return IF_TYPE_SELECT_TAG | SELECT_TAG_STATE_SUCCESS;
	case SELECT_TAG_STATE_NOT_EXISTING:
		PRINT("ѡ���ǩ������\n");
		return IF_TYPE_SELECT_TAG | SELECT_TAG_STATE_NOT_EXISTING;
	case SELECT_TAG_STATE_ALREADY_SELECTED:
		PRINT("��ǩ�Ѿ���ѡ��\n");
		return IF_TYPE_SELECT_TAG | SELECT_TAG_STATE_ALREADY_SELECTED;
	default:
		PRINT("ѡ���ǩδ֪����~~~~~~selectTagStateΪ%d\n",pcCmdReply.uReplyParam.selectTagParam.selectTagState);
		return IF_TYPE_SELECT_TAG | pcCmdReply.uReplyParam.selectTagParam.selectTagState;
	}
}

/******************************************************
*�������ƣ�rfOpenMonitorSensor
*�������ܣ���/�رշ�������
*����������
*����״̬��
*/
RF_STATUS rfOpenMonitorSensor(IN 	WORD 	FileId,		//�ļ�ID
							  IN 	BYTE 	SensorId,	//������ID
							  IN 	BYTE 	SensorState	//������״̬
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
		PRINT ("�򿪷�������ʱ\n");
		return IF_TYPE_OPEN_SENSOR | MONITOR_SENSOREN_STATE_TIMEOUT;
	}

	switch (pcCmdReply.uReplyParam.monitorSensorEnParam.sensorEnState)
	{
	case MONITOR_SENSOREN_STATE_SUCCESS:
		PRINT ("��/�رշ������ɹ�\n");
		return IF_TYPE_OPEN_SENSOR | pcCmdReply.uReplyParam.monitorSensorEnParam.sensorEnState;
	case MONITOR_SENSOREN_STATE_UNKNOWN:
		PRINT ("��/�رշ�����δ֪\n");
		return IF_TYPE_OPEN_SENSOR | pcCmdReply.uReplyParam.monitorSensorEnParam.sensorEnState;
	case MONITOR_SENSOREN_STATE_SENSOR_IDMISMATCH:
		PRINT ("��/�رշ�����ID��ʧƥ��\n");
		return IF_TYPE_OPEN_SENSOR | pcCmdReply.uReplyParam.monitorSensorEnParam.sensorEnState;
	case MONITOR_SENSOREN_STATE_TIMEOUT:
		PRINT ("��/�رշ�������ʱ\n");
		return IF_TYPE_OPEN_SENSOR | pcCmdReply.uReplyParam.monitorSensorEnParam.sensorEnState;
	case MONITOR_SENSOREN_STATE_FAILED:
		PRINT ("��/�رշ�����ʧ��\n");
		return IF_TYPE_OPEN_SENSOR | pcCmdReply.uReplyParam.monitorSensorEnParam.sensorEnState;
	default:
		return IF_TYPE_OPEN_SENSOR | pcCmdReply.uReplyParam.monitorSensorEnParam.sensorEnState;
	}
}

/******************************************************
*�������ƣ�rfSleepAllTag
*�������ܣ���ʹ�����ǩ���������б�ǩ��
*����������
*����״̬��
*/
RF_STATUS rfSleepAllTag(IN 	BYTE 	sleepTime,		//˯��ʱ��(��λ:1s)
						IN	DWORD 	sleepCode,		//˯�߿���
						IN 	BYTE 	listenTime		//����ʱ�䣨����ֵ��11��
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
		PRINT("�������б�ǩ�����ʧ��\n");
		return IF_TYPE_SLEEP_ALL_TAG | SLEEP_TAG_FAILED;
	}

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_SLEEP_ALL_TAG, 3*TIME_OUT))
	{
		PRINT ("�������б�ǩ��ʱ\n");
		return IF_TYPE_SLEEP_ALL_TAG | SLEEP_TAG_TIMEOUT;
	}
	PRINT ("sleep All tag success.sleepTime:0x%x sleepCode:0x%x listenTime:0x%x\n",sleepTime,sleepCode,listenTime);

	return IF_TYPE_SLEEP_ALL_TAG | SLEEP_TAG_SUCCESS;
}

/******************************************************
*�������ƣ�rfSleepExOnesTag
*�������ܣ����߳�ĳһ���ǩ�����б�ǩ
*����������
*����״̬��
*/
RFIDLIBDLL RF_STATUS rfSleepExOnesTag(IN BYTE  sleepTime,		//˯��ʱ��
									  IN DWORD sleepCode,		//˯�߿���
									  IN BYTE  listenTime,		//����ʱ�䣨����ֵ��11��
									  IN WORD  fileID,			//�ļ���ʶ��
									  IN BYTE  offset,			//ƫ��
									  IN BYTE  dataLen,		//���ݳ���
									  IN BYTE  dataBuffer[]	//�������ݣ�������80�ֽڣ�
									  )
{
	if (dataLen > 80)
	{
		PRINT("���߳�ĳһ���ǩ�����б�ǩʧ�ܣ�dataLen���ܴ���80\n");
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
		PRINT("�������߳�ĳһ���ǩ�����б�ǩ�������\n");
		return IF_TYPE_SLEEP_EX_ONES_TAG | SLEEP_EX_ONES_TAG_STATE_FAILED;
	}

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_SLEEP_EX_ONES_TAG, 3*TIME_OUT))
	{
		PRINT ("���߳�ĳһ���ǩ�����б�ǩ��ʱ\n");
		return IF_TYPE_SLEEP_EX_ONES_TAG | SLEEP_EX_ONES_TAG_STATE_TIMEOUT;
	}
	PRINT ("���߳�ĳһ���ǩ�����б�ǩ�ɹ�\n");
	return IF_TYPE_SLEEP_EX_ONES_TAG | SLEEP_EX_ONES_TAG_STATE_SUCCESS;
}

/******************************************************
*�������ƣ�rfSleepExOneTag
*�������ܣ����߳�ĳ����ǩ�����б�ǩ
*����������
*����״̬��
*/
RFIDLIBDLL RF_STATUS rfSleepExOneTag( IN	BYTE	TID[8],
									  IN 	BYTE 	sleepTime,		//˯��ʱ��
									  IN	DWORD 	sleepCode,		//˯�߿���
									  IN 	BYTE 	listenTime		//����ʱ�䣨����ֵ��11��
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
		PRINT("��������ĳ����ǩ�����������\n");
		return IF_TYPE_SLEEP_EX_ONE_TAG | SLEEP_EX_ONE_TAG_STATE_FAILED;
	}

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_SLEEP_EX_ONE_TAG, 3*TIME_OUT))
	{
		PRINT ("���߳�ĳ����ǩ�����б�ǩ��ʱ\n");
		return IF_TYPE_SLEEP_EX_ONE_TAG | SLEEP_EX_ONE_TAG_STATE_TIMEOUT;
	}
	PRINT ("���߳�ĳ����ǩ�����б�ǩ�ɹ�\n");
	return IF_TYPE_SLEEP_EX_ONE_TAG | SLEEP_EX_ONE_TAG_STATE_SUCCESS;
}

/******************************************************
*�������ƣ�rfListFile
*�������ܣ��б��ļ������б����ʽ��ʾһ���ļ�
*����������
*����״̬��
*/
RF_STATUS rfListFile( IN	BYTE 		listMode,		//�б�ģʽ:02:��ǰѡ���ļ�ǰһ���ļ�,03:��ǰѡ���ļ���һ���ļ�,04:ָ��ƫ����λ���ļ�
					  IN	BYTE 		dfRecPointer,	//04ģʽʱ,ָ�����ļ�ƫ����
					  IN 	DWORD 		fileNums1,		//���뻺����fileAttr_R�Ĵ�С,��λΪFILE_ATTRIBUTE_R��С
					  OUT 	FILE_ATTRIBUTE_R* 	fileAttr_R,	//�ļ����Ի�����ָ��
					  OUT 	DWORD* 		fileNums2		//����ʵ���б��ļ�ʱ�ļ�����
					 )
{
	if ((selectedFile.securityType & TOKEN_LIST_MASK) == TOKEN_LIST_STATE_PWD)  //��Ҫ����
	{
		if (false == pwdState.isListPwdPassed)
		{
			PRINT ("�б��ļ������������벢��֤������ȷ\n");
			return IF_TYPE_LIST_FILE | LIST_FILE_STATE_NEED_PWD_VERIFY;
		}
	}
	if ((selectedFile.securityType & TOKEN_LIST_MASK) == TOKEN_LIST_STATE_ATH)  //��ҪУ��
	{
		if ((rfSafeSession() != (IF_TYPE_SAFE_SESSION | SAFE_SESSION_STATE_SUCCESS)) &&
			(rfSafeSession() != (IF_TYPE_SAFE_SESSION | SAFE_SESSION_STATE_ALREADY_IN)))
		{
			PRINT ("�б��ļ�����ȫ�Ự����ȷ�����Ƚ��밲ȫ�Ự\n");
			return IF_TYPE_LIST_FILE | LIST_FILE_STATE_NEED_ATH_VERIFY;
		}
	}
	if ((selectedFile.securityType & TOKEN_LIST_MASK) == TOKEN_LIST_STATE_NEV)
	{
		PRINT("��Ŀ¼�ļ�listȨ��ΪNEV�������б�\n");
		return IF_TYPE_LIST_FILE | LIST_FILE_STATE_FAILED;
	}

	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	
	*fileNums2 = 0;
	int cnt = 0;	//ѭ��cnt(5)���б��ļ�ʧ�����˳�������
	initCmdData(pcCmdData,PC_CMD_TYPE_LIST_FILE);
	pcCmdData.uParam.LIST_FILE_PARAM.dfRecPointer = 0;//dfRecPointer;
	pcCmdData.uParam.LIST_FILE_PARAM.lsitMode = listMode;
	pcCmdData.uParam.LIST_FILE_PARAM.opMode = DERECTORY_LIST_MODE_CURRENT;
	int Info = send_cmd(&pcCmdData);

	do
	{
		if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_LIST_FILE, 15*TIME_OUT))
		{
			PRINT ("�б��ļ���ʱ\n");
			return IF_TYPE_LIST_FILE | LIST_FILE_STATE_TIMEOUT;
		}
		if (pcCmdReply.uReplyParam.listFileParam.listState == LIST_FILE_STATE_NO_TID)
		{
			printf("�б�ǰ��ѡ���ǩ\n");
			return IF_TYPE_LIST_FILE | LIST_FILE_STATE_NO_TID;
		}
		
		if (pcCmdReply.uReplyParam.listFileParam.listState == LIST_FILE_STATE_FILE_ALREDAY_LAST_FILE)
		{
			
			PRINT ("�б��ļ��ɹ�\n");
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
			PRINT ("�б��ļ���������֤\n");
			return IF_TYPE_LIST_FILE | LIST_FILE_STATE_NEED_PWD_VERIFY;
		case LIST_FILE_STATE_NEED_ATH_VERIFY:
			PRINT ("�б��ļ���ʵ����֤\n");
			return IF_TYPE_LIST_FILE | LIST_FILE_STATE_NEED_ATH_VERIFY;
		case LSIT_FILE_STATE_FILE_NOT_EXISTING:
			PRINT ("�б��ļ�������\n");
			return IF_TYPE_LIST_FILE | LSIT_FILE_STATE_FILE_NOT_EXISTING;
		case LIST_FILE_STATE_FAILED:
			PRINT ("�б��ļ�ʧ��\n");
			cnt++;
			if (cnt >= 5)
			{
				return IF_TYPE_LIST_FILE | LIST_FILE_STATE_FAILED;
			}
		case LIST_FILE_STATE_TIMEOUT:
			PRINT("�б��ļ���ʱ\n");
			return IF_TYPE_LIST_FILE | LIST_FILE_STATE_TIMEOUT;
		default:
			return IF_TYPE_LIST_FILE | LIST_FILE_STATE_FAILED;
		}

		if (fileNums1 < *fileNums2)
		{
			PRINT ("�б��ļ�����������\n");
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
*�������ƣ�rfListFileNew
*�������ܣ����б��ļ�
*����������
*����״̬��
*��ע��by ppwu 12-27
*/
RF_STATUS rfListFileNew(IN  BYTE  listMode,		//�б�ģʽ:02:��ǰѡ���ļ�ǰһ���ļ�,03:��ǰѡ���ļ���һ���ļ�,04:ָ��ƫ����λ���ļ�
						IN	BYTE  dfRecPointer,	//04ģʽʱ,ָ�����ļ�ƫ����
						IN  BYTE  opMode,
						OUT FILE_ATTRIBUTE_R*  fileAttr_R//�ļ����Ի�����ָ��
						)
{
	if ((selectedFile.securityType & TOKEN_LIST_MASK) == TOKEN_LIST_STATE_PWD)  //��Ҫ����
	{
		if (false == pwdState.isListPwdPassed)
		{
			PRINT ("�б��ļ������������벢��֤������ȷ\n");
			return IF_TYPE_LIST_FILE | LIST_FILE_STATE_NEED_PWD_VERIFY;
		}
	}
	if ((selectedFile.securityType & TOKEN_LIST_MASK) == TOKEN_LIST_STATE_ATH)  //��ҪУ��
	{
		if ((rfSafeSession() != (IF_TYPE_SAFE_SESSION | SAFE_SESSION_STATE_SUCCESS)) &&
			(rfSafeSession() != (IF_TYPE_SAFE_SESSION | SAFE_SESSION_STATE_ALREADY_IN)))
		{
			PRINT ("�б��ļ�����ȫ�Ự����ȷ�����Ƚ��밲ȫ�Ự\n");
			return IF_TYPE_LIST_FILE | LIST_FILE_STATE_NEED_ATH_VERIFY;
		}
	}
	if ((selectedFile.securityType & TOKEN_LIST_MASK) == TOKEN_LIST_STATE_NEV)
	{
		PRINT("��Ŀ¼�ļ�listȨ��ΪNEV�������б�\n");
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
		//ʧ��
		return IF_TYPE_LIST_FILE | LIST_FILE_STATE_FAILED;
	}

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_LIST_FILE, INFINITE/*3*TIME_OUT*/))
	{
		PRINT ("�б��ļ���ʱ\n");
		return IF_TYPE_LIST_FILE | LIST_FILE_STATE_TIMEOUT;
	}
	if (pcCmdReply.uReplyParam.listFileParam.listState == LIST_FILE_STATE_NO_TID)
	{
		printf("�б�ǰ��ѡ���ǩ\n");
		return IF_TYPE_LIST_FILE | LIST_FILE_STATE_NO_TID;
	}

	if (pcCmdReply.uReplyParam.listFileParam.listState == LIST_FILE_STATE_FILE_ALREDAY_LAST_FILE)
	{

		PRINT ("�б��ļ��ɹ�\n");
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
		PRINT ("�б��ļ���������֤\n");
		return IF_TYPE_LIST_FILE | LIST_FILE_STATE_NEED_PWD_VERIFY;
	case LIST_FILE_STATE_NEED_ATH_VERIFY:
		PRINT ("�б��ļ���ʵ����֤\n");
		return IF_TYPE_LIST_FILE | LIST_FILE_STATE_NEED_ATH_VERIFY;
	case LSIT_FILE_STATE_FILE_NOT_EXISTING:
		PRINT ("�б��ļ�������\n");
		return IF_TYPE_LIST_FILE | LSIT_FILE_STATE_FILE_NOT_EXISTING;
	case LIST_FILE_STATE_FAILED:
		PRINT ("�б��ļ�ʧ��\n");
		return IF_TYPE_LIST_FILE | LIST_FILE_STATE_FAILED;
	case LIST_FILE_STATE_TIMEOUT:
		PRINT("�б��ļ���ʱ\n");
		return IF_TYPE_LIST_FILE | LIST_FILE_STATE_TIMEOUT;
	default:
		return IF_TYPE_LIST_FILE | LIST_FILE_STATE_FAILED;
	}
}

/******************************************************
*�������ƣ�rfCreateFile
*�������ܣ������ļ��������������64K���ļ���Ҫ��֤һ������������
*				1�����ļ�ID������4�ı�����
*				2��ID + 1��ID+ 2��ID + 3�⼸���ļ�ID���뱣�������ļ�ʹ�ã��´δ���ʱ��ע�ⲻҪ����ЩID
*����������
*����״̬��
*/
RF_STATUS rfCreateFile(IN  FILE_ATTRIBUTE_W*  fileAttr_W)	//�ļ�����:fileAttr_Wָ��
{
	selectedFile.fileID = NULL;
	if (fileAttr_W == NULL)
	{
		PRINT("�����ָ��Ϊ��\n");
		return IF_TYPE_CREATE_FILE | CREATE_FILE_STATE_IN_NULL;
	}
	if (fileAttr_W->fileAttr_R.fileID < 0x3f00)
	{
		PRINT("�������ļ�ID����С��0x3f00\n");
		return IF_TYPE_CREATE_FILE | CREATE_FILE_ATTRIBUTE_ERROR;
	}
	if ((fileAttr_W->fileAttr_R.fileTypeValid & FILE_TYPE_MASK) == FILE_TYPE_DF)
	{
		if ((fileAttr_W->fileAttr_R.fileTypeValid & FILE_INVALID_MASK) == FILE_TYPE_INVALID)
		{
			PRINT("������Ŀ¼�ļ�����Ϊ��ЧĿ¼\n");
			return IF_TYPE_CREATE_FILE | CREATE_FILE_INVALID;
		}
		if (fileAttr_W->fileAttr_R.uFileAttr.MFDF.numsOfMaxFile > 65532)
		{
			PRINT("������Ŀ¼�ļ�����ļ������ܳ���65532");
			return IF_TYPE_CREATE_FILE | CREATE_FILE_SPACE_NOT_ENOUPH;
		}
		fileAttr_W->fileAttr_R.uFileAttr.MFDF.numsOfRealFile = 0;
		//fileAttr_W->fileAttr_R.uFileAttr.MFDF.numsOfMaxFile += 3;

	}
	if (((fileAttr_W->fileAttr_R.fileTypeValid & FILE_TYPE_MASK) == FILE_TYPE_TRANS) &&
		(fileAttr_W->fileAttr_R.uFileAttr.transBinEF.realFileLen > 0))
	{
		PRINT("������͸���ļ�ʵ�ʳ��ȴ���0������͸���ļ�ʧ��\n");
		return IF_TYPE_CREATE_FILE | CREATE_FILE_STATE_FAILED;
	}
	if (((fileAttr_W->fileAttr_R.fileTypeValid & FILE_TYPE_MASK) == FILE_TYPE_RECORD) &&
		(fileAttr_W->fileAttr_R.uFileAttr.recEF.maxRecordNums < fileAttr_W->fileAttr_R.uFileAttr.recEF.realRecordNums))
	{
		PRINT("�����ļ�¼�ļ�ʵ�ʼ�¼����������¼����������¼�ļ�ʧ��\n");
		return IF_TYPE_CREATE_FILE | CREATE_FILE_STATE_FAILED;
	}
	for (unsigned int i=0; i < listFileNums; i++)
	{
		if (fileAttr_W->fileAttr_R.fileID == listFileID[i])
		{
			PRINT ("���ļ��Ѵ��ڣ������ٴδ���\n");
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
		PRINT("�����ļ�����ʧ��\n");
		return IF_TYPE_CREATE_FILE | CREATE_FILE_STATE_FAILED;
	}

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_CREATE_FILE, 3*TIME_OUT))	//��ʱ
	{
		return IF_TYPE_CREATE_FILE | CREATE_FILE_STATE_FAILED;
	}

	switch (pcCmdReply.uReplyParam.createFileParam.createFileState)
	{
	case CREATE_FILE_STATE_SUCCESS:
		PRINT ("�����ļ��ɹ�,ID = %d\n",fileAttr_W->fileAttr_R.fileID);

		return IF_TYPE_CREATE_FILE | CREATE_FILE_STATE_SUCCESS;
	case CREATE_FILE_STATE_FAILED:
		PRINT ("�����ļ�ʧ��\n");
		return IF_TYPE_CREATE_FILE | CREATE_FILE_STATE_FAILED;
	case CREATE_FILE_STATE_TIMEOUT:
		PRINT ("�����ļ���ʱ\n");
		return IF_TYPE_CREATE_FILE | CREATE_FILE_STATE_TIMEOUT;
	case CREATE_FILE_STATE_EXISTING:	//�ļ��Ѿ�����
		PRINT ("�ļ��Ѵ���\n");
		return IF_TYPE_CREATE_FILE | CREATE_FILE_STATE_EXISTING;
	default:
		return IF_TYPE_CREATE_FILE | pcCmdReply.uReplyParam.createFileParam.createFileState;
	}
}

/******************************************************
*�������ƣ�rfSelectFile
*�������ܣ�ѡ���ļ���
*����������
*����״̬��
*/
RF_STATUS rfSelectFile(IN  WORD  fileID,
					   OUT FILE_ATTRIBUTE_R*  fileAttr_R	//����ѡ����ļ�����
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
	if (0 == dispatch_cmd(&pcRepData, PC_CMD_TYPE_SELECT_FILE, 3*TIME_OUT))//��ʱ
	{
		return IF_TYPE_SELECT_FILE | SELECT_FILE_STATE_TIMEOUT;
	}


	switch (pcRepData.uReplyParam.selectFileParam.selectFileState)
	{
	case SELECT_FILE_STATE_SUCCESS:
		PRINT ("ѡ���ļ��ɹ�,file ID = %x\n",pcRepData.uReplyParam.selectFileParam.fileAttr_R.fileID);
		PRINT("security type:%x\n",pcRepData.uReplyParam.selectFileParam.fileAttr_R.securityType);
		*fileAttr_R = pcRepData.uReplyParam.selectFileParam.fileAttr_R;
		selectedFile = pcRepData.uReplyParam.selectFileParam.fileAttr_R;
		return IF_TYPE_SELECT_FILE | SELECT_FILE_STATE_SUCCESS;
	case SELECT_FILE_STATE_NEED_PWD_VERIFY:
		PRINT ("ѡ���ļ���Ҫ������֤\n");
		return IF_TYPE_SELECT_FILE | SELECT_FILE_STATE_NEED_PWD_VERIFY;
	case SELECT_FILE_STATE_NEED_ATH_VERIFY:
		PRINT ("ѡ���ļ���Ҫʵ����֤\n");
		return IF_TYPE_SELECT_FILE | SELECT_FILE_STATE_NEED_ATH_VERIFY;
	case SELECT_FILE_STATE_NOT_EXISTING:
		PRINT ("��ѡ�ļ�������\n");
		return IF_TYPE_SELECT_FILE | SELECT_FILE_STATE_NOT_EXISTING;
	case SELECT_FILE_STATE_DENIED:
		PRINT ("ѡ���ļ�,���ʱ��ܾ�,���ļ�������ѡ��Ȩ��\n");
		return IF_TYPE_SELECT_FILE | SELECT_FILE_STATE_DENIED;
	case SELECT_FILE_STATE_TIMEOUT:
		PRINT("ѡ���ļ��ļ���ʱ\n");
		return IF_TYPE_SELECT_FILE | SELECT_FILE_STATE_TIMEOUT;
	default:
		return IF_TYPE_SELECT_FILE | pcRepData.uReplyParam.selectFileParam.selectFileState;
	}
}

/******************************************************
*�������ƣ�rfReadRecFile
*�������ܣ�����¼�ļ���
*����������
*����״̬��
*/
RF_STATUS rfReadRecFile(IN 	BYTE 	recPointer,			//��¼ƫ����
						IN 	BYTE 	opMode,				//02:��ǰ��¼�ļ�����һ����¼,03:��ǰѡ���ļ��е���һ����¼,04:��¼ƫ����recOffsetָ��
						IN 	BYTE 	recOffset,			//��¼��ƫ��
						IN 	BYTE 	dataLen,			//��ȡ����
						OUT WORD* 	retLen,				//���ض��������ݳ���
						OUT BYTE 	buff[]				//���ļ����ػ�����
						)
{
	if (selectedFile.fileID == NULL)	//ûѡ�ļ�
	{
		PRINT ("����¼�ļ���ûѡ�ļ�\n");
		return IF_TYPE_READ_RECORD_FILE | READ_FILE_STATE_FAILED;
	}

	printf ("State:%d\n",selectedFile.securityType & TOKEN_RDSEARCH_MASK);

	if ((selectedFile.securityType & TOKEN_RDSEARCH_MASK) == TOKEN_RDSEARCH_STATE_PWD)  //��Ҫ����
	{
		if (false == pwdState.isReadPwdPassed)
		{
			PRINT ("����¼�ļ������������벢��֤������ȷ\n");
			return IF_TYPE_READ_RECORD_FILE | READ_FILE_STATE_NEED_PWD_VERIFY;
		}
	}

	if ((selectedFile.securityType & TOKEN_RDSEARCH_MASK) == TOKEN_RDSEARCH_STATE_NEV)  //Ȩ��ΪNEV
	{
		PRINT ("����¼�ļ���Ȩ��ΪNEV�����ʱ��ܾ�\n");
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
			PRINT ("����¼�ļ� ��ʱ\n");
			return IF_TYPE_READ_RECORD_FILE | READ_FILE_STATE_FAILED;
		}
		switch (pcRepData.uReplyParam.readFileParam.readFileState)
		{
		case READ_FILE_STATE_SUCCESS: //δ�꣬����...
			{
				PRINT ("����¼�ļ��ɹ�\n");
				memcpy (buff + *retLen, pcRepData.uReplyParam.readFileParam.buff,
					pcRepData.uReplyParam.readFileParam.retLen);
				*retLen += pcRepData.uReplyParam.readFileParam.retLen;
				if(selectedFile.uFileAttr.recEF.recordLen == *retLen)
					flag = FALSE;
			}
			break;
		case READ_FILE_STATE_NOT_EXISTING:
			PRINT ("����ȡ�ü�¼������\n");
			return IF_TYPE_READ_RECORD_FILE | pcRepData.uReplyParam.readFileParam.readFileState;
		case READ_FILE_STATE_FAILED:
			PRINT ("����¼�ļ�ʧ��\n");
			return IF_TYPE_READ_RECORD_FILE | pcRepData.uReplyParam.readFileParam.readFileState;
		case READ_FILE_STATE_LAST_RECORD:
			PRINT ("����¼�ļ��ѵ����һλ\n");
			return IF_TYPE_READ_RECORD_FILE | pcRepData.uReplyParam.readFileParam.readFileState;
		case READ_FILE_STATE_FINISH:
			PRINT ("����¼�ļ����\n");
			return IF_TYPE_READ_RECORD_FILE | pcRepData.uReplyParam.readFileParam.readFileState;
		case READ_FILE_STATE_NEED_PWD_VERIFY:
			PRINT ("����¼�ļ�������������\n");
			return IF_TYPE_READ_RECORD_FILE	| pcRepData.uReplyParam.readFileParam.readFileState;
		case READ_FILE_STATE_NEED_ATH_VERIFY:
			PRINT ("����¼�ļ����Ƚ��밲ȫ�Ự\n");
			return IF_TYPE_READ_RECORD_FILE	| pcRepData.uReplyParam.readFileParam.readFileState;
		case READ_FILE_STATE_NEED_NEV_VERIFY:
			PRINT ("��⣬�ü�¼�ļ�����ΪNEV���������\n");
			return IF_TYPE_READ_RECORD_FILE	| pcRepData.uReplyParam.readFileParam.readFileState;
		default:
			PRINT ("δ֪\n");
			return IF_TYPE_READ_RECORD_FILE | pcRepData.uReplyParam.readFileParam.readFileState;
		}
	}
	return IF_TYPE_READ_RECORD_FILE | READ_FILE_STATE_SUCCESS;
}

/******************************************************
*�������ƣ�rfReadTransFile
*�������ܣ���͸���ļ�
*����������
*����״̬��
*��ע:�İ�� by 2013-12-27
*/
RF_STATUS rfReadTransFile(IN  WORD 	fileOffset,	//�ļ�ƫ����
						  IN  WORD  NumToRead,  //��ƫ����fileoffset��ʼ��NumToRead���ֽڣ���Ϊ0��Ĭ�϶����ļ�β
						  OUT WORD* RetNumLen,	//���ض��������ݳ���
						  OUT BYTE 	buff[]		//���ļ����ػ�����
						 )
{
	if (selectedFile.fileID == NULL)	//ûѡ�ļ�
	{
		PRINT ("��͸���ļ���ûѡ�ļ�\n",selectedFile.fileID);
		return IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_FAILED;
	}

	if ((selectedFile.securityType & TOKEN_RDSEARCH_MASK) == TOKEN_RDSEARCH_STATE_PWD)  //��Ҫ����
	{
		if (false == pwdState.isReadPwdPassed)
		{
			PRINT ("��͸���ļ������������벢��֤������ȷ\n");
			return IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_NEED_PWD_VERIFY;
		}
	}

	if ((selectedFile.securityType & TOKEN_RDSEARCH_MASK) == TOKEN_RDSEARCH_STATE_NEV)  //Ȩ��ΪNEV
	{
		PRINT ("��͸���ļ���Ȩ��ΪNEV�����ʱ��ܾ�\n");
		return IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_NEED_NEV_VERIFY;
	}

	//����8192�ֽڳ��ȣ��������ö�ȡ����
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
			PRINT("��͸���ļ���ʱ\n");
			return IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_TIMEOUT;
		}

		switch (pcRepData.uReplyParam.readFileParam.readFileState)
		{
		case READ_FILE_STATE_SUCCESS:	//δ�꣬����������
			{
				PRINT ("��͸���ļ��ɹ�\n");
				memcpy (buff + *RetNumLen, pcRepData.uReplyParam.readFileParam.buff,
							pcRepData.uReplyParam.readFileParam.retLen);
				*RetNumLen += pcRepData.uReplyParam.readFileParam.retLen;

				if(selectedFile.uFileAttr.transBinEF.realFileLen == *RetNumLen)
					flag = FALSE;
			}
			break;
		case READ_FILE_STATE_NOT_EXISTING:
			PRINT ("��͸���ļ�������\n");
			return IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_NOT_EXISTING;
		case READ_FILE_STATE_FAILED:
			PRINT ("��͸���ļ�ʧ��\n");
			return IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_FAILED;
		case READ_FILE_STATE_NEED_PWD_VERIFY:
			PRINT ("��͸���ļ�������������\n");
			return IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_NEED_PWD_VERIFY;
		case READ_FILE_STATE_NEED_ATH_VERIFY:
			PRINT ("��͸���ļ����Ƚ��밲ȫ�Ự\n");
			return IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_NEED_ATH_VERIFY;
		case READ_FILE_STATE_NEED_NEV_VERIFY:
			PRINT ("��⣬��͸���ļ�����ΪNEV���������\n");
			return IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_NEED_NEV_VERIFY;
		default:
			return IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_NEED_NEV_VERIFY;
		}
	}

	PRINT("��͸���ļ����\n");
	return IF_TYPE_READ_TRANS_FILE | READ_FILE_STATE_FINISH;
}

/******************************************************
*�������ƣ�rfWriteFile
*�������ܣ�д�ļ�,��Ϊ�׵�ַд�ļ���ƫ����д��ַ���׵�ַ��ƫ����Ϊ0.
*����������
*����״̬��
*/
RF_STATUS rfWriteFile(IN  char	FileType,		//�ļ����ͣ�0Ϊ͸����1Ϊ��¼
					  IN  WORD 	FileOffset,		//�ļ�ƫ����
					  IN  WORD 	WriteLength,	//д�ļ��ĳ���
					  IN  char* pData			//��д���ļ������ݻ�����
					  )
{
	if (selectedFile.fileID == NULL)	//ûѡ�ļ�
	{
		PRINT ("д�ļ���ûѡ�ļ�\n");
		return IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED;
	}
	if ((selectedFile.securityType & TOKEN_UPDATE_MASK) == TOKEN_UPDATE_STATE_PWD)  //��Ҫ����
	{
		if (false == pwdState.isUpdatePwdPassed)
		{
			PRINT ("д�ļ������������벢��֤������ȷ\n");
			return IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_PWD_VERIFY;
		}
	}
	if ((selectedFile.securityType & TOKEN_UPDATE_MASK) == TOKEN_UPDATE_STATE_NEV)  //Ȩ��ΪNEV
	{
		PRINT ("д�ļ���Ȩ��ΪNEV�����ʱ��ܾ�\n");
		return IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_NEV_VERIFY;
	}

	PC_CMD_DATA  pcCmdData;
	PC_CMD_REPLY  pcRepData;
	//д͸���ļ�
	if (0 == FileType)
	{
		if ((selectedFile.securityType & TOKEN_UPDATE_MASK) == TOKEN_UPDATE_STATE_ATH)  //��ҪУ��
		{
			if ((rfSafeSession() != (IF_TYPE_SAFE_SESSION | SAFE_SESSION_STATE_SUCCESS)) &&
				(rfSafeSession() != (IF_TYPE_SAFE_SESSION | SAFE_SESSION_STATE_ALREADY_IN)))
			{
				PRINT ("д͸���ļ�����ȫ�Ự����ȷ�����Ƚ��밲ȫ�Ự\n");
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
			//��ʼ��͸���ļ����Բ���
			pcCmdData.uParam.WRITE_FILE_PARAM.fileType = FILE_TYPE_TRANS;
			pcCmdData.uParam.WRITE_FILE_PARAM.uWriteParam.transBinEF.fileOffset = FileOffset;
			pcCmdData.uParam.WRITE_FILE_PARAM.uWriteParam.transBinEF.pkgIndex = 1;
			pcCmdData.uParam.WRITE_FILE_PARAM.uWriteParam.transBinEF.dataLen = WriteLength;
			memcpy(pcCmdData.uParam.WRITE_FILE_PARAM.uWriteParam.transBinEF.dataBuffer, pData, WriteLength);

			int ret = send_cmd(&pcCmdData);
			if (ret != 0)
			{
				PRINT("д�ļ���������ʧ��\n");
				return IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED;
			}

			if (dispatch_cmd(&pcRepData, PC_CMD_TYPE_WRITE_FILE, 3*TIME_OUT) == 0)
			{
				PRINT("--д�ļ���ʱ ����--\n");
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

		if (WriteLength > CMD_WRITE_TRANS_MAX_LEN) //�ְ�����
		{
			int length = WriteLength;
			int packetNum = 0;
			BYTE endFlag = 0;//�ж��Ƿ������һ����1��ʾ������

			while(length)
			{
				packetNum++;
				memset(&pcCmdData, 0, sizeof(PC_CMD_DATA));
				pcCmdData.sigStart = SIG_START_NUM;
				pcCmdData.sigEnd = SIG_END_NUM;
				pcCmdData.cmdType = PC_CMD_TYPE_WRITE_FILE;
				//��ʼ��͸���ļ����Բ���
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
					PRINT("д�ļ���������ʧ��\n");
					return IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED;
				}

				memset(&pcRepData, 0, sizeof(PC_CMD_REPLY));
				if(dispatch_cmd(&pcRepData, PC_CMD_TYPE_WRITE_FILE, 3*TIME_OUT) == 0)
				{
					PRINT("--д�ļ���ʱ ����--\n");
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
	//д��¼�ļ�
	else
	{
		memset(&pcRepData, 0, sizeof(PC_CMD_REPLY));
		memset(&pcCmdData, 0, sizeof(PC_CMD_DATA));
		pcCmdData.sigStart = SIG_START_NUM;
		pcCmdData.sigEnd = SIG_END_NUM;
		pcCmdData.cmdType = PC_CMD_TYPE_WRITE_FILE;

		pcCmdData.uParam.WRITE_FILE_PARAM.fileType = FILE_TYPE_RECORD;
		pcCmdData.uParam.WRITE_FILE_PARAM.uWriteParam.recEF.recPointer = (unsigned char)FileOffset;
		pcCmdData.uParam.WRITE_FILE_PARAM.uWriteParam.recEF.opMode = 0x04;//����ԭʼ�ļ���ָ����¼
		if(WriteLength > 108)
		{
			PRINT ("--���ݳ���̫�󣬲��ܳ���108�ֽڣ�--");
			return IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED;
		}
		pcCmdData.uParam.WRITE_FILE_PARAM.uWriteParam.recEF.dataLen = WriteLength;
		memcpy(pcCmdData.uParam.WRITE_FILE_PARAM.uWriteParam.recEF.dataBuffer, pData, WriteLength);

		int ret = send_cmd(&pcCmdData);

		if (dispatch_cmd(&pcRepData, PC_CMD_TYPE_WRITE_FILE, 5*TIME_OUT) == 0)
		{
			PRINT("����ʧ��\n");
			return IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_TIMEOUT;
		}

		switch(pcRepData.uReplyParam.writeFileParam.writeFileState)
		{
		case WRITE_FILE_STATE_SUCCESS:
			PRINT("--д�ļ��ɹ�--\n");
			return IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_SUCCESS;
		case WRITE_FILE_STATE_ATH_VERIFY:
			PRINT ("д������¼�ļ��谲ȫ�Ự\n");
			return IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_ATH_VERIFY;
		default:
			PRINT ("д�ļ�ʧ�� @ rfWriteFile \n");
			return IF_TYPE_WRITE_FILE | WRITE_FILE_STATE_FAILED;
		}
	}
}


/******************************************************
*�������ƣ�rfVerifyPwdFile
*�������ܣ�������֤��
*����������
*����״̬��
*/
RF_STATUS rfVerifyPwdFile(IN BYTE  pwdType,	//��֤��������
						  IN DWORD passWord	//����
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
*�������ƣ�rfFileValidEn
*�������ܣ�ʹ�ļ���Ч����Ч���㡣
*����������
*����״̬��
���޸ģ�
	  ����β�TID
*/
RF_STATUS rfFileValidEn(IN BYTE newFileState)  //�µ��ļ�״̬
{
	int Info;
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcRepData;
	initCmdData(pcCmdData,PC_CMD_TYPE_VALID_FILE);
	if (selectedFile.fileID == NULL)
	{
		PRINT ("Select file first and do next! @ rfFileValidEn\n");
		return IF_TYPE_VALID_FILE | VALID_FILE_STATE_FILE_NOT_EXIST;
	}//ûѡ�ļ�
	if ((selectedFile.fileTypeValid & FILE_TYPE_MASK) == FILE_TYPE_DF)
	{
		PRINT("���ܽ�Ŀ¼�ļ���Ϊ��Ч����Ч\n");
		return IF_TYPE_VALID_FILE | VALID_FILE_STATE_NOT_ALLOWED;
	}

	PRINT ("Input :%d,Before:%d\n",newFileState,selectedFile.fileTypeValid);

	if ( (selectedFile.fileTypeValid & FILE_INVALID_MASK) == !newFileState)
	{
		PRINT ("You know,you cant do same operate twice!\n");
		return IF_TYPE_VALID_FILE | VALID_FILE_STATE_ALREADY_INVALID;
	}//�ظ�����

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
//Q:���ĸ����룬����Ļ��ǹ�����
		pcCmdData.uParam.VERIFY_PWD_FILE_PARAM.pwdType = TOKEN_TYPE_VALID;
		pcCmdData.cmdType = PC_CMD_TYPE_VALID_FILE;
		pcCmdData.sigStart = SIG_START_NUM;
		pcCmdData.sigEnd = SIG_END_NUM;
	}//��Ҫ����
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
			PRINT("��PWD\n");
			return IF_TYPE_VALID_FILE | VALID_FILE_STATE_PWD_VERIFY;
		case VALID_FILE_STATE_ATH_VERIFY:
			PRINT("��ATH\n");
			return IF_TYPE_VALID_FILE | VALID_FILE_STATE_ATH_VERIFY;
		case VALID_FILE_STATE_NEV_VERIFY:
			PRINT("NEV������������\n");
			return IF_TYPE_VALID_FILE | VALID_FILE_STATE_NEV_VERIFY;
		default:
			PRINT ("What happened? @ rfFileValidEn\n");
			return IF_TYPE_VALID_FILE | VALID_FILE_STATE_UNKNOWN_ERR;
	}
}


/******************************************************
*�������ƣ�rfInitTag
*�������ܣ���ʼ����ǩ��
*����������
*����״̬��
*/
RF_STATUS rfInitTag( IN	TAG_ATTRIBUTE*	tagAttr)	//��ǩ����
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	int Info;
	initCmdData(pcCmdData,PC_CMD_TYPE_INIT_TAG);
	pcCmdData.uParam.TAG_ATTR_PARAM.paraType = 0;	//0���������� 1��ECC_key����
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
		PRINT("�������ñ�ǩ������Ϣ����ʧ��\n");
		return IF_TYPE_INIT_TAG | INIT_TAG_STATE_FAILED;
	}
	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_INIT_TAG, 3*TIME_OUT))
	{
		return IF_TYPE_INIT_TAG | INIT_TAG_STATE_TIMEOUT;
	}

	switch(pcCmdReply.uReplyParam.initTagStateParam.initTagState)
	{
	case INIT_TAG_STATE_SUCCESS:
		PRINT ("��ʼ����ǩ�ɹ�\n");
		return IF_TYPE_INIT_TAG | INIT_TAG_STATE_SUCCESS;
	case INIT_TAG_STATE_FAILED:
		PRINT ("��ʼ����ǩʧ��\n");
		return IF_TYPE_INIT_TAG | INIT_TAG_STATE_FAILED;
	default:
		return IF_TYPE_INIT_TAG | INIT_TAG_STATE_TIMEOUT;
	}
}

/******************************************************
*�������ƣ�rfSetReaderInfo
*�������ܣ������Ķ�����Ϣ��
*����������
*����״̬��
*/
RF_STATUS rfSetReaderInfo(IN  READER_INFO_W*  readerInfo_w)		//�����Ķ�����Ϣ
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;

	initCmdData(pcCmdData,PC_CMD_TYPE_SET_READER_INFO);
	pcCmdData.uParam.SET_READER_INFO_PARAM.paraType = 0;	//0���������� 1��ECC_key����
	memcpy(pcCmdData.uParam.SET_READER_INFO_PARAM.u.readerInfo_w.RID,readerInfo_w->RID,3);
	memcpy(pcCmdData.uParam.SET_READER_INFO_PARAM.u.readerInfo_w.ReadName,readerInfo_w->ReadName,32);
	pcCmdData.uParam.SET_READER_INFO_PARAM.u.readerInfo_w.baudRate = readerInfo_w->baudRate;

	int Info = send_cmd(&pcCmdData);
	if (Info != 0)
	{
		PRINT("���������Ķ�����Ϣ����ʧ��\n");
		return IF_TYPE_SET_RADER_INFO | SET_READER_INFO_STATE_FAILED;
	}
	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_SET_READER_INFO, 3*TIME_OUT))
	{
		return IF_TYPE_SET_RADER_INFO | SET_READER_INFO_TIMEOUT;
	}
	switch(pcCmdReply.uReplyParam.setReaderInfoParam.setReaderInfoState)
	{
	case SET_READER_INFO_STATE_SUCCESS:
		PRINT ("�����Ķ���������Ϣ�ɹ�\n");
		return IF_TYPE_SET_RADER_INFO | SET_READER_INFO_STATE_SUCCESS;
	case SET_READER_INFO_STATE_FAILED:
		PRINT ("�����Ķ���������Ϣʧ��\n");
		return IF_TYPE_SET_RADER_INFO | SET_READER_INFO_STATE_FAILED;
	default:
		return IF_TYPE_SET_RADER_INFO | SET_READER_INFO_TIMEOUT;
	}
}

/******************************************************
*�������ƣ�rfGetReaderInfo
*�������ܣ���ȡ�Ķ�����Ϣ��
*����������
*����״̬��
*/
RF_STATUS rfGetReaderInfo(OUT  READER_INFO*  readerInfo)		//��ȡ�Ķ�����Ϣ
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
		PRINT ("��ѯ�Ķ���������Ϣ�ɹ�\n");
		memcpy(readerInfo,&pcCmdReply.uReplyParam.getReaderInfoParam.readerInfo,sizeof(READER_INFO));
		return IF_TYPE_GET_RADER_INFO | GET_READER_INFO_STATE_SUCCESS;
	case GET_READER_INFO_STATE_FAILED:
		PRINT ("��ѯ�Ķ���������Ϣʧ��\n");
		return IF_TYPE_GET_RADER_INFO | GET_READER_INFO_STATE_FAILED;
	default:
		return IF_TYPE_GET_RADER_INFO | GET_READER_INFO_TIMEOUT;
	}
}


/******************************************************
*�������ƣ�rfGetTagInfo
*�������ܣ���ȡ��ǩ������Ϣ
*����������
*����״̬��
*��ע��
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
		PRINT ("��ѯ�Ķ���������Ϣ�ɹ�\n");
		return IF_TYPE_GET_RADER_INFO | GET_TAG_INFO_STATE_SUCCESS;
	case GET_TAG_INFO_STATE_FAILED:
		PRINT ("��ѯ�Ķ���������Ϣʧ��\n");
		return IF_TYPE_GET_RADER_INFO | GET_TAG_INFO_STATE_FAILED;
	default:
		return IF_TYPE_GET_RADER_INFO | GET_TAG_INFO_STATE_TIMEOUT;
	}
}


/******************************************************
*�������ƣ�rfSafeSession
*�������ܣ���ȫ�Ự��
*����������
*����״̬��
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
			PRINT ("�ѽ��밲ȫ�Ự  @��ȫ�ỰAPI\n");
			return IF_TYPE_SAFE_SESSION | SAFE_SESSION_STATE_ALREADY_IN;
		}
	}
	return 0;
}

/******************************************************
*�������ƣ�rfAesRegTag
*�������ܣ�ע��AES��ǩ��
*����������
*����״̬��
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
*�������ƣ�rfEccRegTag
*�������ܣ�ע��Ecc��ǩ��
*����������
*����״̬��
*/

RF_STATUS rfEccRegTag(IN BYTE* getTID,
					  IN BYTE* getTagPubkey,
					  IN BYTE  keyLen
					 )
{
	// 	//���ǩ����ͨ��
	// 	BYTE ConnectedTag[6];
	// 	memcpy (ConnectedTag,"listen",6);
	// 	send_cmd((PC_CMD_DATA*)ConnectedTag,6);
	// 	BYTE pkg[9];
	// 	for (int i=0; i < 1000; i++)
	// 	{
	// 		DispatchCMD((PC_CMD_REPLY*)pkg);
	// 		if (0 == memcmp (pkg,"connected",9))	//����ͨ�ųɹ�
	// 			break;
	// 		else									//����ͨ��ʧ��
	// 			return IF_TYPE_REGISTER_TAG | REGISTER_TAG_STATE_TIMEOUT;
	// 	}
	//
	// 	//����ͨ�ųɹ��������һ�����ò���
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
		// 		//�ͷ����ǩ������ͨ��
		// 		BYTE UnconnectedTag[4];
		// 		memcpy (UnconnectedTag,"exit",4);
		// 		send_cmd((PC_CMD_DATA*)UnconnectedTag,4);

		PRINT ("Dispatch Error! @rfRegTag \n");
		return IF_TYPE_REGISTER_TAG | REGISTER_TAG_STATE_FAILED;
	}
	// 	//�ͷ����ǩ������ͨ��
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
*�������ƣ�rfCleanAllTag
*�������ܣ�������е��Ѿ�ע��ı�ǩ��Ϣ��
*����������
*����״̬��
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
*�������ƣ�rfMultiTagAlert
*�������ܣ����ǩ������
*����������
*����״̬��
*/
RF_STATUS rfMultiTagAlert(IN  TID_INFO*	tidArray,	//��ǩ����
						  IN  DWORD		tagNums,	//��ǩ����
						  IN  BYTE		openOrClose	//�򿪻�رգ�0��1�գ�
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
*�������ƣ�rfFileDelete
*�������ܣ��ļ�ɾ��
*����������
*����״̬��
*��ע:
*/
RF_STATUS rfFileDelete()
{
	if (selectedFile.fileID == 0x3f00 ||
		selectedFile.fileID == 0xf000 ||
		selectedFile.fileID == 0xa101 ||
		selectedFile.fileID == 0xa102 ||
		selectedFile.fileID == 0xa103)
	{
		PRINT("ϵͳ�ļ�����ɾ��\n");
		return IF_TYPE_FILE_DELETE | FILE_DELETE_STATE_SYSFILE;
	}

	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_FILE_DELETE);

	send_cmd(&pcCmdData);

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_FILE_DELETE, 3*TIME_OUT))
	{
		PRINT ("DispatchCMD �ļ�ɾ����ʱ!\n");
		return IF_TYPE_FILE_DELETE | FILE_DELETE_STATE_TIMEOUT;
	}
	switch (pcCmdReply.uReplyParam.fileDeleteParam.fileDeleteState)
	{
	case FILE_DELETE_STATE_SUCCESS:
		PRINT ("�ļ�ɾ���ɹ�\n");

		return IF_TYPE_FILE_DELETE | pcCmdReply.uReplyParam.fileDeleteParam.fileDeleteState;
	case FILE_DELETE_STATE_TIMEOUT:
		PRINT ("�ļ�ɾ����ʱ\n");
		return IF_TYPE_FILE_DELETE | pcCmdReply.uReplyParam.fileDeleteParam.fileDeleteState;
	default:
		PRINT ("�ļ�ɾ��δ֪����\n");
		return IF_TYPE_FILE_DELETE | FILE_DELETE_STATE_UNKNOWN_ERR;
	}
}

/******************************************************
*�������ƣ�rfFileSysFormat
*�������ܣ��ļ�ϵͳ��ʽ��
*����������
*����״̬��
*��ע:
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
		PRINT ("DispatchCMD �ļ�ϵͳ��ʽ����ʱ!\n");
		return IF_TYPE_FILESYS_FORMAT | FILESYS_FORMAT_STATE_TIMEOUT;
	}
	switch (pcCmdReply.uReplyParam.fileDeleteParam.fileDeleteState)
	{
	case FILESYS_FORMAT_STATE_SUCCESS:
		PRINT ("�ļ�ϵͳ��ʽ���ɹ�\n");
		return IF_TYPE_FILESYS_FORMAT | FILESYS_FORMAT_STATE_SUCCESS;
	case FILESYS_FORMAT_STATE_TIMEOUT:
		PRINT ("�ļ�ϵͳ��ʽ����ʱ\n");
		return IF_TYPE_FILESYS_FORMAT | FILESYS_FORMAT_STATE_TIMEOUT;
	case FILESYS_FORMAT_STATE_PWD_ERROR:
		PRINT ("�ļ�ϵͳ��ʽ��������������\n");
		return IF_TYPE_FILESYS_FORMAT | FILESYS_FORMAT_STATE_PWD_ERROR;
	}
	return 0;
}

/******************************************************
*�������ƣ�rfImpoReaderPriKey
*�������ܣ������Ķ���˽Կ�����Ķ�����Կ����������
*����������
*����״̬��
*��ע:
*/
RFIDLIBDLL RF_STATUS rfImpoReaderPriKey(OUT	BYTE	pubKeyReader[PKI_PUBLICKEY_LEN],	//�Ķ�����Կ
										OUT	DWORD*	pubLenReader						//�Ķ�����Կ����
										)
{
	int Info;
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_SET_READER_INFO);
	BYTE	priKeyReader[PKI_PRIVATEKEY_LEN]={0};	//�Ķ���˽Կ
	DWORD	priLenReader;						//�Ķ���˽Կ����
	//BYTE	ecdh[0x50];							//�Ķ���ECDH
	unsigned char tmpRand[8];					//���������
	//	FILE *fp;									//���ڱ����ļ�
	srand((unsigned int)(time(NULL)));			//������������ӣ�����ʱ�䣩
	tmpRand[0] = rand();
	tmpRand[1] = rand();
	tmpRand[2] = rand();
	tmpRand[3] = rand();
	tmpRand[4] = rand();
	tmpRand[5] = rand();
	tmpRand[6] = rand();
	tmpRand[7] = rand();

	//	makeECDH(tmpRand,ecdh);				//����ECDH

	makeKey(tmpRand,pubKeyReader,pubLenReader,priKeyReader,&priLenReader);		//�����Ķ�����Կ

	pcCmdData.uParam.SET_READER_INFO_PARAM.paraType = 1; // 0���������� 1��ECC_key����
	memcpy(pcCmdData.uParam.SET_READER_INFO_PARAM.u.priKeyReader,priKeyReader,PKI_PRIVATEKEY_LEN);

	//Info = send_cmd(&pcCmdData,sizeof(pcCmdData));			//��һ���Ŀ����Ҫ�Ǵ�ͨ��
	Info = send_cmd(&pcCmdData);
	if (0 == dispatch_cmd(&pcCmdReply,PC_CMD_TYPE_SET_READER_INFO,INFINITE))
	{
		return IF_TYPE_SET_RADER_INFO | SET_READER_INFO_TIMEOUT;
	}
	switch(pcCmdReply.uReplyParam.setReaderInfoParam.setReaderInfoState)
	{
	case SET_READER_INFO_STATE_SUCCESS:
		PRINT ("rfImpoReaderPriKey�ɹ�\n");
		return IF_TYPE_SET_RADER_INFO | SET_READER_INFO_STATE_SUCCESS;
	case SET_READER_INFO_STATE_FAILED:
		PRINT ("rfImpoReaderPriKeyʧ��\n");
		return IF_TYPE_SET_RADER_INFO | SET_READER_INFO_STATE_FAILED;
	default:
		return IF_TYPE_SET_RADER_INFO | SET_READER_INFO_TIMEOUT;
	}

	// 	fp = fopen ("ReaderPubKey","w+");
	// 	fwrite (pubKeyReader,sizeof(U8),*pubLenReader + sizeof(U32),fp);			//���Ķ�����Կд�뱾���ļ�
	// 	fclose (fp);
}

/******************************************************
*�������ƣ�rfImpoTagPriReaderPub
*�������ܣ������ǩ˽Կ������ǩ��Կ���������أ������Ķ�����Կ
*����������
*����״̬��
*��ע:
*/
RFIDLIBDLL RF_STATUS rfImpoTagPriReaderPub(IN  BYTE	  TID[8],							//�����ǩID
										   IN  BYTE	  pubKeyReader[PKI_PUBLICKEY_LEN],	//�Ķ�����Կ
										   OUT BYTE	  pubKeyTag[PKI_PUBLICKEY_LEN],		//��ǩ��Կ
										   OUT DWORD* pubLenTag							//��ǩ��Կ����
										   )
{
	int Info;
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_INIT_TAG);
	BYTE	priKeyTag[PKI_PRIVATEKEY_LEN]={0};		//��ǩ˽Կ
	DWORD	priLenTag;							//��ǩ˽Կ����

	unsigned char tmpRand[8];					//���������
	//	FILE *fp;									//���ڱ����ļ�
	srand((unsigned int)(time(NULL)));			//������������ӣ�����ʱ�䣩
	tmpRand[0] = rand();
	tmpRand[1] = rand();
	tmpRand[2] = rand();
	tmpRand[3] = rand();
	tmpRand[4] = rand();
	tmpRand[5] = rand();
	tmpRand[6] = rand();
	tmpRand[7] = rand();

	makeKey(tmpRand,pubKeyTag,pubLenTag,priKeyTag,&priLenTag);		//���ɱ�ǩ��Կ

	pcCmdData.uParam.TAG_ATTR_PARAM.paraType = 1; //0���������� 1��ECC_key����
	memcpy(pcCmdData.uParam.TAG_ATTR_PARAM.ATTR.ECC_KEY.priKeyTag,priKeyTag,PKI_PUBLICKEY_LEN);
	memcpy(pcCmdData.uParam.TAG_ATTR_PARAM.ATTR.ECC_KEY.pubKeyReader,pubKeyReader,PKI_PUBLICKEY_LEN);

	// 	Info = send_cmd(&pcCmdData,sizeof(pcCmdData)); //��һ���Ŀ���ǽ���ͨ��
	// 	PRINT ("����ͨ��.\n");
	// 	Sleep(500);
	// 	Info = send_cmd(&pcCmdData,sizeof(pcCmdData));
	// 	Sleep(500);
	// 	Info = send_cmd(&pcCmdData,sizeof(pcCmdData));
	// 	initCmdData(pcCmdData,PC_CMD_TYPE_DISCONNECT_TAG);			//���Ҫ�Ͽ�ͨ��
	// 	PRINT("�ر�ͨ��.\n");
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
		PRINT ("rfImpoTagPriReaderPub�ɹ�\n");
		return IF_TYPE_INIT_TAG | INIT_TAG_STATE_SUCCESS;
	case INIT_TAG_STATE_FAILED:
		PRINT ("rfImpoTagPriReaderPubʧ��\n");
		return IF_TYPE_INIT_TAG | INIT_TAG_STATE_FAILED;
	default:
		return IF_TYPE_INIT_TAG | INIT_TAG_STATE_TIMEOUT;
	}

	//fp = fopen ((LPCTSTR)TID,"w+");
	//fwrite (pubKeyTag,sizeof(U8),*pubLenTag + sizeof(U32),fp);			//��ǩ����Կд�뱾���ļ�
	//fclose (fp);
}

/******************************************************
*�������ƣ�rfSetChannel
*�������ܣ������Ķ������ǩ�ŵ�
*����������
*����״̬��
*��ע:
*/
RFIDLIBDLL RF_STATUS rfSetChannel(IN  BYTE	newChannel,	//��Χ��0��15
								  IN  BYTE	readerOrTag	//0:Reader,1:Tag
								  )
{
	if (0 == readerOrTag)		//����Reader�ŵ�
	{
		if ((newChannel > 15) || (newChannel <0))
		{
			PRINT ("ע�⣺�����Ķ����ŵ�����0-15��Χ\n");
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
			PRINT("���������Ķ����ŵ�����ʧ��\n");
			return IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_FAILED;
		}
		if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_SELLECT_CHANNEL, 3*TIME_OUT))
		{
			PRINT ("�����Ķ����ŵ�δ�յ��ظ�����ʱ\n");
			return IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_TIMEOUT;
		}

		switch(pcCmdReply.uReplyParam.setChannelParam.setChannelState)
		{
		case SET_CHANNEL_SUCCESS:
			PRINT ("�����Ķ����ŵ��ɹ�\n");
			return IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_SUCCESS;
		case SET_CHANNEL_FAILED:
			PRINT ("�����Ķ����ŵ�ʧ��\n");
			return IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_FAILED;
		default:
			return IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_FAILED;
		}
	}
	else if (1 == readerOrTag)		//����Tag�ŵ�
	{
		if ((newChannel > 15) || (newChannel <0))
		{
			PRINT ("ע�⣺���ñ�ǩ�ŵ�����0-15��Χ\n");
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
			PRINT("�������ñ�ǩ�ŵ�����ʧ��");
			return IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_FAILED;
		}
		if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_SET_CHANNEL_TAG, 3*TIME_OUT))
		{
			PRINT ("���ñ�ǩ�ŵ�δ�յ��ظ�����ʱ\n");
			return IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_TIMEOUT;
		}

		switch(pcCmdReply.uReplyParam.setChannelParam.setChannelState)
		{
		case SET_CHANNEL_SUCCESS:
			PRINT ("���ñ�ǩ�ŵ��ɹ�\n");
			return IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_SUCCESS;
		case SET_CHANNEL_FAILED:
			PRINT ("���ñ�ǩ�ŵ�ʧ��\n");
			return IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_FAILED;
		default:
			return IF_TYPE_SELLECT_CHANNEL | SET_CHANNEL_FAILED;
		}
	}
	return 0;
}

/******************************************************
*�������ƣ�rfSetPower
*�������ܣ����÷��书��
*����������
*����״̬��
*��ע:
*/
RFIDLIBDLL RF_STATUS rfSetPower(IN	BYTE  newPower, 	//�Ķ�����Χ��0��6����ǩ��Χ��0��8
								IN	BYTE  readerOrTag	//0:Reader,1:Tag
								)
{
	if (0 == readerOrTag)		//����Reader����
	{
		if ((newPower > 6) || (newPower < 0))
		{
			PRINT ("ע�⣺�����Ķ������ʳ���0-6��Χ\n");
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
			PRINT("���������Ķ�����������ʧ��\n");
			return IF_TYPE_SET_POWER | SET_POWER_STATE_FAILED;
		}
		if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_SET_POWER, 3*TIME_OUT))
		{
			PRINT ("�����Ķ�������δ�յ��ظ�����ʱ\n");
			return IF_TYPE_SET_POWER | SET_POWER_STATE_TIMEOUT;
		}

		switch(pcCmdReply.uReplyParam.setPowerParam.setPowerState)
		{
		case SET_POWER_STATE_SUCCESS:
			PRINT ("�����Ķ������ʳɹ�\n");
			return IF_TYPE_SET_POWER | SET_POWER_STATE_SUCCESS;
		case SET_POWER_STATE_FAILED:
			PRINT ("�����Ķ�������ʧ��\n");
			return IF_TYPE_SET_POWER | SET_POWER_STATE_FAILED;
		default:
			PRINT ("�����Ķ������ʳ�ʱ\n");
			return IF_TYPE_SET_POWER | SET_POWER_STATE_FAILED;
		}
	}
	else if (1 == readerOrTag)		//����Tag����
	{
		if ((newPower > 8) || (newPower < 0))
		{
			PRINT ("ע�⣺���ñ�ǩ���ʳ���0-8��Χ\n");
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
			PRINT ("���ñ�ǩ����δ�յ��ظ�����ʱ\n");
			return IF_TYPE_SET_POWER_TAG | SET_POWER_STATE_TIMEOUT;
		}

		switch(pcCmdReply.uReplyParam.setPowerParam.setPowerState)
		{
		case SET_POWER_STATE_SUCCESS:
			PRINT ("���ñ�ǩ���ʳɹ�\n");
			return IF_TYPE_SET_POWER_TAG | SET_POWER_STATE_SUCCESS;
		case SET_POWER_STATE_FAILED:
			PRINT ("���ñ�ǩ����ʧ��\n");
			return IF_TYPE_SET_POWER_TAG | SET_POWER_STATE_FAILED;
		default:
			PRINT ("���ñ�ǩ���ʳ�ʱ\n");
			return IF_TYPE_SET_POWER_TAG | SET_POWER_STATE_FAILED;
		}
	}
	return 0;
}


/******************************************************
*�������ƣ�rfKillTag
*�������ܣ���ǩ���
*����������
*����״̬��
*��ע:
*/
RFIDLIBDLL RF_STATUS rfKillTag(IN  DWORD  killTagToken)		//��ǩ���
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_KILL_TAG);
	pcCmdData.uParam.KILL_TAG_PARAM.killTagToken = killTagToken;

	int Info = send_cmd(&pcCmdData);

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_KILL_TAG, 3*TIME_OUT))
	{
		PRINT ("DispatchCMD ��ǩ��ʱ\n");
		return IF_TYPE_KILL_TAG | KILL_TAG_TIMEOUT;
	}
	switch (pcCmdReply.uReplyParam.killTagParam.killTagstate)
	{
	case KILL_TAG_SUCCESS:
		PRINT ("��ǩ���ɹ�\n");
		return IF_TYPE_KILL_TAG | KILL_TAG_SUCCESS;
	case KILL_TAG_FAILED:
		PRINT ("��ǩ���ʧ��\n");
		return IF_TYPE_KILL_TAG | KILL_TAG_FAILED;
	case KILL_TAG_TIMEOUT:
		PRINT ("��ǩ��ʱ\n");
		return IF_TYPE_KILL_TAG | KILL_TAG_TIMEOUT;
	case 0x0902:
		PRINT("��ǩ������Ϊ�㣬�������\n");
		return IF_TYPE_KILL_TAG | 3;
	}
	return 0;
}

/******************************************************
*�������ƣ�rfWritecacheSync
*�������ܣ�д����ͬ��
*����������
*����״̬��
*��ע:
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
		PRINT ("д����ͬ���ɹ�\n");
		return IF_TYPE_WRITECACHE_SYNC | WRITECACHE_SYNC_SUCCESS;
	case WRITECACHE_SYNC_FAILED:
		PRINT ("д����ͬ��ʧ��\n");
		return IF_TYPE_WRITECACHE_SYNC | WRITECACHE_SYNC_FAILED;
	case WRITECACHE_SYNC_TIMEOUT:
		return IF_TYPE_WRITECACHE_SYNC | WRITECACHE_SYNC_TIMEOUT;
	}
	return 0;
}

/******************************************************
*�������ƣ�rfWriteCache
*�������ܣ���д͸���ļ������ݴ����Ķ���Flash��
*����������
*����״̬��
*��ע:
*/
RFIDLIBDLL RF_STATUS rfWriteCache(IN  WORD 	FileOffset,	  //�ļ�ƫ����
								  IN  DWORD WriteLength,  //д�ļ��ĳ���
								  IN  char* pData		  //��д���ļ������ݻ�����
								  )
{
	if (selectedFile.fileID == NULL)
	{
		PRINT ("Select file first and do next! @ rfWriteCache\n");
		return IF_TYPE_WRITE_CACHE | WRITE_CACHE_FAILED;
	}//ûѡ�ļ�

	if ((selectedFile.securityType & TOKEN_UPDATE_MASK) == TOKEN_UPDATE_STATE_PWD)
	{
		if (false == pwdState.isUpdatePwdPassed)
		{
			PRINT ("Check your Password first! @ rfWriteCache\n");
			return IF_TYPE_WRITE_CACHE | WRITE_CACHE_FAILED;
		}
	}//��Ҫ����

	if ((selectedFile.securityType & TOKEN_UPDATE_MASK) == TOKEN_UPDATE_STATE_ATH)
	{
		if ((rfSafeSession() != (IF_TYPE_SAFE_SESSION | SAFE_SESSION_STATE_SUCCESS)) &&
			(rfSafeSession() != (IF_TYPE_SAFE_SESSION | SAFE_SESSION_STATE_ALREADY_IN)))
		{
			PRINT ("Need Verify but not pass verify.  @ rfWriteCache\n");
			return IF_TYPE_WRITE_CACHE | WRITE_CACHE_FAILED;
		}
	}//��ҪУ��

	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	int BufOffset = 0;
	WORD writedLength = 0;
	int Info;
	int length = WriteLength;
	int count = 0;
	int index = 1;//���������ţ���1��ʼ

	initCmdData(pcCmdData,PC_CMD_TYPE_WRITE_CACHE);
	pcCmdData.uParam.WRITE_CACHE_PARAM.fileOffset = FileOffset;
	pcCmdData.uParam.WRITE_CACHE_PARAM.dataLen = 0;
	memset (pcCmdData.uParam.WRITE_CACHE_PARAM.dataBuffer,0,sizeof(pcCmdData.uParam.WRITE_CACHE_PARAM.dataBuffer));
	Info = send_cmd(&pcCmdData);			//��̽��

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

		//PRINT ("��ǰ���͵İ���Ϊ%d\n",index++);
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
		PRINT ("д����ɹ�\n");
		return IF_TYPE_WRITE_CACHE | WRITE_CACHE_SUCCESS;
	case WRITE_CACHE_FAILED:
		PRINT ("д����ʧ��\n");
		return IF_TYPE_WRITE_CACHE | WRITE_CACHE_FAILED;
	case WRITE_CACHE_TIMEOUT:
		PRINT ("д���泬ʱ\n");
		return IF_TYPE_WRITE_CACHE | WRITE_CACHE_TIMEOUT;
	}
	return 0;
}

/******************************************************
*�������ƣ�rfWriteLargeCache
*�������ܣ���д���ļ�������64K�������ݴ����Ķ���Flash��
*����������
*����״̬��
*��ע:
*/
RFIDLIBDLL RF_STATUS rfWriteLargeCache(IN  DWORD  fileOffset,	//�ļ�ƫ����
									   IN  DWORD  dataLen,		//���ݳ���
									   OUT DWORD* retLen,		//����д������ݳ���
									   IN  BYTE   buff[]		//д���buffer
									   )
{
	if (dataLen > 240*1024)
	{
		return IF_TYPE_WRITE_CACHE | WRITE_CACHE_OVERRANGE;
	}
	int subFileIndex;
	int creatFileNums;//�贴�����ļ�����
	FILE_ATTRIBUTE_W fileToSelect;
	DWORD lengthToWrite;
	RF_STATUS ret;
	WORD tmp_fileID;

	*retLen = 0;
	memset(&fileToSelect, 0, sizeof FILE_ATTRIBUTE_W);
	tmp_fileID = selectedFile.fileID;

	PRINT ("Ready to write %d bytes.\n",dataLen);

	subFileIndex = 0;
	while (fileOffset >= MAX_FILE_LENGTH_IN_TAG)//��λ��ȡ��ʼλ��
	{
		subFileIndex++;
		fileOffset -= MAX_FILE_LENGTH_IN_TAG;
	}
	creatFileNums = dataLen/MAX_FILE_LENGTH_IN_TAG;	//��һ���ļ��Ѵ���
	for (int i=0; i < creatFileNums; i++)//����ʣ�µ��ļ�
	{
		subFileIndex++;
		fileToSelect.fileAttr_R.securityType = 0;//ʼ���������
		fileToSelect.fileAttr_R.fileID = selectedFile.fileID + subFileIndex;
		fileToSelect.fileAttr_R.fileTypeValid = 0x01;//��Ч��͸��
		fileToSelect.fileAttr_R.uFileAttr.transBinEF.maxFileLen = MAX_FILE_LENGTH_IN_TAG;
		fileToSelect.fileAttr_R.uFileAttr.transBinEF.constMaxRecord = 1;
		fileToSelect.fileAttr_R.uFileAttr.transBinEF.realFileLen = 0;
		fileToSelect.access_tokens.transBinEF.read_token = 0;
		fileToSelect.access_tokens.transBinEF.update_token = 0;
		fileToSelect.access_tokens.transBinEF.valid_token = 0;
		rfCreateFile(&fileToSelect);
	}


	// 	ret = rfSelectFile((BYTE*)TID, selectedFile.fileID + subFileIndex, &fileToSelect.fileAttr_R);
	// 	if (SELECT_FILE_STATE_SUCCESS != (ret & 0xFF))//ѡ���ļ�����
	// 	{
	// 		PRINT("ѡ���ļ�����\n");
	// 		return IF_TYPE_SELECT_FILE | SELECT_FILE_STATE_TIMEOUT;
	// 	}

	do
	{
		if (fileOffset + dataLen > MAX_FILE_LENGTH_IN_TAG)//���д������Խ�����ǩ�ļ�����һ��ֻд��һ���ļ�
		{
			lengthToWrite = dataLen - (fileOffset + dataLen - MAX_FILE_LENGTH_IN_TAG);
		}
		else
		{
			lengthToWrite = dataLen;
		}

		PRINT ("Send %d bytes to Reader.\n",lengthToWrite);

		ret = rfWriteCache((WORD)fileOffset, lengthToWrite,  (char*)(buff + *retLen));
		if (WRITE_CACHE_SUCCESS != (ret & 0xFF))//д�����
		{
			PRINT("д�����\n");
			return IF_TYPE_WRITE_CACHE | (ret & 0xFF);
		}

		fileOffset = 0;
		dataLen -= lengthToWrite;
		*retLen += lengthToWrite;
	}
	while (dataLen > 0);

	PRINT("д���ļ�����ɹ�\n");
	//rfSelectFile((BYTE*)TID, tmp_fileID, &selectedFile);
	return IF_TYPE_WRITE_CACHE | WRITE_CACHE_SUCCESS;
}

/******************************************************
*�������ƣ�rfSendCache
*�������ܣ����Ķ���Flash�е����ݷ��ͳ�ȥ������ǩ��
*����������
*����״̬��
*��ע:
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
		PRINT ("rfSendCacheû�յ�\n");
		return IF_TYPE_SEND_CACHE | SEND_CACHE_TIMEOUT;
	}
	switch(pcCmdReply.uReplyParam.sendCacheParam.sendCacheState)
	{
	case SEND_CACHE_SUCCESS:
		PRINT ("���ͻ���ɹ�\n");
		return IF_TYPE_SEND_CACHE | SEND_CACHE_SUCCESS;
	case SEND_CACHE_FAILED:
		PRINT ("���ͻ���ʧ��\n");
		return IF_TYPE_SEND_CACHE | SEND_CACHE_FAILED;
	case SEND_CACHE_TIMEOUT:
		PRINT ("���ͻ��泬ʱ\n");
		return IF_TYPE_SEND_CACHE | SEND_CACHE_TIMEOUT;
	}
	return 0;
}

/******************************************************
*�������ƣ�rfCleanCache
*�������ܣ��������
*����������
*����״̬��
*��ע:
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
		PRINT ("rfCleanCacheû�յ�\n");
		return IF_TYPE_CLEAN_CACHE | CLEAN_CACHE_TIMEOUT;
	}
	switch(pcCmdReply.uReplyParam.cleanCacheParam.cleanCacheState)
	{
	case CLEAN_CACHE_SUCCESS:
		PRINT ("�������ɹ�\n");
		return IF_TYPE_CLEAN_CACHE | CLEAN_CACHE_SUCCESS;
	case CLEAN_CACHE_FAILED:
		PRINT ("�������ʧ��\n");
		return IF_TYPE_CLEAN_CACHE | CLEAN_CACHE_FAILED;
	case CLEAN_CACHE_TIMEOUT:
		PRINT ("������泬ʱ\n");
		return IF_TYPE_CLEAN_CACHE | CLEAN_CACHE_TIMEOUT;
	}
	return 0;
}

/******************************************************
*�������ƣ�rfSendPurewave
*�������ܣ����ô��ز�
*����������
*����״̬��
*��ע:
*/
RFIDLIBDLL RF_STATUS rfSendPurewave()
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_SEND_PUREWAVE);

	int Info = send_cmd(&pcCmdData);

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_SEND_PUREWAVE, 3*TIME_OUT))
	{
		PRINT ("DispatchCMD ���䴿�ز���ʱ\n");
		return IF_TYPE_SEND_PUREWAVE | SEND_PUREVAVE_STATE_TIMEOUT;
	}

	switch (pcCmdReply.uReplyParam.sendPurewaveParam.sendPurewaveState)
	{
	case SEND_PUREWAVE_STATE_SUCCESS:
		PRINT ("���䴿�ز��ɹ�\n");
		return IF_TYPE_SEND_PUREWAVE | SEND_PUREWAVE_STATE_SUCCESS;
	case SEND_PUREWAVE_STATE_FAILED:
		PRINT ("���䴿�ز�ʧ��\n");
		return IF_TYPE_SEND_PUREWAVE | SEND_PUREWAVE_STATE_FAILED;
	case SEND_PUREVAVE_STATE_TIMEOUT:
		PRINT ("���䴿�ز���ʱ\n");
		return IF_TYPE_SEND_PUREWAVE | SEND_PUREVAVE_STATE_TIMEOUT;
	}
	return 0;
}

/******************************************************
*�������ƣ�rfStopPurewave
*�������ܣ�ֹͣ���ز�
*����������
*����״̬��
*��ע:
*/
RFIDLIBDLL RF_STATUS rfStopPurewave()
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_STOP_PUREWAVE);

	int Info = send_cmd(&pcCmdData);

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_STOP_PUREWAVE, 3*TIME_OUT))
	{
		PRINT ("DispatchCMD ֹͣ���ز���ʱ\n");
		return IF_TYPE_STOP_PUREWAVE | STOP_PUREWAVE_STATE_TIMEOUT;
	}
	switch (pcCmdReply.uReplyParam.stopPurewaveParam.stopPurewaveState)
	{
	case STOP_PUREWAVE_STATE_SUCCESS:
		PRINT ("ֹͣ���ز��ɹ�\n");
		return IF_TYPE_STOP_PUREWAVE | STOP_PUREWAVE_STATE_SUCCESS;
	case STOP_PUREWAVE_STATE_FAILED:
		PRINT ("ֹͣ���ز�ʧ��\n");
		return IF_TYPE_STOP_PUREWAVE | STOP_PUREWAVE_STATE_FAILED;
	case STOP_PUREWAVE_STATE_TIMEOUT:
		PRINT ("ֹͣ���ز���ʱ\n");
		return IF_TYPE_STOP_PUREWAVE | STOP_PUREWAVE_STATE_TIMEOUT;
	}
	return 0;
}

/******************************************************
*�������ƣ�rfSendModulInfo
*�������ܣ����������Ϣ
*����������
*����״̬��
*��ע:
*/
RFIDLIBDLL RF_STATUS rfSendModulInfo()
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_SEND_MODUL_INFO);

	int Info = send_cmd(&pcCmdData);

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_SEND_MODUL_INFO, 3*TIME_OUT))
	{
		PRINT ("DispatchCMD ���������Ϣ��ʱ\n");
		return IF_TYPE_SEND_MODUL_INFO | SEND_MODUL_INFO_STATE_TIMEOUT;
	}
	switch (pcCmdReply.uReplyParam.sendModulInfoParam.sendModulInfoState)
	{
	case SEND_MODUL_INFO_STATE_SUCCESS:
		PRINT ("���������Ϣ�ɹ�\n");
		return IF_TYPE_SEND_MODUL_INFO | SEND_MODUL_INFO_STATE_SUCCESS;
	case SEND_MODUL_INFO_STATE_FAILED:
		PRINT ("���������Ϣʧ��\n");
		return IF_TYPE_SEND_MODUL_INFO | SEND_MODUL_INFO_STATE_FAILED;
	case SEND_MODUL_INFO_STATE_TIMEOUT:
		PRINT ("���������Ϣ��ʱ\n");
		return IF_TYPE_SEND_MODUL_INFO | SEND_MODUL_INFO_STATE_TIMEOUT;
	}
	return 0;
}

/******************************************************
*�������ƣ�rfStopModulInfo
*�������ܣ�ֹͣ������Ϣ
*����������
*����״̬��
*��ע:
*/
RFIDLIBDLL RF_STATUS rfStopModulInfo()
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_STOP_MODUL_INFO);

	int Info = send_cmd(&pcCmdData);

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_STOP_MODUL_INFO, 3*TIME_OUT))
	{
		PRINT ("DispatchCMD ֹͣ������Ϣ��ʱ\n");
		return IF_TYPE_STOP_MODUL_INFO | STOP_MODUL_INFO_STATE_TIMEOUT;
	}
	switch (pcCmdReply.uReplyParam.stopModulInfoParam.stopModulInfoState)
	{
	case STOP_MODUL_INFO_STATE_SUCCESS:
		PRINT ("���������Ϣ�ɹ�\n");
		return IF_TYPE_STOP_MODUL_INFO | STOP_MODUL_INFO_STATE_SUCCESS;
	case STOP_MODUL_INFO_STATE_FAILED:
		PRINT ("���������Ϣʧ��\n");
		return IF_TYPE_STOP_MODUL_INFO | STOP_MODUL_INFO_STATE_FAILED;
	case STOP_MODUL_INFO_STATE_TIMEOUT:
		PRINT ("���������Ϣ��ʱ\n");
		return IF_TYPE_STOP_MODUL_INFO | STOP_MODUL_INFO_STATE_TIMEOUT;
	}
	return 0;
}

/******************************************************
*�������ƣ�rfSetRecvModel
*�������ܣ����ý���ģʽ
*����������
*����״̬��
*��ע:
*/
RFIDLIBDLL RF_STATUS rfSetRecvModel()
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_SET_RECV_MODEL);

	int Info = send_cmd(&pcCmdData);

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_SET_RECV_MODEL, 3*TIME_OUT))
	{
		PRINT ("DispatchCMD ���ý���ģʽ��ʱ\n");
		return IF_TYPE_SET_RECV_MODEL | SET_RECV_MODEL_STATE_TIMEOUT;
	}
	switch (pcCmdReply.uReplyParam.setRecvModelParam.setRecvModelState)
	{
	case SET_RECV_MODEL_STATE_SUCCESS:
		PRINT ("���ý���ģʽ�ɹ�\n");
		return IF_TYPE_SET_RECV_MODEL | SET_RECV_MODEL_STATE_SUCCESS;
	case SET_RECV_MODEL_STATE_FAILED:
		PRINT ("���ý���ģʽʧ��\n");
		return IF_TYPE_SET_RECV_MODEL | SET_RECV_MODEL_STATE_FAILED;
	case SET_RECV_MODEL_STATE_TIMEOUT:
		PRINT ("���ý���ģʽ��ʱ\n");
		return IF_TYPE_SET_RECV_MODEL | SET_RECV_MODEL_STATE_TIMEOUT;
	}
	return 0;
}

/******************************************************
*�������ƣ�rfStopRecvModel
*�������ܣ�ֹͣ����ģʽ
*����������
*����״̬��
*��ע:
*/
RFIDLIBDLL RF_STATUS rfStopRecvModel()
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_STOP_RECV_MODEL);

	int Info = send_cmd(&pcCmdData);
	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_STOP_RECV_MODEL, 3*TIME_OUT))
	{
		PRINT ("DispatchCMD ֹͣ����ģʽ��ʱ\n");
		return IF_TYPE_STOP_RECV_MODEL | STOP_RECV_MODEL_STATE_TIMEOUT;
	}
	switch (pcCmdReply.uReplyParam.stopRecvModelParam.stopRecvModelState)
	{
	case SET_RECV_MODEL_STATE_SUCCESS:
		PRINT ("ֹͣ����ģʽ�ɹ�\n");
		return IF_TYPE_STOP_RECV_MODEL | SET_RECV_MODEL_STATE_SUCCESS;
	case SET_RECV_MODEL_STATE_FAILED:
		PRINT ("ֹͣ����ģʽʧ��\n");
		return IF_TYPE_STOP_RECV_MODEL | SET_RECV_MODEL_STATE_FAILED;
	case STOP_RECV_MODEL_STATE_TIMEOUT:
		PRINT ("ֹͣ����ģʽ��ʱ\n");
		return IF_TYPE_STOP_RECV_MODEL | STOP_RECV_MODEL_STATE_TIMEOUT;
	}
	return 0;
}


/******************************************************
*�������ƣ�rfMakeKey
*�������ܣ���������Ķ������ǩ�Ĺ�Կ��˽Կ
*����������
*����״̬��
*��ע:
*/
RFIDLIBDLL RF_STATUS rfMakeKey(	OUT	BYTE	w_pubKeyReader[PKI_PUBLICKEY_LEN],	//�Ķ�����Կ
								OUT	DWORD*	w_pubLenReader,						//�Ķ�����Կ����
								OUT	BYTE	w_priKeyReader[PKI_PRIVATEKEY_LEN],	//�Ķ���˽Կ
								OUT DWORD*	w_priLenReader,						//�Ķ���˽Կ����
								OUT BYTE	w_pubKeyTag[PKI_PUBLICKEY_LEN],		//��ǩ��Կ
								OUT DWORD*	w_pubLenTag,						//��ǩ��Կ����
								OUT BYTE	w_priKeyTag[PKI_PRIVATEKEY_LEN],	//��ǩ˽Կ
								OUT DWORD*	w_priLenTag							//��ǩ˽Կ����
								)
{
	unsigned char tmpRand[8];				//���������
	FILE *fp;								//���ڱ����ļ�

	unsigned char ecdh[0x42] = {0};			//ECDH����
	unsigned char pubKeyReader[69] = {0};	//�Ķ�����Կ����
	unsigned char priKeyReader[99] = {0};	//�Ķ���˽Կ����
	unsigned long pubLenReader = 0;			//�Ķ�����Կ����
	unsigned long priLenReader = 0;			//�Ķ���˽Կ����

	unsigned char pubKeyTag[69] = {0};		//��ǩ��Կ����
	unsigned char priKeyTag[99] = {0};		//��ǩ˽Կ����
	unsigned long pubLenTag = 0;			//��ǩ��Կ����
	unsigned long priLenTag = 0;			//��ǩ˽Կ����

	//*********************�������Ķ�����Կ*******************************
	srand((unsigned int)(time(NULL)));			//������������ӣ�����ʱ�䣩
	tmpRand[0] = rand();
	tmpRand[1] = rand();
	tmpRand[2] = rand();
	tmpRand[3] = rand();
	tmpRand[4] = rand();
	tmpRand[5] = rand();
	tmpRand[6] = rand();
	tmpRand[7] = rand();

	//makeECDH(tmpRand,ecdh);				//����ECDH

	makeKey(tmpRand,pubKeyReader,&pubLenReader,priKeyReader,&priLenReader);		//�����Ķ�����Կ

	fp = fopen ("ReaderPubKey","wb");
	fwrite (pubKeyReader,sizeof(U8),pubLenReader + sizeof(U32),fp);			//���Ķ�����Կд�뱾���ļ�
	fclose (fp);

	fp = fopen ("ReaderPriKey","wb");
	fwrite (priKeyReader,sizeof(U8),priLenReader + sizeof(U32),fp);			//���Ķ���˽Կд�뱾���ļ�
	fclose (fp);


	//*********************�����ɱ�ǩ��Կ*******************************
	srand((unsigned int)(time(NULL)));			//������������ӣ�����ʱ�䣩
	tmpRand[0] = rand();
	tmpRand[1] = rand();
	tmpRand[2] = rand();
	tmpRand[3] = rand();
	tmpRand[4] = rand();
	tmpRand[5] = rand();
	tmpRand[6] = rand();
	tmpRand[7] = rand();

	makeKey (tmpRand,pubKeyTag,&pubLenTag,priKeyTag,&priLenTag);		//���ɱ�ǩ��Կ

	fp = fopen ("TagPubKey","wb");
	fwrite (pubKeyTag,sizeof(U8),pubLenTag + sizeof(U32),fp);			//����ǩ��Կд�뱾���ļ�
	fclose (fp);

	fp = fopen ("TagPriKey","wb");
	fwrite (priKeyTag,sizeof(U8),priLenTag + sizeof(U32),fp);			//����ǩ˽Կд�뱾���ļ�
	fclose (fp);

	//-------���Ķ���������������----------
	memcpy(w_pubKeyReader,pubKeyReader,PKI_PUBLICKEY_LEN);
	memcpy(w_priKeyReader,priKeyReader,PKI_PRIVATEKEY_LEN);
	*w_pubLenReader = pubLenReader + sizeof(U32);
	*w_priLenReader = priLenReader + sizeof(U32);
	//--------����ǩ������������---------
	memcpy(w_pubKeyTag,pubKeyTag,PKI_PUBLICKEY_LEN);
	memcpy(w_priKeyTag,priKeyTag,PKI_PRIVATEKEY_LEN);
	*w_pubLenTag = pubLenTag + sizeof(U32);
	*w_priLenTag = priLenTag + sizeof(U32);

	return 0;
}

/******************************************************
*�������ƣ�rfWirelessSendKey
*�������ܣ����з����Ķ������ǩ�Ĺ�Կ��˽Կ
*����������
*����״̬��
*��ע:
*/
RFIDLIBDLL RF_STATUS rfWirelessSendKey(	IN	BYTE	w_pubKeyReader[PKI_PUBLICKEY_LEN],	//�Ķ�����Կ
										IN	DWORD	w_pubLenReader,						//�Ķ�����Կ����
										IN	BYTE	w_priKeyReader[PKI_PRIVATEKEY_LEN],	//�Ķ���˽Կ
										IN	DWORD	w_priLenReader,						//�Ķ���˽Կ����
										IN	BYTE	w_pubKeyTag[PKI_PUBLICKEY_LEN],		//��ǩ��Կ
										IN	DWORD	w_pubLenTag,						//��ǩ��Կ����
										IN	BYTE	w_priKeyTag[PKI_PRIVATEKEY_LEN],	//��ǩ˽Կ
										IN	DWORD	w_priLenTag							//��ǩ˽Կ����
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
		PRINT ("DispatchCMD ��д�Ķ�����ʱ\n");
		return IF_TYPE_WIRELESS_SET_KEY | WIRELESS_SET_KEY_STATE_TIMEOUT;
	}
	if ((pcCmdReply.cmdType != PC_CMD_TYPE_SET_KEY) ||
		(pcCmdReply.uReplyParam.wirelessSetKeyParam.wirelessSetKeyState == WIRELESS_SET_KEY_STATE_FAILED))
	{
		PRINT ("������дReader��Կʧ�ܣ�������Tag��Կ��д\n");
		return IF_TYPE_WIRELESS_SET_KEY | WIRELESS_SET_KEY_STATE_FAILED;
	}
	PRINT ("������дReader��Կ�ɹ�����������Tag��Կ��д......\n");

	Sleep(10);

	initCmdData(pcCmdData,PC_CMD_TYPE_SET_KEY);
	memset(&pcCmdReply,0,sizeof(PC_CMD_REPLY));
	pcCmdData.uParam.WIRELESS_SET_KEY.paraType = TAG_SET_KEY;
	memcpy(pcCmdData.uParam.WIRELESS_SET_KEY.ECC_KEY.pubKey,w_pubKeyTag,w_pubLenTag);
	memcpy(pcCmdData.uParam.WIRELESS_SET_KEY.ECC_KEY.priKey,w_priKeyTag,w_priLenTag);
	Info = send_cmd(&pcCmdData);
	if (0 == dispatch_cmd(&pcCmdReply,PC_CMD_TYPE_SET_KEY,INFINITE))
	{
		PRINT ("DispatchCMD ��д��ǩ��ʱ\n");
		return IF_TYPE_WIRELESS_SET_KEY | WIRELESS_SET_KEY_STATE_TIMEOUT;
	}
	if ((pcCmdReply.cmdType != PC_CMD_TYPE_SET_KEY) ||
		(pcCmdReply.uReplyParam.wirelessSetKeyParam.wirelessSetKeyState == WIRELESS_SET_KEY_STATE_FAILED))
	{
		PRINT ("������дTag��Կʧ��\n");
		return IF_TYPE_WIRELESS_SET_KEY | WIRELESS_SET_KEY_STATE_FAILED;
	}
	PRINT ("������д��Կ�ɹ���\n");

	return IF_TYPE_WIRELESS_SET_KEY | WIRELESS_SET_KEY_STATE_SUCCESS;
}

/******************************************************
*�������ƣ�rfSearchRecords
*�������ܣ�������¼
*����������
*����״̬��
*��ע:
*/
RFIDLIBDLL RF_STATUS rfSearchRecords(IN  SEARCH_ELEMENT*  searchElement,
									 IN  DWORD	elementNums,
									 OUT WORD*	retRecords
									 )
{
	unsigned char* wPt = (unsigned char*)searchElement;		//����ͷָ��
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
			PRINT ("i = %d ������¼ʧ��,��ϵӦΪ=��G��L��>��<����������һ��\n",i);
			return IF_TYPE_SEARCH_RECORDS | SEARCH_RECORDS_STATE_FAILED;
		}
		if ((searchElement->logicalOperator != 0x41) &&
			(searchElement->logicalOperator != 0x43) &&
			(searchElement->logicalOperator != 0x4f))
		{
			PRINT ("������¼ʧ��,�߼�ӦΪC��A��O������һ��\n");
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
	searchElement = head;						//�û�ͷָ��
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
	PRINT ("��ѯԪ��Ϊ:%x\n",pcCmdData.uParam.SEARCH_RECORDS.dataBuf);
	PRINT ("��ѯ����Ϊ:%d\n",j);
	Info = send_cmd(&pcCmdData);
	if (0 == dispatch_cmd(&pcCmdReply,PC_CMD_TYPE_SEARCH_RECORDS,INFINITE))
	{
		PRINT ("DispatchCMD ������¼��ʱ\n");
		return IF_TYPE_SEARCH_RECORDS | SEARCH_RECORDS_STATE_TIMEOUT;
	}
	switch (pcCmdReply.uReplyParam.searchRecordsParam.searchRecordsState)
	{
	case SEARCH_RECORDS_STATE_SUCCESS:
		PRINT ("������¼�ɹ�\n");
		*retRecords = pcCmdReply.uReplyParam.searchRecordsParam.retRecords;
		return IF_TYPE_SEARCH_RECORDS | SEARCH_RECORDS_STATE_SUCCESS;
	case SEARCH_RECORDS_STATE_FAILED:
		PRINT ("������¼ʧ��\n");
		return IF_TYPE_SEARCH_RECORDS | SEARCH_RECORDS_STATE_FAILED;
	case SEARCH_RECORDS_STATE_TIMEOUT:
		PRINT ("������¼��ʱ\n");
		return IF_TYPE_SEARCH_RECORDS | SEARCH_RECORDS_STATE_TIMEOUT;
	}
	return 0;
}

/******************************************************
*�������ƣ�rfSearchAlltagRecs
*�������ܣ��������б�ǩ��¼
*����������
*����״̬��
*��ע:
*/
RFIDLIBDLL RF_STATUS rfSearchAlltagRecs(IN SEARCH_ELEMENT*	searchElement,
										IN DWORD	elementNums
										)
{
	unsigned char* wPt = (unsigned char*)searchElement;	//����ͷָ��
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
			PRINT ("i = %d ������¼ʧ��,��ϵӦΪ=��G��L��>��<����������һ��\n",i);
			return IF_TYPE_SEARCH_RECORDS | SEARCH_RECORDS_STATE_FAILED;
		}
		if ((searchElement->logicalOperator != LOGICAL_C) &&
			(searchElement->logicalOperator != LOGICAL_A) &&
			(searchElement->logicalOperator != LOGICAL_O))
		{
			PRINT ("������¼ʧ��,�߼�ӦΪC��A��O������һ��\n");
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
	searchElement = head;						//�û�ͷָ��
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
	PRINT ("��ѯԪ��Ϊ:%x\n",pcCmdData.uParam.SEARCH_RECORDS.dataBuf);
	PRINT ("��ѯ����Ϊ:%d\n",j);
	Info = send_cmd(&pcCmdData);
	Sleep(2000);
	PRINT ("�����������");
	return IF_TYPE_SEARCH_RECORDS | SEARCH_RECORDS_STATE_SUCCESS;
}

/******************************************************
*�������ƣ�rfSearchAlltagTrans
*�������ܣ��������б�ǩ͸��
*����������
*����״̬��
*��ע:
*/
RFIDLIBDLL RF_STATUS rfSearchAlltagTrans(IN SEARCH_ELEMENT*	searchElement,
										 IN DWORD	elementNums
										 )
{
	unsigned char* wPt = (unsigned char*)searchElement;		//����ͷָ��
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
			PRINT ("i = %d ������¼ʧ��,��ϵӦΪ=��G��L��>��<����������һ��\n",i);
			return IF_TYPE_SEARCH_RECORDS | SEARCH_RECORDS_STATE_FAILED;
		}
		if ((searchElement->logicalOperator != LOGICAL_C) &&
			(searchElement->logicalOperator != LOGICAL_A) &&
			(searchElement->logicalOperator != LOGICAL_O))
		{
			PRINT ("������¼ʧ��,�߼�ӦΪC��A��O������һ��\n");
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
	searchElement = head; //�û�ͷָ��
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
	PRINT ("��ѯԪ��Ϊ:%x\n",pcCmdData.uParam.SEARCH_RECORDS.dataBuf);
	PRINT ("��ѯ����Ϊ:%d\n",j);
	Info = send_cmd(&pcCmdData);
	Sleep(2000);
	PRINT ("��������");
	return IF_TYPE_SEARCH_RECORDS | SEARCH_RECORDS_STATE_SUCCESS;
}

/******************************************************
*�������ƣ�rfUpdatePwd
*�������ܣ�����ϵͳ����
*����������
*����״̬��
*��ע:
*/
RFIDLIBDLL RF_STATUS rfUpdatePwd(IN  DWORD	adminPWD,	//�ṩ����Ա����
								 IN  WORD	indexPWD,	//��������
								 IN  BYTE	mode,		//indexΪ�ļ���ʾ��ʱʹ�ã������������
								 IN  DWORD	newPWD		//������
								 )
{
	if ((indexPWD>4) && (indexPWD<16128))
	{
		PRINT ("����ϵͳ���������������ģʽ���Ϸ�\n");
		return IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_INVALID_INDEX_OR_MODE;
	}
	if ((indexPWD>=16128) && ((mode<1)||(mode>4)))
	{
		PRINT ("����ϵͳ���������������ģʽ���Ϸ�\n");
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
		PRINT ("DispatchCMD ����ϵͳ���ʱ\n");
		return IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_TIMEOUT;
	}
	switch (pcCmdReply.uReplyParam.updatePWDParam.upDatePwdState)
	{
	case UPDATE_PWD_STATE_SUCCESS:
		PRINT ("����ϵͳ����ɹ�\n");
		return IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_SUCCESS;
	case UPDATE_PWD_STATE_FAILED:
		PRINT ("����ϵͳ����ʧ��\n");
		return IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_FAILED;
	case UPDATE_PWD_STATE_TIMEOUT:
		PRINT ("����ϵͳ���ʱ\n");
		return IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_TIMEOUT;
	case UPDATE_PWD_STATE_ADMIN_PWD_NOT_RIGHT:
		PRINT ("����ϵͳ�������Ա���벻ƥ��\n");
		return IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_ADMIN_PWD_NOT_RIGHT;
	case UPDATE_PWD_STATE_INVALID_NEW_PWD:
		PRINT ("����ϵͳ�����¿���Ϸ�\n");
		return IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_INVALID_NEW_PWD;
	case UPDATE_PWD_STATE_NOT_AUTHORISED:
		PRINT ("����ϵͳ����ڰ�ȫ�Ự\n");
		return IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_NOT_AUTHORISED;
	case UPDATE_PWD_STATE_UNKNOWN:
		PRINT ("����ϵͳ����δ֪����\n");
		return IF_TYPE_UPDATE_PWD | UPDATE_PWD_STATE_UNKNOWN;
	}
	return 0;
}

/******************************************************
*�������ƣ�rfMonitorTrConf
*�������ܣ������������
*����������
*����״̬��
*��ע:
*/
RFIDLIBDLL RF_STATUS rfMonitorTrConf(IN	 WORD	fileID,			 //�ļ�ID
									 IN	 BYTE	fileOffset,		 //�ļ�ƫ��
									 IN	 BYTE	dataLen,		 //���ݳ���
									 IN	 WORD	timeSlotNum,	 //ʱ϶���
									 IN	 WORD	reportedCycle,	 //�ϱ�����
									 IN  WORD	monitorCycle,	 //�������
									 IN	 WORD	timeSlotWidth,	 //ʱ϶���
									 IN	 WORD	timeSlotSum,	 //����ʱ϶����
									 IN	 BYTE	reportIndication, //�շ�ָʾ
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
		PRINT ("DispatchCMD ����������ó�ʱ\n");
		return IF_TYPE_MONITOR_CONF | CONF_MONITOR_TIMEOUT;
	}
	switch (pcCmdReply.uReplyParam.confMonParam.confMonState)
	{
	case CONF_MONITOR_SUCCESS:
		PRINT ("����������óɹ�\n");
		return IF_TYPE_MONITOR_CONF | CONF_MONITOR_SUCCESS;

	case CONF_MONITOR_NOT_MATCH:
		PRINT ("�����������__��������ʶ����ƥ��\n");
		return IF_TYPE_MONITOR_CONF | CONF_MONITOR_NOT_MATCH;
	case CONF_MONITOR_OVERRANGE:
		PRINT ("���������������Խ��\n");
		return IF_TYPE_MONITOR_CONF | CONF_MONITOR_OVERRANGE;
	case CONF_MONITOR_FAILED:
		PRINT ("�����������ʧ��\n");
		return IF_TYPE_MONITOR_CONF | CONF_MONITOR_FAILED;
	case CONF_MONITOR_TIMEOUT:
		PRINT ("����������ó�ʱ\n");
		return IF_TYPE_MONITOR_CONF | CONF_MONITOR_TIMEOUT;
	default:
		PRINT ("�����������δ֪����\n");
		return IF_TYPE_MONITOR_CONF | CONF_MONITOR_UNKNOWN;
	}
}

/******************************************************
*�������ƣ�rfMonitorTrStart
*�������ܣ������ʼ����
*����������
*����״̬��
*��ע:
*/
RFIDLIBDLL RF_STATUS rfMonitorTrStart()
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_MONITOR_START);

	int Info = send_cmd(&pcCmdData);
	if (0 != Info)
	{
		PRINT ("�����ʼ����ʹ���\n");
		return IF_TYPE_MONITOR_START | MONITOR_START_STATE_TIMEOUT;
	}
	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_MONITOR_START, 3*TIME_OUT))
	{
		PRINT("�����ʼ����ظ���ʱ\n");
		return IF_TYPE_MONITOR_START| MONITOR_START_STATE_TIMEOUT;
	}
	switch(pcCmdReply.uReplyParam.monitorStartParam.monitorStartState)
	{
	case MONITOR_START_STATE_SUCCESS:
		PRINT ("�����ʼ�ɹ�\n");
		return IF_TYPE_MONITOR_START | MONITOR_START_STATE_SUCCESS;
	case MONITOR_START_STATE_TIMEOUT:
		PRINT ("�����ʼ��ʱ\n");
		return IF_TYPE_MONITOR_START| MONITOR_START_STATE_TIMEOUT;
	}
	return 0;
}

/******************************************************
*�������ƣ�rfMonitorTrStop
*�������ܣ����ֹͣ����
*����������
*����״̬��
*��ע:
*/
RFIDLIBDLL RF_STATUS rfMonitorTrStop(IN	 WORD  monitorCycle)	//������ڣ���λ���룩
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_MONITOR_STOP);

	int Info = send_cmd(&pcCmdData);
	if (0 != Info)
	{
		PRINT ("���ֹͣ����ʹ���\n");
		return IF_TYPE_MONITOR_STOP | MONITOR_STOP_STATE_TIMEOUT;
	}
	Sleep(monitorCycle*1000);
	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_MONITOR_STOP, 3*TIME_OUT))
	{
		PRINT("���ֹͣ����ظ���ʱ\n");
		return IF_TYPE_MONITOR_STOP| MONITOR_STOP_STATE_TIMEOUT;
	}
	switch(pcCmdReply.uReplyParam.monitorStopParam.monitorStopState)
	{
	case MONITOR_STOP_STATE_SUCCESS:
		PRINT ("���ֹͣ�ɹ�\n");
		return IF_TYPE_MONITOR_STOP | MONITOR_STOP_STATE_SUCCESS;
	case MONITOR_STOP_STATE_TIMEOUT:
		PRINT ("���ֹͣ��ʱ\n");
		return IF_TYPE_MONITOR_STOP| MONITOR_STOP_STATE_TIMEOUT;
	}
	return 0;
}

/******************************************************
*�������ƣ�rfMonitorTrReport
*�������ܣ���������ϱ�
*����������
*����״̬��
*��ע:
*/
RFIDLIBDLL RF_STATUS rfMonitorTrReport(IN  DWORD		waitTime,  //s
									   OUT CYCLE_INFO*	CycleInfo
									   )
{
	if (CycleInfo == NULL)
	{
		PRINT("CycleInfoΪ��ָ��\n");
		return IF_TYPE_MONITOR_REPORT | GET_MONITOR_REPORT_PARAM_ERROR;
	}

	PC_CMD_REPLY pcCmdReply;
	memset(&pcCmdReply,0,sizeof(PC_CMD_REPLY));

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_MONITOR_REPORT, 10*TIME_OUT))
	{
		PRINT ("����ϱ������ݳ�ʱ��\n");
		return IF_TYPE_MONITOR_REPORT | GET_MONITOR_REPORT_TIMEOUT;
	}

	switch(pcCmdReply.uReplyParam.MonRepParam.State)
	{
	case GET_MONITOR_REPORT_SUCCESS:
		*CycleInfo = pcCmdReply.uReplyParam.MonRepParam.cycleInfo;
		break;
	case GET_MONITOR_REPORT_TIMEOUT:
		PRINT ("���ϱ����ݱ�ǩ��\n");
		*CycleInfo = pcCmdReply.uReplyParam.MonRepParam.cycleInfo;
		return IF_TYPE_MONITOR_REPORT | GET_MONITOR_REPORT_TIMEOUT;
	}
	PRINT ("����ϱ��ɹ�\n");
	return IF_TYPE_MONITOR_REPORT | GET_MONITOR_REPORT_SUCCESS;
}

/******************************************************
*�������ƣ�rfUpdateAdminPwd
*�������ܣ����¹���Ա����
*����������
*����״̬��
*��ע:
*/
RFIDLIBDLL RF_STATUS rfUpdateAdminPwd(IN  DWORD  oldPWD, //������
									  IN  DWORD  newPWD	 //������
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
		PRINT ("DispatchCMD ���¹���Ա���볬ʱ\n");
		return IF_TYPE_UPDATE_ADMIN_PWD | UPDATE_ADMIN_PWD_STATE_TIMEOUT;
	}
	switch (pcCmdReply.uReplyParam.updateAdminPwdParam.updateAdminPwdState)
	{
	case UPDATE_ADMIN_PWD_STATE_SUCCESS:
		PRINT("���¹���Ա����ɹ�\n");
		return IF_TYPE_UPDATE_ADMIN_PWD | UPDATE_ADMIN_PWD_STATE_SUCCESS;
	case UPDATE_ADMIN_PWD_STATE_FAILED:
		PRINT("���¹���Ա����ʧ��\n");
		return IF_TYPE_UPDATE_ADMIN_PWD | UPDATE_ADMIN_PWD_STATE_FAILED;
	case UPDATE_ADMIN_PWD_STATE_TIMEOUT:
		PRINT("���¹���Ա���볬ʱ\n");
		return IF_TYPE_UPDATE_ADMIN_PWD | UPDATE_ADMIN_PWD_STATE_TIMEOUT;
	case UPDATE_ADMIN_PWD_STATE_NOT_AUTHORISED:
		PRINT("���¹���Ա���벻�ڰ�ȫ�Ự��\n");
		return IF_TYPE_UPDATE_ADMIN_PWD | UPDATE_ADMIN_PWD_STATE_NOT_AUTHORISED;
	case UPDATE_ADMIN_PWD_STATE_PWD_ERROR:
		PRINT("���¹���Ա������������\n");
		return IF_TYPE_UPDATE_ADMIN_PWD | UPDATE_ADMIN_PWD_STATE_PWD_ERROR;
	}
	return 0;
}

/******************************************************
*�������ƣ�rfGetTagSafeParam
*�������ܣ���ȡ��ǩ��ȫ����
*����������
*����״̬��
*��ע:
*/
RFIDLIBDLL RF_STATUS rfGetTagSafeParam(OUT  WORD*  keyParam,		//��Կ����
									   OUT  WORD*  safePowerParam	//��ȫ��������
									   )
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_GET_TAG_SAFE_PARAM);

	int Info = send_cmd(&pcCmdData);
	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_GET_TAG_SAFE_PARAM, 3*TIME_OUT))
	{
		PRINT("DispatchCMD ��ȡ��ǩ��ȫ������ʱ\n");
		return IF_TYPE_GET_TAGE_SAFE_PARAM | GET_TAG_SAFE_PARAM_STATE_TIMEOUT;
	}
	switch(pcCmdReply.uReplyParam.getTagSafeParam.getTagSafeParamState)
	{
	case GET_TAG_SAFE_PARAM_STATE_SUCCESS:
		PRINT("��ȡ��ǩ��ȫ�����ɹ�\n");
		*keyParam = pcCmdReply.uReplyParam.getTagSafeParam.keyParam;
		*safePowerParam = pcCmdReply.uReplyParam.getTagSafeParam.safePowerParam;
		return IF_TYPE_GET_TAGE_SAFE_PARAM | GET_TAG_SAFE_PARAM_STATE_SUCCESS;
	case GET_TAG_SAFE_PARAM_STATE_FAILED:
		PRINT("��ȡ��ǩ��ȫ����ʧ��\n");
		return IF_TYPE_GET_TAGE_SAFE_PARAM | GET_TAG_SAFE_PARAM_STATE_FAILED;
	default:
		PRINT("��ȡ��ǩ��ȫ����δ֪����\n");
		return IF_TYPE_GET_TAGE_SAFE_PARAM | pcCmdReply.uReplyParam.getTagSafeParam.getTagSafeParamState;
	}
}

/******************************************************
*�������ƣ�rfRequireRandom
*�������ܣ����������
*����������
*����״̬��
*��ע:
*/
RFIDLIBDLL RF_STATUS rfRequireRandom(IN  BYTE randomLenth,	 //��������ȣ���1-4��
									 OUT BYTE randomNums[8]	 //�����
									 )
{
	PC_CMD_DATA pcCmdData;
	PC_CMD_REPLY pcCmdReply;
	initCmdData(pcCmdData,PC_CMD_TYPE_REQUIRE_RANDOM);
	pcCmdData.uParam.REQUIRE_RANDOM_PARAM.randomLenth = randomLenth;

	int Info = send_cmd(&pcCmdData);

	if (0 == dispatch_cmd(&pcCmdReply, PC_CMD_TYPE_REQUIRE_RANDOM, 3*TIME_OUT))
	{
		PRINT("DispatchCMD �����������ʱ\n");
		return IF_TYPE_REQUIRE_RANDOM | REQUIRE_RANDOM_STATE_TIMEOUT;
	}
	switch(pcCmdReply.uReplyParam.requireRandomParam.requireRandomState)
	{
	case REQUIRE_RANDOM_STATE_SUCCESS:
		PRINT("����������ɹ�\n");
		memcpy(randomNums,pcCmdReply.uReplyParam.requireRandomParam.randomNums,8);
		return IF_TYPE_REQUIRE_RANDOM | REQUIRE_RANDOM_STATE_SUCCESS;
	case REQUIRE_RANDOM_STATE_FAILED:
		PRINT("���������ʧ��\n");
		return IF_TYPE_REQUIRE_RANDOM | REQUIRE_RANDOM_STATE_FAILED;
	default:
		PRINT("���������δ֪����\n");
		return IF_TYPE_REQUIRE_RANDOM | pcCmdReply.uReplyParam.requireRandomParam.requireRandomState;
	}
}

//���ݹ㲥
RFIDLIBDLL RF_STATUS rfDataBoardcast( IN  DWORD readyToken,				//��������
									  IN  BYTE  boardcastData[100]		//�㲥����
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
		PRINT("DispatchCMD ���ݹ㲥��ʱ\n");
		return IF_TYPE_DATA_BOARDCAST | DATA_BOARDCAST_STATE_TIMEOUT;
	}
	switch(pcCmdReply.uReplyParam.dataBoardcastParam.dataBoardcastState)
	{
	case DATA_BOARDCAST_STATE_SUCCESS:
		PRINT("���ݹ㲥�ɹ�\n");
		return IF_TYPE_DATA_BOARDCAST | DATA_BOARDCAST_STATE_SUCCESS;
	default:
		PRINT("���ݹ㲥δ֪����");
		return IF_TYPE_DATA_BOARDCAST | pcCmdReply.uReplyParam.dataBoardcastParam.dataBoardcastState;
	}
}

/*
���ļ����ڣ����ȡ�ļ�����
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
		PRINT("��дָ��ID�ļ�����\n");
		return IF_TYPE_SEARCH_FILE | SEARCH_FILE_STATE_TIMEOUT;
	}
	datalen = sizeof(pcCmdReply.uReplyParam.searchFileParam.readBuffer);
	switch(pcCmdReply.uReplyParam.searchFileParam.RepState)
	{
	case SEARCH_FILE_STATE_SUCESS:
		PRINT("��дָ��ID�ļ��ɹ�");
		memcpy(readBuffer, pcCmdReply.uReplyParam.searchFileParam.readBuffer, datalen);
		return IF_TYPE_SEARCH_FILE | SEARCH_FILE_STATE_SUCESS;
	case SEARCH_FILE_STATE_FAILD:
		PRINT("��дָ��ID�ļ�ʧ��");
		return IF_TYPE_SEARCH_FILE | SEARCH_FILE_STATE_FAILD;
	default:
		return IF_TYPE_SEARCH_FILE | SEARCH_FILE_STATE_TIMEOUT;
	}
}
