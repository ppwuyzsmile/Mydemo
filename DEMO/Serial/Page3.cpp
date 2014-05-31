// Page3.cpp : 实现文件
//

#include "stdafx.h"
#include "Serial.h"
#include "Page3.h"
#include "Page1.h"
#include "afxdialogex.h"
#include "pcCommand.h"
#include "SerialDlg.h"
#include "..\..\API二期\APIDLL\apidll.h"


// CPage3 对话框

IMPLEMENT_DYNAMIC(CPage3, CDialogEx)

CPage3::CPage3(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPage3::IDD, pParent)
{

	m_Status3 = _T("");
	m_Rid3 = _T("303030");
	m_Tid3 = _T("a1a2a3a4a5a6a7a8");
	m_FindCondition = _T("00");
	m_BoaderCastData = _T("00");
	m_TimeWidth = _T("0008");
	m_SendTimeNum = _T("0008");
	m_SendRecvIndicate = _T("00");
	m_SensorID = _T("00");
	m_SensorState = _T("00");
	m_DeactiveCode = _T("00000000");
	m_AdminPWD = _T("00000000");
	m_PWDIndex = _T("0000");
	m_Mode = _T("00");
	m_NewPWD = _T("00000000");
	m_ReadyToken = _T("00000000");
	m_RandomLen = _T("00");
	m_FileID = _T("a105");
	m_Offset = _T("00");
	m_DataLength = _T("08");
	m_TimeNum = _T("0000");
	m_UpCycle = _T("0008");
	m_MonitorCycle = _T("0008");
}

CPage3::~CPage3()
{
}

void CPage3::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATUS3, m_Status3);
	DDX_Text(pDX, IDC_EDIT1, m_Rid3);
	DDX_Text(pDX, IDC_EDIT2, m_Tid3);
	DDX_Text(pDX, IDC_EDIT3, m_FindCondition);
	DDX_Text(pDX, IDC_EDIT25, m_BoaderCastData);
	DDX_Control(pDX, IDC_BUTTON1, m_Page3Button1);
	DDX_Control(pDX, IDC_BUTTON2, m_Page3Button2);
	DDX_Control(pDX, IDC_BUTTON3, m_Page3Button3);
	DDX_Control(pDX, IDC_BUTTON4, m_Page3Button4);
	DDX_Control(pDX, IDC_BUTTON5, m_Page3Button5);
	DDX_Control(pDX, IDC_BUTTON6, m_Page3Button6);
	DDX_Control(pDX, IDC_BUTTON7, m_Page3Button7);
	DDX_Control(pDX, IDC_BUTTON8, m_Page3Button8);
	DDX_Control(pDX, IDC_BUTTON9, m_Page3Button9);
	DDX_Text(pDX, IDC_EDIT10, m_TimeWidth);
	DDX_Text(pDX, IDC_EDIT11, m_SendTimeNum);
	DDX_Text(pDX, IDC_EDIT12, m_SendRecvIndicate);
	DDX_Text(pDX, IDC_EDIT13, m_SensorID);
	DDX_Text(pDX, IDC_EDIT14, m_SensorState);
	DDX_Text(pDX, IDC_EDIT15, m_DeactiveCode);
	DDX_Text(pDX, IDC_EDIT18, m_AdminPWD);
	DDX_Text(pDX, IDC_EDIT19, m_PWDIndex);
	DDX_Text(pDX, IDC_EDIT21, m_Mode);
	DDX_Text(pDX, IDC_EDIT22, m_NewPWD);
	DDX_Text(pDX, IDC_EDIT24, m_ReadyToken);
	DDX_Text(pDX, IDC_EDIT28, m_RandomLen);
	DDX_Text(pDX, IDC_EDIT4, m_FileID);
	DDX_Text(pDX, IDC_EDIT5, m_Offset);
	DDX_Text(pDX, IDC_EDIT6, m_DataLength);
	DDX_Text(pDX, IDC_EDIT7, m_TimeNum);
	DDX_Text(pDX, IDC_EDIT8, m_UpCycle);
	DDX_Text(pDX, IDC_EDIT9, m_MonitorCycle);
}


BEGIN_MESSAGE_MAP(CPage3, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CPage3::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CPage3::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CPage3::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CPage3::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CPage3::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CPage3::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CPage3::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CPage3::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CPage3::OnBnClickedButton9)
END_MESSAGE_MAP()


// CPage3 消息处理程序
void CPage3::InitSingleCmd3(PC_CMD_DATA& pcCmdData)
{
	memset(&pcCmdData,0,sizeof(PC_CMD_DATA));
	pcCmdData.sigEnd = SIG_END_NUM;
	pcCmdData.sigStart = SIG_START_NUM;
	pcCmdData.cmdType = PC_CMD_TYPE_TEST_SINGLE_CMD;
}


void CPage3::InitParameter3()
{
	UpdateData(TRUE);

	CSerialDlg newDlg;
	CPage1 newPage;
	int tmpLen = 0;

	tmpLen = newDlg.StrToHex(m_Rid3.GetBuffer(), Rid3);
	tmpLen = newDlg.StrToHex(m_Tid3.GetBuffer(), Tid3);
	FileID = newPage.StringToHex(m_FileID);
	TimeNum = newPage.StringToHex(m_TimeNum);
	UpCycle = newPage.StringToHex(m_UpCycle);
	MonitorCycle = newPage.StringToHex(m_MonitorCycle);
	TimeWidth = newPage.StringToHex(m_TimeWidth);
	SendTimeNum = newPage.StringToHex(m_SendTimeNum);
	PWDIndex = newPage.StringToHex(m_PWDIndex);
	Offset = newPage.StringToHex(m_Offset);
	DataLength = newPage.StringToHex(m_DataLength);
	SendRecvIndicate = newPage.StringToHex(m_SendRecvIndicate);
	SensorID = newPage.StringToHex(m_SensorID);
	SensorState = newPage.StringToHex(m_SensorState);
	Mode = newPage.StringToHex(m_Mode);
	RandomLen = newPage.StringToHex(m_RandomLen);
	DeactiveCode = newPage.StringToHex(m_DeactiveCode);
	AdminPWD = newPage.StringToHex(m_AdminPWD);
	NewPWD = newPage.StringToHex(m_NewPWD);
	ReadyToken = newPage.StringToHex(m_ReadyToken);
}


void CPage3::OnBnClickedButton1() //检索记录
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_Status3.Empty();
	UpdateData(FALSE);

	PC_CMD_DATA pcCmdData;
	InitSingleCmd3(pcCmdData);
	InitParameter3();

	CSerialDlg newDlg;
	int tmpLen = newDlg.StrToHex(m_FindCondition.GetBuffer(), FindCondition); 

	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_3AH.RID, Rid3, 3);
	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_3AH.TID, Tid3, 8);
	pcCmdData.uParam.TEST_SINGLE.singleLength = 13 + tmpLen;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_3AH.cmdCode = 0x3A;
	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_3AH.Data, FindCondition, tmpLen);
	pcCmdData.uParam.TEST_SINGLE.frameOption = 0x10;

	if(TestSendCmd(&pcCmdData) != 0)
	{
		m_Status3 = "测试检索记录命令失败";
		UpdateData(FALSE);
		return;
	}

	CPage1 newPage;
	PC_CMD_REPLY pcCmdReply;
	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	int Ret = newPage.SingleCmdRecvFun(&pcCmdReply, PC_CMD_TYPE_TEST_SINGLE_CMD, 2*TIME_OUT);
	if(Ret == 1)
	{
		if(pcCmdReply.uReplyParam.testSingleCmd.testRadioState == TEST_RADIO_GET_REPLY)
		{
			CString tmpStr;
			m_Status3 += "测试检索记录命令返回成功：";
			CPage1 newPage;
			tmpStr = newPage.HexPrintf(pcCmdReply.uReplyParam.testSingleCmd.Data);
			m_Status3 += tmpStr;
		}
		else
			m_Status3 = "测试检索记录命令返回超时";
	}
	else
		m_Status3 = "测试检索记录命令失败";

	UpdateData(FALSE);

// 	if(TestSendCmd(&pcCmdData) == 0)
// 		m_Status3 = "测试检索记录命令成功";
// 	else
// 		m_Status3 = "测试检索记录命令失败";
// 
// 	UpdateData(FALSE);
}


void CPage3::OnBnClickedButton2() //检测周期配置
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_Status3.Empty();
	UpdateData(FALSE);

	PC_CMD_DATA pcCmdData;
	InitSingleCmd3(pcCmdData);
	InitParameter3();

	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_51H.RID, Rid3, 3);
	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_51H.TID, Tid3, 8);
	pcCmdData.uParam.TEST_SINGLE.singleLength = 28;
	pcCmdData.uParam.TEST_SINGLE.frameOption = 0x10;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_51H.cmdCode = 0x51;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_51H.FileId = Swap16(FileID);
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_51H.FileOffset = Offset;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_51H.DataLen = DataLength;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_51H.TimeSlotNum = Swap16(TimeNum);
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_51H.ReportedCycle = Swap16(UpCycle);
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_51H.MonitorCycle = Swap16(MonitorCycle);
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_51H.TimeSlotWidth = Swap16(TimeWidth);
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_51H.TimeSlotSum = Swap16(SendTimeNum);
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_51H.ReportIndication = SendRecvIndicate;

	if(TestSendCmd(&pcCmdData) != 0)
	{
		m_Status3 = "测试检测周期配置命令失败";
		UpdateData(FALSE);
		return;
	}

	CPage1 newPage;
	PC_CMD_REPLY pcCmdReply;
	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	int Ret = newPage.SingleCmdRecvFun(&pcCmdReply, PC_CMD_TYPE_TEST_SINGLE_CMD, 2*TIME_OUT);
	if(Ret == 1)
	{
		if(pcCmdReply.uReplyParam.testSingleCmd.testRadioState == TEST_RADIO_GET_REPLY)
		{
			CString tmpStr;
			m_Status3 += "测试检测周期配置命令返回成功：";
			CPage1 newPage;
			tmpStr = newPage.HexPrintf(pcCmdReply.uReplyParam.testSingleCmd.Data);
			m_Status3 += tmpStr;
		}
		else
			m_Status3 = "测试检测周期配置命令返回超时";
	}
	else
		m_Status3 = "测试检测周期配置命令失败";

	UpdateData(FALSE);

// 	if(TestSendCmd(&pcCmdData) == 0)
// 		m_Status3 = "测试检测周期配置命令成功";
// 	else
// 		m_Status3 = "测试检测周期配置命令失败";
// 
// 	UpdateData(FALSE);
}


void CPage3::OnBnClickedButton3() //检测周期起始
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_Status3.Empty();
	UpdateData(FALSE);

	PC_CMD_DATA pcCmdData;
	InitSingleCmd3(pcCmdData);
	InitParameter3();

	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_52H.RID, Rid3, 3);
	pcCmdData.uParam.TEST_SINGLE.singleLength = 5;
	pcCmdData.uParam.TEST_SINGLE.frameOption = 0x00;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_52H.cmdCode = 0x52;

	if(TestSendCmd(&pcCmdData) != 0)
	{
		m_Status3 = "测试检测周期起始命令失败";
		UpdateData(FALSE);
		return;
	}

	CPage1 newPage;
	PC_CMD_REPLY pcCmdReply;
	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	int Ret = newPage.SingleCmdRecvFun(&pcCmdReply, PC_CMD_TYPE_TEST_SINGLE_CMD, 2*TIME_OUT);
	if(Ret == 1)
	{
		if(pcCmdReply.uReplyParam.testSingleCmd.testRadioState == TEST_RADIO_GET_REPLY)
		{
			CString tmpStr;
			m_Status3 += "测试检测周期起始命令返回成功：";
			CPage1 newPage;
			tmpStr = newPage.HexPrintf(pcCmdReply.uReplyParam.testSingleCmd.Data);
			m_Status3 += tmpStr;
		}
		else
			m_Status3 = "测试检测周期起始命令返回超时";
	}
	else
		m_Status3 = "测试检测周期起始命令失败";

	UpdateData(FALSE);

// 	if(TestSendCmd(&pcCmdData) == 0)
// 		m_Status3 = "测试检测周期起始命令成功";
// 	else
// 		m_Status3 = "测试检测周期起始命令失败";
// 
// 	UpdateData(FALSE);
}


void CPage3::OnBnClickedButton4() //传感器使能
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_Status3.Empty();
	UpdateData(FALSE);

	PC_CMD_DATA pcCmdData;
	InitSingleCmd3(pcCmdData);
	InitParameter3();

	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_53H.RID, Rid3, 3);
	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_53H.TID, Tid3, 8);
	pcCmdData.uParam.TEST_SINGLE.singleLength = 17;
	pcCmdData.uParam.TEST_SINGLE.frameOption = 0x10;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_53H.cmdCode = 0x53;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_53H.FileId = Swap16(FileID);
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_53H.SensorId = SensorID;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_53H.SensorState = SensorState;

	if(TestSendCmd(&pcCmdData) != 0)
	{
		m_Status3 = "测试传感器使能命令失败";
		UpdateData(FALSE);
		return;
	}

	CPage1 newPage;
	PC_CMD_REPLY pcCmdReply;
	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	int Ret = newPage.SingleCmdRecvFun(&pcCmdReply, PC_CMD_TYPE_TEST_SINGLE_CMD, 2*TIME_OUT);
	if(Ret == 1)
	{
		if(pcCmdReply.uReplyParam.testSingleCmd.testRadioState == TEST_RADIO_GET_REPLY)
		{
			CString tmpStr;
			m_Status3 += "测试传感器使能命令返回成功：";
			CPage1 newPage;
			tmpStr = newPage.HexPrintf(pcCmdReply.uReplyParam.testSingleCmd.Data);
			m_Status3 += tmpStr;
		}
		else
			m_Status3 = "测试传感器使能命令返回超时";
	}
	else
		m_Status3 = "测试传感器使能命令失败";

	UpdateData(FALSE);

// 	if(TestSendCmd(&pcCmdData) == 0)
// 		m_Status3 = "测试传感器使能命令成功";
// 	else
// 		m_Status3 = "测试传感器使能命令失败";
// 
// 	UpdateData(FALSE);
}


void CPage3::OnBnClickedButton5() //获取标签安全参数
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_Status3.Empty();
	UpdateData(FALSE);

	PC_CMD_DATA pcCmdData;
	InitSingleCmd3(pcCmdData);
	InitParameter3();

	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_71H.RID, Rid3, 3);
	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_71H.TID, Tid3, 8);
	pcCmdData.uParam.TEST_SINGLE.singleLength = 13;
	pcCmdData.uParam.TEST_SINGLE.frameOption = 0x10;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_71H.cmdCode = 0x71;

	if(TestSendCmd(&pcCmdData) != 0)
	{
		m_Status3 = "测试获取标签安全参数命令失败";
		UpdateData(FALSE);
		return;
	}

	CPage1 newPage;
	PC_CMD_REPLY pcCmdReply;
	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	int Ret = newPage.SingleCmdRecvFun(&pcCmdReply, PC_CMD_TYPE_TEST_SINGLE_CMD, 2*TIME_OUT);
	if(Ret == 1)
	{
		if(pcCmdReply.uReplyParam.testSingleCmd.testRadioState == TEST_RADIO_GET_REPLY)
		{
			CString tmpStr;
			m_Status3 += "测试获取标签安全参数命令返回成功：";
			CPage1 newPage;
			tmpStr = newPage.HexPrintf(pcCmdReply.uReplyParam.testSingleCmd.Data);
			m_Status3 += tmpStr;
		}
		else
			m_Status3 = "测试获取标签安全参数命令返回超时";
	}
	else
		m_Status3 = "测试获取标签安全参数命令失败";

	UpdateData(FALSE);

// 	if(TestSendCmd(&pcCmdData) == 0)
// 		m_Status3 = "测试获取标签安全参数命令成功";
// 	else
// 		m_Status3 = "测试获取标签安全参数命令失败";
// 
// 	UpdateData(FALSE);
}


void CPage3::OnBnClickedButton6() //灭活
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_Status3.Empty();
	UpdateData(FALSE);

	PC_CMD_DATA pcCmdData;
	InitSingleCmd3(pcCmdData);
	InitParameter3();

	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_91H.RID, Rid3, 3);
	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_91H.TID, Tid3, 8);
	pcCmdData.uParam.TEST_SINGLE.singleLength = 17;
	pcCmdData.uParam.TEST_SINGLE.frameOption = 0x10;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_91H.cmdCode = 0x91;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_91H.deactiveCode = Swap32(DeactiveCode);

	if(TestSendCmd(&pcCmdData) != 0)
	{
		m_Status3 = "测试灭活命令失败";
		UpdateData(FALSE);
		return;
	}

	CPage1 newPage;
	PC_CMD_REPLY pcCmdReply;
	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	int Ret = newPage.SingleCmdRecvFun(&pcCmdReply, PC_CMD_TYPE_TEST_SINGLE_CMD, 2*TIME_OUT);
	if(Ret == 1)
	{
		if(pcCmdReply.uReplyParam.testSingleCmd.testRadioState == TEST_RADIO_GET_REPLY)
		{
			CString tmpStr;
			m_Status3 += "测试灭活命令返回成功：";
			CPage1 newPage;
			tmpStr = newPage.HexPrintf(pcCmdReply.uReplyParam.testSingleCmd.Data);
			m_Status3 += tmpStr;
		}
		else
			m_Status3 = "测试灭活命令返回超时";
	}
	else
		m_Status3 = "测试灭活命令失败";

	UpdateData(FALSE);

// 	if(TestSendCmd(&pcCmdData) == 0)
// 		m_Status3 = "测试灭活命令成功";
// 	else
// 		m_Status3 = "测试灭活命令失败";
// 
// 	UpdateData(FALSE);
}


void CPage3::OnBnClickedButton7() //更新系统口令
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_Status3.Empty();
	UpdateData(FALSE);

	PC_CMD_DATA pcCmdData;
	InitSingleCmd3(pcCmdData);
	InitParameter3();

	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_93H.RID, Rid3, 3);
	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_93H.TID, Tid3, 8);
	pcCmdData.uParam.TEST_SINGLE.singleLength = 24;
	pcCmdData.uParam.TEST_SINGLE.frameOption = 0x10;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_93H.cmdCode = 0x93;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_93H.adminPWD = Swap32(AdminPWD);
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_93H.index = Swap16(PWDIndex);
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_93H.mode = Mode;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_93H.newPWD = Swap32(NewPWD);

	if(TestSendCmd(&pcCmdData) != 0)
	{
		m_Status3 = "测试更新系统口令命令失败";
		UpdateData(FALSE);
		return;
	}

	CPage1 newPage;
	PC_CMD_REPLY pcCmdReply;
	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	int Ret = newPage.SingleCmdRecvFun(&pcCmdReply, PC_CMD_TYPE_TEST_SINGLE_CMD, 2*TIME_OUT);
	if(Ret == 1)
	{
		if(pcCmdReply.uReplyParam.testSingleCmd.testRadioState == TEST_RADIO_GET_REPLY)
		{
			CString tmpStr;
			m_Status3 += "测试更新系统口令命令返回成功：";
			CPage1 newPage;
			tmpStr = newPage.HexPrintf(pcCmdReply.uReplyParam.testSingleCmd.Data);
			m_Status3 += tmpStr;
		}
		else
			m_Status3 = "测试更新系统口令命令返回超时";
	}
	else
		m_Status3 = "测试更新系统口令命令失败";

	UpdateData(FALSE);

// 	if(TestSendCmd(&pcCmdData) == 0)
// 		m_Status3 = "测试更新系统口令命令成功";
// 	else
// 		m_Status3 = "测试更新系统口令命令失败";
// 
// 	UpdateData(FALSE);
}


void CPage3::OnBnClickedButton8() //数据广播
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_Status3.Empty();
	UpdateData(FALSE);

	PC_CMD_DATA pcCmdData;
	InitSingleCmd3(pcCmdData);
	InitParameter3();

	CSerialDlg newDlg;
	int tmpLen = newDlg.StrToHex(m_BoaderCastData.GetBuffer(), BoaderCastData); 

	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_94H.RID, Rid3, 3);
	pcCmdData.uParam.TEST_SINGLE.singleLength = 9 + tmpLen;
	pcCmdData.uParam.TEST_SINGLE.frameOption = 0x00;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_94H.cmdCode = 0x94;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_94H.readyToken = Swap32(ReadyToken);
	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_94H.Data, BoaderCastData, tmpLen);

	if(TestSendCmd(&pcCmdData) != 0)
	{
		m_Status3 = "测试数据广播命令失败";
		UpdateData(FALSE);
		return;
	}

	CPage1 newPage;
	PC_CMD_REPLY pcCmdReply;
	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	int Ret = newPage.SingleCmdRecvFun(&pcCmdReply, PC_CMD_TYPE_TEST_SINGLE_CMD, 2*TIME_OUT);
	if(Ret == 1)
	{
		if(pcCmdReply.uReplyParam.testSingleCmd.testRadioState == TEST_RADIO_GET_REPLY)
		{
			CString tmpStr;
			m_Status3 += "测试数据广播命令返回成功：";
			CPage1 newPage;
			tmpStr = newPage.HexPrintf(pcCmdReply.uReplyParam.testSingleCmd.Data);
			m_Status3 += tmpStr;
		}
		else
			m_Status3 = "测试数据广播命令返回超时";
	}
	else
		m_Status3 = "测试数据广播命令失败";

	UpdateData(FALSE);

// 	if(TestSendCmd(&pcCmdData) == 0)
// 		m_Status3 = "测试数据广播命令成功";
// 	else
// 		m_Status3 = "测试数据广播命令失败";
// 
// 	UpdateData(FALSE);
}


void CPage3::OnBnClickedButton9() //请求随机数
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_Status3.Empty();
	UpdateData(FALSE);

	PC_CMD_DATA pcCmdData;
	InitSingleCmd3(pcCmdData);
	InitParameter3();

	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_92H.RID, Rid3, 3);
	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_92H.TID, Tid3, 8);
	pcCmdData.uParam.TEST_SINGLE.singleLength = 14;
	pcCmdData.uParam.TEST_SINGLE.frameOption = 0x10;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_92H.cmdCode = 0x92;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_92H.RanLen = RandomLen;

	if(TestSendCmd(&pcCmdData) != 0)
	{
		m_Status3 = "测试请求随机数命令失败";
		UpdateData(FALSE);
		return;
	}

	CPage1 newPage;
	PC_CMD_REPLY pcCmdReply;
	memset(&pcCmdReply, 0, sizeof(PC_CMD_REPLY));
	int Ret = newPage.SingleCmdRecvFun(&pcCmdReply, PC_CMD_TYPE_TEST_SINGLE_CMD, 2*TIME_OUT);
	if(Ret == 1)
	{
		if(pcCmdReply.uReplyParam.testSingleCmd.testRadioState == TEST_RADIO_GET_REPLY)
		{
			CString tmpStr;
			m_Status3 += "测试请求随机数命令返回成功：";
			CPage1 newPage;
			tmpStr = newPage.HexPrintf(pcCmdReply.uReplyParam.testSingleCmd.Data);
			m_Status3 += tmpStr;
		}
		else
			m_Status3 = "测试请求随机数命令返回超时";
	}
	else
		m_Status3 = "测试请求随机数命令失败";

	UpdateData(FALSE);

// 	if(TestSendCmd(&pcCmdData) == 0)
// 		m_Status3 = "测试请求随机数命令成功";
// 	else
// 		m_Status3 = "测试请求随机数命令失败";
// 
// 	UpdateData(FALSE);
}
