// Page1.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Serial.h"
#include "Page1.h"
#include "afxdialogex.h"
#include "pcCommand.h"
#include "SerialDlg.h"
#include "..\..\API����\APIDLL\apidll.h"


// CPage1 �Ի���

IMPLEMENT_DYNAMIC(CPage1, CDialogEx)

CPage1::CPage1(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPage1::IDD, pParent)
{

	m_Rid1 = _T("303030");
	m_Tid1 = _T("a1a2a3a4a5a6a7a8");
	m_Status = _T("");
	m_DataContent = _T("00");
	m_DataLength = _T("00");
	m_Q = _T("01");
	m_N = _T("08");
	m_SNslot = _T("08");
	m_SNsuccess = _T("0000");
	m_RN8 = _T("08");
	m_TimeWidth = _T("0008");
	m_Indicate = _T("00");
	m_StartTime = _T("0000");
	m_EndTime = _T("0008");
	m_ChannelNum = _T("08");
	m_ReadyToken = _T("00000000");
	m_SleepToken = _T("00000000");
	m_SleepTime = _T("00");
	m_ListenTime = _T("00");
	m_FileNum = _T("a105");
	m_PianYi = _T("00");
}

CPage1::~CPage1()
{
}

void CPage1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_RID1, m_Rid1);
	DDX_Text(pDX, IDC_TID1, m_Tid1);
	DDX_Text(pDX, IDC_STATUS, m_Status);
	DDX_Text(pDX, IDC_EDIT11, m_DataContent);
	DDX_Control(pDX, IDC_BUTTON1, m_TestButton1);
	DDX_Control(pDX, IDC_BUTTON2, m_TestButton2);
	DDX_Control(pDX, IDC_BUTTON3, m_TestButton3);
	DDX_Control(pDX, IDC_BUTTON6, m_TestButton6);
	DDX_Control(pDX, IDC_BUTTON7, m_TestButton7);
	DDX_Control(pDX, IDC_BUTTON8, m_TestButton8);
	DDX_Control(pDX, IDC_BUTTON9, m_TestButton9);
	DDX_Control(pDX, IDC_BUTTON10, m_TestButton10);
	DDX_Control(pDX, IDC_BUTTON12, m_TestButton12);
	DDX_Control(pDX, IDC_BUTTON13, m_TestButton13);
	DDX_Text(pDX, IDC_EDIT10, m_DataLength);
	DDX_Text(pDX, IDC_EDIT12, m_Q);
	DDX_Text(pDX, IDC_EDIT13, m_N);
	DDX_Text(pDX, IDC_EDIT14, m_SNslot);
	DDX_Text(pDX, IDC_EDIT15, m_SNsuccess);
	DDX_Text(pDX, IDC_EDIT18, m_RN8);
	DDX_Text(pDX, IDC_EDIT19, m_TimeWidth);
	DDX_Text(pDX, IDC_EDIT21, m_Indicate);
	DDX_Text(pDX, IDC_EDIT24, m_StartTime);
	DDX_Text(pDX, IDC_EDIT25, m_EndTime);
	DDX_Text(pDX, IDC_EDIT3, m_ChannelNum);
	DDX_Text(pDX, IDC_EDIT4, m_ReadyToken);
	DDX_Text(pDX, IDC_EDIT5, m_SleepToken);
	DDX_Text(pDX, IDC_EDIT6, m_SleepTime);
	DDX_Text(pDX, IDC_EDIT7, m_ListenTime);
	DDX_Text(pDX, IDC_EDIT8, m_FileNum);
	DDX_Text(pDX, IDC_EDIT9, m_PianYi);
}


BEGIN_MESSAGE_MAP(CPage1, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CPage1::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CPage1::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CPage1::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON6, &CPage1::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CPage1::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CPage1::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CPage1::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON10, &CPage1::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON12, &CPage1::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON13, &CPage1::OnBnClickedButton13)
END_MESSAGE_MAP()


// CPage1 ��Ϣ�������


void CPage1::InitSingleCmd1(PC_CMD_DATA& pcCmdData)
{
	memset(&pcCmdData,0,sizeof(PC_CMD_DATA));
	pcCmdData.cmdType = PC_CMD_TYPE_TEST_SINGLE_CMD;
	pcCmdData.sigEnd = SIG_END_NUM;
	pcCmdData.sigStart = SIG_START_NUM;
}


int CPage1::SingleCmdRecvFun(PC_CMD_REPLY *pcCmdReply, PC_CMD_TYPE pcCmdType, unsigned int TimeOut)
{
	int Ret = TestRecvCmd(pcCmdReply, pcCmdType, TimeOut);
	if(Ret != 1)
		return 0;
	else
		return 1;//����1��ʾ�ɹ���0��ʾʧ��
}


void CPage1::InitParameter1()
{
	UpdateData(TRUE);

	CSerialDlg newDlg; //��ʼ����������
	int tmpLen = 0;
	
	//ʹ��CSerialDlg�����Ա������ʮ������ת��
	tmpLen = newDlg.StrToHex(m_Rid1.GetBuffer(), Rid1);
	tmpLen = newDlg.StrToHex(m_Tid1.GetBuffer(), Tid1);

	//ʹ��strtol������ʮ������ת��
	ReadyToken = StringToHex(m_ReadyToken);
	SleepToken = StringToHex(m_SleepToken);
	FileNum = StringToHex(m_FileNum);
	SNsuccess = StringToHex(m_SNsuccess);
	TimeWidth = StringToHex(m_TimeWidth);
	StartTime = StringToHex(m_StartTime);
	EndTime = StringToHex(m_EndTime);
	ChannelNum = StringToHex(m_ChannelNum);
	SleepTime = StringToHex(m_SleepTime);
	ListenTime = StringToHex(m_ListenTime);
	PianYi = StringToHex(m_PianYi);
	DataLength = StringToHex(m_DataLength);
	Q = StringToHex(m_Q);
	N = StringToHex(m_N);
	SNslot = StringToHex(m_SNslot);
	RN8 = StringToHex(m_RN8);
	Indicate = StringToHex(m_Indicate);
}


int CPage1::StringToHex(CString strTmp)
{
	int retNum = 0;
	char *strSwitch = (LPSTR)(LPCTSTR)(strTmp);
	retNum = strtol(strSwitch, NULL, 16); //ʮ������ת������

	return retNum;
}


CString CPage1::HexPrintf(unsigned char *DataBuffer)
{
	CString strTmp;
	for(int i = 1; i < DataBuffer[0]; i++)
	{
		CString Tmp;
		Tmp.Format("%x", DataBuffer[i]);
		strTmp += Tmp;
	}
	return strTmp;
}


void CPage1::OnBnClickedButton1() //����
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	m_Status.Empty();
	UpdateData(FALSE);

	PC_CMD_DATA pcCmdData;
	InitSingleCmd1(pcCmdData);
	InitParameter1();

	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_01H.RID, Rid1, 3);
	pcCmdData.uParam.TEST_SINGLE.frameOption = 0x00;
	pcCmdData.uParam.TEST_SINGLE.singleLength = 10;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_01H.cmdCode = 0x01;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_01H.channelNum = ChannelNum;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_01H.readyToken = Swap32(ReadyToken);

	if(TestSendCmd(&pcCmdData) != 0)
	{
		m_Status = "���Ծ�������ʧ��";
		UpdateData(FALSE);
		return;
	}
		
	PC_CMD_REPLY pcCmdReply;
	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	int Ret = SingleCmdRecvFun(&pcCmdReply, PC_CMD_TYPE_TEST_SINGLE_CMD, 2*TIME_OUT);
	if(Ret == 1)
	{
		if(pcCmdReply.uReplyParam.testSingleCmd.testRadioState == TEST_RADIO_GET_REPLY)
		{
			CString tmpStr;
			m_Status += "���Ծ�������سɹ���";
			tmpStr = HexPrintf(pcCmdReply.uReplyParam.testSingleCmd.Data);
			m_Status += tmpStr;
		}
		else
			m_Status = "���Ծ�������س�ʱ";
	}
	else
		m_Status = "���Ծ�������ʧ��";

	UpdateData(FALSE);
}


void CPage1::OnBnClickedButton2() //�������б�ǩ����
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	m_Status.Empty();
	UpdateData(FALSE);

	PC_CMD_DATA pcCmdData;
	InitSingleCmd1(pcCmdData);
	InitParameter1();

	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_02H.RID, Rid1, 3);
	pcCmdData.uParam.TEST_SINGLE.singleLength = 11;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_02H.cmdCode = 0x02;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_02H.SleepToken = Swap32(SleepToken);
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_02H.SleepTime = SleepTime;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_02H.ListenTime = ListenTime;
	pcCmdData.uParam.TEST_SINGLE.frameOption = 0x00;

	if(TestSendCmd(&pcCmdData) != 0)
	{
		m_Status = "�����������б�ǩ����ʧ��";
		UpdateData(FALSE);
		return;
	}

	PC_CMD_REPLY pcCmdReply;
	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	int Ret = SingleCmdRecvFun(&pcCmdReply, PC_CMD_TYPE_TEST_SINGLE_CMD, 2*TIME_OUT);
	if(Ret == 1)
	{
		if(pcCmdReply.uReplyParam.testSingleCmd.testRadioState == TEST_RADIO_GET_REPLY)
		{
			CString tmpStr;
			m_Status += "�����������б�ǩ����سɹ���";
			tmpStr = HexPrintf(pcCmdReply.uReplyParam.testSingleCmd.Data);
			m_Status += tmpStr;
		}
		else
			m_Status = "�����������б�ǩ����س�ʱ";
	}
	else
		m_Status = "�����������б�ǩ����ʧ��";

	UpdateData(FALSE);

// 	if(TestSendCmd(&pcCmdData) == 0)
// 		m_Status = "�����������б�ǩ����ɹ�";
// 	else
// 		m_Status = "�����������б�ǩ����ʧ��";
// 
// 	UpdateData(FALSE);
}


void CPage1::OnBnClickedButton3() //���߳�ĳһ���ǩ������б�ǩ
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	m_Status.Empty();
	UpdateData(FALSE);

	PC_CMD_DATA pcCmdData;
	InitSingleCmd1(pcCmdData);
	InitParameter1();

	CSerialDlg newDlg; //��ʼ����������
	int tmpLen = newDlg.StrToHex(m_DataContent.GetBuffer(), DataContent);

	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_03H.RID, Rid1, 3);
	pcCmdData.uParam.TEST_SINGLE.singleLength = 15 + tmpLen;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_03H.cmdCode = 0x03;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_03H.SleepToken = Swap32(SleepToken);
// 	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_03H.SleepTime = SleepTime;
// 	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_03H.ListenTime = ListenTime;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_03H.DateLangth = DataLength;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_03H.Offset = PianYi;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_03H.FileID = Swap16(FileNum);
	memcpy(&pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_03H.Data, DataContent, tmpLen);
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_03H.Data[tmpLen] = SleepTime;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_03H.Data[tmpLen + 1] = ListenTime;
	pcCmdData.uParam.TEST_SINGLE.frameOption = 0x00;

	if(TestSendCmd(&pcCmdData) != 0)
	{
		m_Status = "�������߳�ĳһ���ǩ������б�ǩ����ʧ��";
		UpdateData(FALSE);
		return;
	}

	PC_CMD_REPLY pcCmdReply;
	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	int Ret = SingleCmdRecvFun(&pcCmdReply, PC_CMD_TYPE_TEST_SINGLE_CMD, 2*TIME_OUT);
	if(Ret == 1)
	{
		if(pcCmdReply.uReplyParam.testSingleCmd.testRadioState == TEST_RADIO_GET_REPLY)
		{
			CString tmpStr;
			m_Status += "�������߳�ĳһ���ǩ������б�ǩ����سɹ���";
			tmpStr = HexPrintf(pcCmdReply.uReplyParam.testSingleCmd.Data);
			m_Status += tmpStr;
		}
		else
			m_Status = "�������߳�ĳһ���ǩ������б�ǩ����س�ʱ";
	}
	else
		m_Status = "�������߳�ĳһ���ǩ������б�ǩ����ʧ��";

	UpdateData(FALSE);

// 	if(TestSendCmd(&pcCmdData) == 0)
// 		m_Status = "�������߳�ĳһ���ǩ������б�ǩ����ɹ�";
// 	else
// 		m_Status = "�������߳�ĳһ���ǩ������б�ǩ����ʧ��";
// 
// 	UpdateData(FALSE);
}


void CPage1::OnBnClickedButton6() //����ĳ����ǩ�����б�ǩ
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	m_Status.Empty();
	UpdateData(FALSE);

	PC_CMD_DATA pcCmdData;
	InitSingleCmd1(pcCmdData);
	InitParameter1();

	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_04H.RID, Rid1, 3);
	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_04H.TID, Tid1, 8);
	pcCmdData.uParam.TEST_SINGLE.singleLength = 19;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_04H.cmdCode = 0x04;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_04H.SleepToken = Swap32(SleepToken);
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_04H.SleepTime = SleepTime;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_04H.ListenTime = ListenTime;
	pcCmdData.uParam.TEST_SINGLE.frameOption = 0x10;

	if(TestSendCmd(&pcCmdData) != 0)
	{
		m_Status = "��������ĳ����ǩ�����б�ǩ����ʧ��";
		UpdateData(FALSE);
		return;
	}

	PC_CMD_REPLY pcCmdReply;
	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	int Ret = SingleCmdRecvFun(&pcCmdReply, PC_CMD_TYPE_TEST_SINGLE_CMD, 2*TIME_OUT);
	if(Ret == 1)
	{
		if(pcCmdReply.uReplyParam.testSingleCmd.testRadioState == TEST_RADIO_GET_REPLY)
		{
			CString tmpStr;
			m_Status += "��������ĳ����ǩ�����б�ǩ����سɹ���";
			tmpStr = HexPrintf(pcCmdReply.uReplyParam.testSingleCmd.Data);
			m_Status += tmpStr;
		}
		else
			m_Status = "��������ĳ����ǩ�����б�ǩ����س�ʱ";
	}
	else
		m_Status = "��������ĳ����ǩ�����б�ǩ����ʧ��";

	UpdateData(FALSE);

// 	if(TestSendCmd(&pcCmdData) == 0)
// 		m_Status = "��������ĳ����ǩ�����б�ǩ����ɹ�";
// 	else
// 		m_Status = "��������ĳ����ǩ�����б�ǩ����ʧ��";
// 
// 	UpdateData(FALSE);
}


void CPage1::OnBnClickedButton7() //����֡��ʼ
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	m_Status.Empty();
	UpdateData(FALSE);

	PC_CMD_DATA pcCmdData;
	InitSingleCmd1(pcCmdData);
	InitParameter1();

	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_11H.RID, Rid1, 3);
	pcCmdData.uParam.TEST_SINGLE.singleLength = 7;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_11H.cmdCode = 0x11;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_11H.Q = Q;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_11H.N = N;
	pcCmdData.uParam.TEST_SINGLE.frameOption = 0x00;

	if(TestSendCmd(&pcCmdData) != 0)
	{
		m_Status = "���Խ���֡��ʼ����ʧ��";
		UpdateData(FALSE);
		return;
	}

	PC_CMD_REPLY pcCmdReply;
	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	int Ret = SingleCmdRecvFun(&pcCmdReply, PC_CMD_TYPE_TEST_SINGLE_CMD, 2*TIME_OUT);
	if(Ret == 1)
	{
		if(pcCmdReply.uReplyParam.testSingleCmd.testRadioState == TEST_RADIO_GET_REPLY)
		{
			CString tmpStr;
			m_Status += "���Խ���֡��ʼ����سɹ���";
			tmpStr = HexPrintf(pcCmdReply.uReplyParam.testSingleCmd.Data);
			m_Status += tmpStr;
		}
		else
			m_Status = "���Խ���֡��ʼ����س�ʱ";
	}
	else
		m_Status = "���Խ���֡��ʼ����ʧ��";

	UpdateData(FALSE);

// 	if(TestSendCmd(&pcCmdData) == 0)
// 		m_Status = "���Խ���֡��ʼ����ɹ�";
// 	else
// 		m_Status = "���Խ���֡��ʼ����ʧ��";
// 
// 	UpdateData(FALSE);
}


void CPage1::OnBnClickedButton8() //���Խ���ʱ϶��ʼ
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	m_Status.Empty();
	UpdateData(FALSE);

	PC_CMD_DATA pcCmdData;
	InitSingleCmd1(pcCmdData);
	InitParameter1();

	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_11H.RID, Rid1, 3);
	pcCmdData.uParam.TEST_SINGLE.singleLength = 6;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_12H.cmdCode = 0x12;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_12H.SNslot = SNslot;
	pcCmdData.uParam.TEST_SINGLE.frameOption = 0x00;

	if(TestSendCmd(&pcCmdData) != 0)
	{
		m_Status = "���Խ���ʱ϶��ʼ����ʧ��";
		UpdateData(FALSE);
		return;
	}

	PC_CMD_REPLY pcCmdReply;
	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	int Ret = SingleCmdRecvFun(&pcCmdReply, PC_CMD_TYPE_TEST_SINGLE_CMD, 2*TIME_OUT);
	if(Ret == 1)
	{
		if(pcCmdReply.uReplyParam.testSingleCmd.testRadioState == TEST_RADIO_GET_REPLY)
		{
			CString tmpStr;
			m_Status += "���Խ���ʱ϶��ʼ����سɹ���";
			tmpStr = HexPrintf(pcCmdReply.uReplyParam.testSingleCmd.Data);
			m_Status += tmpStr;
		}
		else
			m_Status = "���Խ���ʱ϶��ʼ����س�ʱ";
	}
	else
		m_Status = "���Խ���ʱ϶��ʼ����ʧ��";

	UpdateData(FALSE);

// 	if(TestSendCmd(&pcCmdData) == 0)
// 		m_Status = "���Խ���ʱ϶��ʼ����ɹ�";
// 	else
// 		m_Status = "���Խ���ʱ϶��ʼ����ʧ��";
// 
// 	UpdateData(FALSE);
}


void CPage1::OnBnClickedButton9() //����ʧ��ȷ��
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	m_Status.Empty();
	UpdateData(FALSE);

	PC_CMD_DATA pcCmdData;
	InitSingleCmd1(pcCmdData);
	InitParameter1();

	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_13H.RID, Rid1, 3);
	pcCmdData.uParam.TEST_SINGLE.singleLength = 5;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_13H.cmdCode = 0x13;
	pcCmdData.uParam.TEST_SINGLE.frameOption = 0x00;

	if(TestSendCmd(&pcCmdData) != 0)
	{
		m_Status = "���Խ���ʧ��ȷ������ʧ��";
		UpdateData(FALSE);
		return;
	}

	PC_CMD_REPLY pcCmdReply;
	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	int Ret = SingleCmdRecvFun(&pcCmdReply, PC_CMD_TYPE_TEST_SINGLE_CMD, 2*TIME_OUT);
	if(Ret == 1)
	{
		if(pcCmdReply.uReplyParam.testSingleCmd.testRadioState == TEST_RADIO_GET_REPLY)
		{
			CString tmpStr;
			m_Status += "���Խ���ʧ��ȷ������سɹ���";
			tmpStr = HexPrintf(pcCmdReply.uReplyParam.testSingleCmd.Data);
			m_Status += tmpStr;
		}
		else
			m_Status = "���Խ���ʧ��ȷ������س�ʱ";
	}
	else
		m_Status = "���Խ���ʧ��ȷ������ʧ��";

	UpdateData(FALSE);

// 	if(TestSendCmd(&pcCmdData) == 0)
// 		m_Status = "���Խ���ʧ��ȷ������ɹ�";
// 	else
// 		m_Status = "���Խ���ʧ��ȷ������ʧ��";
// 
// 	UpdateData(FALSE);
}


void CPage1::OnBnClickedButton10() //����ɹ�ȷ��
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	m_Status.Empty();
	UpdateData(FALSE);

	PC_CMD_DATA pcCmdData;
	InitSingleCmd1(pcCmdData);
	InitParameter1();

	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_14H.RID, Rid1, 3);
	pcCmdData.uParam.TEST_SINGLE.singleLength = 8;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_14H.cmdCode = 0x14;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_14H.RN8 = RN8;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_14H.SNsuccess = Swap16(SNsuccess);
	pcCmdData.uParam.TEST_SINGLE.frameOption = 0x00;

	if(TestSendCmd(&pcCmdData) != 0)
	{
		m_Status = "���Խ���ɹ�ȷ������ʧ��";
		UpdateData(FALSE);
		return;
	}

	PC_CMD_REPLY pcCmdReply;
	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	int Ret = SingleCmdRecvFun(&pcCmdReply, PC_CMD_TYPE_TEST_SINGLE_CMD, 2*TIME_OUT);
	if(Ret == 1)
	{
		if(pcCmdReply.uReplyParam.testSingleCmd.testRadioState == TEST_RADIO_GET_REPLY)
		{
			CString tmpStr;
			m_Status += "���Խ���ɹ�ȷ������سɹ���";
			tmpStr = HexPrintf(pcCmdReply.uReplyParam.testSingleCmd.Data);
			m_Status += tmpStr;
		}
		else
			m_Status = "���Խ���ɹ�ȷ������س�ʱ";
	}
	else
		m_Status = "���Խ���ɹ�ȷ������ʧ��";

	UpdateData(FALSE);

// 	if(TestSendCmd(&pcCmdData) == 0)
// 		m_Status = "���Խ���ɹ�ȷ������ɹ�";
// 	else
// 		m_Status = "���Խ���ɹ�ȷ������ʧ��";
// 
// 	UpdateData(FALSE);
}


void CPage1::OnBnClickedButton12() //���½�������
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	m_Status.Empty();
	UpdateData(FALSE);

	PC_CMD_DATA pcCmdData;
	InitSingleCmd1(pcCmdData);
	InitParameter1();

	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_15H.RID, Rid1, 3);
	pcCmdData.uParam.TEST_SINGLE.singleLength = 5;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_15H.cmdCode = 0x15;
	pcCmdData.uParam.TEST_SINGLE.frameOption = 0x00;

	if(TestSendCmd(&pcCmdData) != 0)
	{
		m_Status = "�������½�������ʧ��";
		UpdateData(FALSE);
		return;
	}

	PC_CMD_REPLY pcCmdReply;
	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	int Ret = SingleCmdRecvFun(&pcCmdReply, PC_CMD_TYPE_TEST_SINGLE_CMD, 2*TIME_OUT);
	if(Ret == 1)
	{
		if(pcCmdReply.uReplyParam.testSingleCmd.testRadioState == TEST_RADIO_GET_REPLY)
		{
			CString tmpStr;
			m_Status += "�������½�������سɹ���";
			tmpStr = HexPrintf(pcCmdReply.uReplyParam.testSingleCmd.Data);
			m_Status += tmpStr;
		}
		else
			m_Status = "�������½������ʱ";
	}
	else
		m_Status = "�������½�������ʧ��";

	UpdateData(FALSE);

// 	if(TestSendCmd(&pcCmdData) == 0)
// 		m_Status = "�������½�������ɹ�";
// 	else
// 		m_Status = "�������½�������ʧ��";
// 
// 	UpdateData(FALSE);
}


void CPage1::OnBnClickedButton13() //�ռ�����ʼ����
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	m_Status.Empty();
	UpdateData(FALSE);

	PC_CMD_DATA pcCmdData;
	InitSingleCmd1(pcCmdData);
	InitParameter1();

	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_21H.RID, Rid1, 3);
	pcCmdData.uParam.TEST_SINGLE.singleLength = 14;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_21H.cmdCode = 0x21;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_21H.ChannelWidth = Swap16(TimeWidth);
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_21H.FileID = Swap16(FileNum);
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_21H.Indicate = Indicate;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_21H.StartChannel = Swap16(StartTime);
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_21H.StopChannel = Swap16(EndTime);
	pcCmdData.uParam.TEST_SINGLE.frameOption = 0x00;

	if(TestSendCmd(&pcCmdData) != 0)
	{
		m_Status = "�����ռ�����ʼ����ʧ��";
		UpdateData(FALSE);
		return;
	}

	PC_CMD_REPLY pcCmdReply;
	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	int Ret = SingleCmdRecvFun(&pcCmdReply, PC_CMD_TYPE_TEST_SINGLE_CMD, 2*TIME_OUT);
	if(Ret == 1)
	{
		if(pcCmdReply.uReplyParam.testSingleCmd.testRadioState == TEST_RADIO_GET_REPLY)
		{
			CString tmpStr;
			m_Status += "�����ռ�����ʼ����سɹ���";
			tmpStr = HexPrintf(pcCmdReply.uReplyParam.testSingleCmd.Data);
			m_Status += tmpStr;
		}
		else
			m_Status = "�����ռ�����ʼ����س�ʱ";
	}
	else
		m_Status = "�����ռ�����ʼ����ʧ��";

	UpdateData(FALSE);

// 	if(TestSendCmd(&pcCmdData) == 0)
// 		m_Status = "�����ռ�����ʼ����ɹ�";
// 	else
// 		m_Status = "�����ռ�����ʼ����ʧ��";
// 
// 	UpdateData(FALSE);
}
