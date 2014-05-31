// Page2.cpp : 实现文件
//

#include "stdafx.h"
#include "Serial.h"
#include "Page2.h"
#include "Page1.h"
#include "afxdialogex.h"
#include "pcCommand.h"
#include "SerialDlg.h"
#include "..\..\API二期\APIDLL\apidll.h"


// CPage2 对话框

IMPLEMENT_DYNAMIC(CPage2, CDialogEx)

CPage2::CPage2(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPage2::IDD, pParent)
{
	m_Status2 = _T("");
	m_Rid2 = _T("303030");
	m_Tid2 = _T("a1a2a3a4a5a6a7a8");
	m_DataContent = _T("00");
	m_TimeSlot = _T("0010");
	m_Offset = _T("0000");
	m_Length = _T("08");
	m_RecordPoint = _T("04");
	m_Pianyi = _T("00");
	m_SendIndicate = _T("00");
	m_FileID = _T("a105");
	m_CMDType = _T("00");
	m_Token = _T("00000000");
	m_FileOffset = _T("00");
	m_Type = _T("00");
}

CPage2::~CPage2()
{
}

void CPage2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATUS2, m_Status2);
	DDX_Text(pDX, IDC_EDIT4, m_Rid2);
	DDX_Text(pDX, IDC_EDIT5, m_Tid2);
	DDX_Text(pDX, IDC_EDIT12, m_DataContent);
	DDX_Control(pDX, IDC_BUTTON1, m_Page2Button1);
	DDX_Control(pDX, IDC_BUTTON2, m_Page2Button2);
	DDX_Control(pDX, IDC_BUTTON3, m_Page2Button3);
	DDX_Control(pDX, IDC_BUTTON4, m_Page2Button4);
	DDX_Control(pDX, IDC_BUTTON5, m_Page2Button5);
	DDX_Control(pDX, IDC_BUTTON6, m_Page2Button6);
	DDX_Control(pDX, IDC_BUTTON7, m_Page2Button7);
	DDX_Control(pDX, IDC_BUTTON8, m_Page2Button8);
	DDX_Control(pDX, IDC_BUTTON9, m_Page2Button9);
	DDX_Control(pDX, IDC_BUTTON10, m_Page2Button10);
	DDX_Control(pDX, IDC_BUTTON12, m_Page2Button12);
	DDX_Text(pDX, IDC_EDIT1, m_TimeSlot);
	DDX_Text(pDX, IDC_EDIT10, m_Offset);
	DDX_Text(pDX, IDC_EDIT11, m_Length);
	DDX_Text(pDX, IDC_EDIT13, m_RecordPoint);
	DDX_Text(pDX, IDC_EDIT14, m_Pianyi);
	DDX_Text(pDX, IDC_EDIT2, m_SendIndicate);
	DDX_Text(pDX, IDC_EDIT3, m_FileID);
	DDX_Text(pDX, IDC_EDIT6, m_CMDType);
	DDX_Text(pDX, IDC_EDIT7, m_Token);
	DDX_Text(pDX, IDC_EDIT8, m_FileOffset);
	DDX_Text(pDX, IDC_EDIT9, m_Type);
}


BEGIN_MESSAGE_MAP(CPage2, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CPage2::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CPage2::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CPage2::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CPage2::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CPage2::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CPage2::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CPage2::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CPage2::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CPage2::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON10, &CPage2::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON12, &CPage2::OnBnClickedButton12)
END_MESSAGE_MAP()


// CPage2 消息处理程序
void CPage2::InitSingleCmd2(PC_CMD_DATA& pcCmdData)
{
	memset(&pcCmdData,0,sizeof(PC_CMD_DATA));
	pcCmdData.sigEnd = SIG_END_NUM;
	pcCmdData.sigStart = SIG_START_NUM;
	pcCmdData.cmdType = PC_CMD_TYPE_TEST_SINGLE_CMD;
}


void CPage2::InitParameter2()
{
	UpdateData(TRUE);

	CSerialDlg newDlg;
	CPage1 newPage;
	int tmpLen = 0;

	tmpLen = newDlg.StrToHex(m_Rid2.GetBuffer(), Rid2);
	tmpLen = newDlg.StrToHex(m_Tid2.GetBuffer(), Tid2);
	Token = newPage.StringToHex(m_Token);
	TimeSlot = newPage.StringToHex(m_TimeSlot);
	FileID = newPage.StringToHex(m_FileID);
	Offset = newPage.StringToHex(m_Offset);
	SendIndicate = newPage.StringToHex(m_SendIndicate);
	CMDType = newPage.StringToHex(m_CMDType);
	FileOffset = newPage.StringToHex(m_FileOffset);
	Type = newPage.StringToHex(m_Type);
	Length = newPage.StringToHex(m_Length);
	RecordPoint = newPage.StringToHex(m_RecordPoint);
	Pianyi = newPage.StringToHex(m_Pianyi);
}


void CPage2::OnBnClickedButton1() //收集失败确认命令
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_Status2.Empty();
	UpdateData(FALSE);

	PC_CMD_DATA pcCmdData;
	InitSingleCmd2(pcCmdData);
	InitParameter2();

	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_22H.RID, Rid2, 3);
	pcCmdData.uParam.TEST_SINGLE.singleLength = 10;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_22H.cmdCode = 0x22;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_22H.Channel = Swap16(TimeSlot);
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_22H.FileID = Swap16(FileID);
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_22H.Indicate = SendIndicate;
	pcCmdData.uParam.TEST_SINGLE.frameOption = 0x00;
	
	if(TestSendCmd(&pcCmdData) != 0)
	{
		m_Status2 = "测试收集失败确认命令失败";
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
			m_Status2 += "测试收集失败确认命令返回成功：";
			CPage1 newPage;
			tmpStr = newPage.HexPrintf(pcCmdReply.uReplyParam.testSingleCmd.Data);
			m_Status2 += tmpStr;
		}
		else
			m_Status2 = "测试收集失败确认命令返回超时";
	}
	else
		m_Status2 = "测试收集失败确认命令失败";

	UpdateData(FALSE);

// 	if(TestSendCmd(&pcCmdData) == 0)
// 		m_Status2 = "测试收集失败确认命令成功";
// 	else
// 		m_Status2 = "测试收集失败确认命令失败";
// 
// 	UpdateData(FALSE);
}


void CPage2::OnBnClickedButton2() //收集成功确认
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_Status2.Empty();
	UpdateData(FALSE);

	PC_CMD_DATA pcCmdData;
	InitSingleCmd2(pcCmdData);
	InitParameter2();

	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_23H.RID, Rid2, 3);
	pcCmdData.uParam.TEST_SINGLE.singleLength = 5;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_22H.cmdCode = 0x23;
	pcCmdData.uParam.TEST_SINGLE.frameOption = 0x00;

	if(TestSendCmd(&pcCmdData) != 0)
	{
		m_Status2 = "测试收集成功确认命令失败";
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
			m_Status2 += "测试收集成功确认命令返回成功：";
			CPage1 newPage;
			tmpStr = newPage.HexPrintf(pcCmdReply.uReplyParam.testSingleCmd.Data);
			m_Status2 += tmpStr;
		}
		else
			m_Status2 = "测试收集成功确认命令返回超时";
	}
	else
		m_Status2 = "测试收集成功确认命令失败";

	UpdateData(FALSE);

// 	if(TestSendCmd(&pcCmdData) == 0)
// 		m_Status2 = "测试收集成功确认命令成功";
// 	else
// 		m_Status2 = "测试收集成功确认命令失败";
// 
// 	UpdateData(FALSE);
}


void CPage2::OnBnClickedButton3() //选择文件
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_Status2.Empty();
	UpdateData(FALSE);

	PC_CMD_DATA pcCmdData;
	InitSingleCmd2(pcCmdData);
	InitParameter2();

	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_31H.RID, Rid2, 3);
	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_31H.TID, Tid2, 8);
	pcCmdData.uParam.TEST_SINGLE.singleLength = 15;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_31H.cmdCode = 0x31;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_31H.FileID = Swap16(FileID);
	pcCmdData.uParam.TEST_SINGLE.frameOption = 0x10;

	if(TestSendCmd(&pcCmdData) != 0)
	{
		m_Status2 = "测试选择文件命令失败";
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
			m_Status2 += "测试选择文件命令返回成功：";
			CPage1 newPage;
			tmpStr = newPage.HexPrintf(pcCmdReply.uReplyParam.testSingleCmd.Data);
			m_Status2 += tmpStr;
		}
		else
			m_Status2 = "测试选择文件命令返回超时";
	}
	else
		m_Status2 = "测试选择文件命令失败";

	UpdateData(FALSE);

// 	if(TestSendCmd(&pcCmdData) == 0)
// 		m_Status2 = "测试选择文件命令成功";
// 	else
// 		m_Status2 = "测试选择文件命令失败";
// 
// 	UpdateData(FALSE);
}


void CPage2::OnBnClickedButton4() //验证文件口令
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_Status2.Empty();
	UpdateData(FALSE);

	PC_CMD_DATA pcCmdData;
	InitSingleCmd2(pcCmdData);
	InitParameter2();

	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_32H.RID, Rid2, 3);
	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_32H.TID, Tid2, 8);
	pcCmdData.uParam.TEST_SINGLE.singleLength = 18;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_32H.cmdCode = 0x32;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_32H.Token = Swap32(Token);
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_32H.CMDType = CMDType;
	pcCmdData.uParam.TEST_SINGLE.frameOption = 0x10;

	if(TestSendCmd(&pcCmdData) != 0)
	{
		m_Status2 = "测试验证文件口令命令失败";
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
			m_Status2 += "测试验证文件口令命令返回成功：";
			CPage1 newPage;
			tmpStr = newPage.HexPrintf(pcCmdReply.uReplyParam.testSingleCmd.Data);
			m_Status2 += tmpStr;
		}
		else
			m_Status2 = "测试验证文件口令命令返回超时";
	}
	else
		m_Status2 = "测试验证文件口令命令失败";

	UpdateData(FALSE);

// 	if(TestSendCmd(&pcCmdData) == 0)
// 		m_Status2 = "测试验证文件口令命令成功";
// 	else
// 		m_Status2 = "测试验证文件口令命令失败";
// 
// 	UpdateData(FALSE);
}


void CPage2::OnBnClickedButton5() //文件无效
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_Status2.Empty();
	UpdateData(FALSE);

	PC_CMD_DATA pcCmdData;
	InitSingleCmd2(pcCmdData);
	InitParameter2();

	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_33H.RID, Rid2, 3);
	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_33H.TID, Tid2, 8);
	pcCmdData.uParam.TEST_SINGLE.singleLength = 13;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_33H.cmdCode = 0x33;
	pcCmdData.uParam.TEST_SINGLE.frameOption = 0x10;

	if(TestSendCmd(&pcCmdData) != 0)
	{
		m_Status2 = "测试文件无效命令失败";
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
			m_Status2 += "测试文件无效命令返回成功：";
			CPage1 newPage;
			tmpStr = newPage.HexPrintf(pcCmdReply.uReplyParam.testSingleCmd.Data);
			m_Status2 += tmpStr;
		}
		else
			m_Status2 = "测试文件无效命令返回超时";
	}
	else
		m_Status2 = "测试文件无效命令失败";

	UpdateData(FALSE);

// 	if(TestSendCmd(&pcCmdData) == 0)
// 		m_Status2 = "测试文件无效命令成功";
// 	else
// 		m_Status2 = "测试文件无效命令失败";
// 
// 	UpdateData(FALSE);
}


void CPage2::OnBnClickedButton6() //文件有效
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_Status2.Empty();
	UpdateData(FALSE);

	PC_CMD_DATA pcCmdData;
	InitSingleCmd2(pcCmdData);
	InitParameter2();

	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_34H.RID, Rid2, 3);
	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_34H.TID, Tid2, 8);
	pcCmdData.uParam.TEST_SINGLE.singleLength = 13;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_34H.cmdCode = 0x34;
	pcCmdData.uParam.TEST_SINGLE.frameOption = 0x10;

	if(TestSendCmd(&pcCmdData) != 0)
	{
		m_Status2 = "测试文件有效命令失败";
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
			m_Status2 += "测试文件有效命令返回成功：";
			CPage1 newPage;
			tmpStr = newPage.HexPrintf(pcCmdReply.uReplyParam.testSingleCmd.Data);
			m_Status2 += tmpStr;
		}
		else
			m_Status2 = "测试文件有效命令返回超时";
	}
	else
		m_Status2 = "测试文件有效命令失败";

	UpdateData(FALSE);

// 	if(TestSendCmd(&pcCmdData) == 0)
// 		m_Status2 = "测试文件有效命令成功";
// 	else
// 		m_Status2 = "测试文件有效命令失败";
// 
// 	UpdateData(FALSE);
}


void CPage2::OnBnClickedButton7() //列表文件
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_Status2.Empty();
	UpdateData(FALSE);

	PC_CMD_DATA pcCmdData;
	InitSingleCmd2(pcCmdData);
	InitParameter2();

	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_35H.RID, Rid2, 3);
	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_35H.TID, Tid2, 8);
	pcCmdData.uParam.TEST_SINGLE.singleLength = 15;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_35H.cmdCode = 0x35;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_35H.Offset = FileOffset;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_35H.Type = Type;
	pcCmdData.uParam.TEST_SINGLE.frameOption = 0x10;

	if(TestSendCmd(&pcCmdData) != 0)
	{
		m_Status2 = "测试列表文件命令失败";
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
 			m_Status2 += "测试列表文件命令返回成功：";
			CPage1 newPage;
			tmpStr = newPage.HexPrintf(pcCmdReply.uReplyParam.testSingleCmd.Data);
			m_Status2 += tmpStr;
		}
		else
			m_Status2 = "测试列表文件命令返回超时";
	}
	else
		m_Status2 = "测试列表文件命令失败";

	UpdateData(FALSE);

// 	if(TestSendCmd(&pcCmdData) == 0)
// 		m_Status2 = "测试列表文件命令成功";
// 	else
// 		m_Status2 = "测试列表文件命令失败";
// 
// 	UpdateData(FALSE);
}


void CPage2::OnBnClickedButton8() //读透明文件
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_Status2.Empty();
	UpdateData(FALSE);

	PC_CMD_DATA pcCmdData;
	InitSingleCmd2(pcCmdData);
	InitParameter2();

	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_36H.RID, Rid2, 3);
	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_36H.TID, Tid2, 8);
	pcCmdData.uParam.TEST_SINGLE.singleLength = 16;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_36H.cmdCode = 0x36;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_36H.Offset = Swap16(Offset);
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_36H.Length = Length;
	pcCmdData.uParam.TEST_SINGLE.frameOption = 0x10;

	if(TestSendCmd(&pcCmdData) != 0)
	{
		m_Status2 = "测试读透明文件命令失败";
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
			m_Status2 += "测试读透明文件命令返回成功：";
			CPage1 newPage;
			tmpStr = newPage.HexPrintf(pcCmdReply.uReplyParam.testSingleCmd.Data);
			m_Status2 += tmpStr;
		}
		else
			m_Status2 = "测试读透明文件命令返回超时";
	}
	else
		m_Status2 = "测试读透明文件命令失败";

	UpdateData(FALSE);

// 	if(TestSendCmd(&pcCmdData) == 0)
// 		m_Status2 = "测读透明文件命令成功";
// 	else
// 		m_Status2 = "测试读透明文件命令失败";
// 
// 	UpdateData(FALSE);
}


void CPage2::OnBnClickedButton9() //更新透明文件
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_Status2.Empty();
	UpdateData(FALSE);

	PC_CMD_DATA pcCmdData;
	InitSingleCmd2(pcCmdData);
	InitParameter2();

	CSerialDlg newDlg;
	int tmpLen = newDlg.StrToHex(m_DataContent.GetBuffer(), DataContent); 

	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_37H.RID, Rid2, 3);
	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_37H.TID, Tid2, 8);
	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_37H.Data, DataContent, tmpLen);
	pcCmdData.uParam.TEST_SINGLE.singleLength = 16 + tmpLen;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_37H.cmdCode = 0x37;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_37H.Offset = Swap16(Offset);
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_37H.Length = Length;
	pcCmdData.uParam.TEST_SINGLE.frameOption = 0x10;

	if(TestSendCmd(&pcCmdData) != 0)
	{
		m_Status2 = "测试更新透明文件命令失败";
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
			m_Status2 += "测试更新透明文件命令返回成功：";
			CPage1 newPage;
			tmpStr = newPage.HexPrintf(pcCmdReply.uReplyParam.testSingleCmd.Data);
			m_Status2 += tmpStr;
		}
		else
			m_Status2 = "测试更新透明文件命令返回超时";
	}
	else
		m_Status2 = "测试更新透明文件命令失败";

	UpdateData(FALSE);

// 	if(TestSendCmd(&pcCmdData) == 0)
// 		m_Status2 = "测试更新透明文件命令成功";
// 	else
// 		m_Status2 = "测试更新透明文件命令失败";
// 
// 	UpdateData(FALSE);
}


void CPage2::OnBnClickedButton10() //读记录文件
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_Status2.Empty();
	UpdateData(FALSE);

	PC_CMD_DATA pcCmdData;
	InitSingleCmd2(pcCmdData);
	InitParameter2();

	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_38H.RID, Rid2, 3);
	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_38H.TID, Tid2, 8);
	pcCmdData.uParam.TEST_SINGLE.singleLength = 17;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_38H.cmdCode = 0x38;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_38H.Offset = Pianyi;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_38H.Length = Length;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_38H.Model = Type;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_38H.RecordPoint = RecordPoint;
	pcCmdData.uParam.TEST_SINGLE.frameOption = 0x10;

	if(TestSendCmd(&pcCmdData) != 0)
	{
		m_Status2 = "测试读记录文件命令失败";
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
			m_Status2 += "测试读记录文件命令返回成功：";
			CPage1 newPage;
			tmpStr = newPage.HexPrintf(pcCmdReply.uReplyParam.testSingleCmd.Data);
			m_Status2 += tmpStr;
		}
		else
			m_Status2 = "测试读记录文件命令返回超时";
	}
	else
		m_Status2 = "测试读记录文件命令失败";

	UpdateData(FALSE);

// 	if(TestSendCmd(&pcCmdData) == 0)
// 		m_Status2 = "测试读记录文件命令成功";
// 	else
// 		m_Status2 = "测试读记录文件命令失败";
// 
// 	UpdateData(FALSE);
}


void CPage2::OnBnClickedButton12() //更新记录文件
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_Status2.Empty();
	UpdateData(FALSE);

	PC_CMD_DATA pcCmdData;
	InitSingleCmd2(pcCmdData);
	InitParameter2();

	CSerialDlg newDlg;
	int tmpLen = newDlg.StrToHex(m_DataContent.GetBuffer(), DataContent);

	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_39H.RID, Rid2, 3);
	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_39H.TID, Tid2, 8);
	pcCmdData.uParam.TEST_SINGLE.singleLength = 16 + tmpLen;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_39H.cmdCode = 0x39;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_39H.RecordPoint = RecordPoint;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_39H.Model = Type;
	pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_39H.Length = Length;
	memcpy(pcCmdData.uParam.TEST_SINGLE.uSingle.TEST_SINGLE_39H.Data, DataContent, tmpLen);
	pcCmdData.uParam.TEST_SINGLE.frameOption = 0x10;

	if(TestSendCmd(&pcCmdData) != 0)
	{
		m_Status2 = "测试更新记录文件命令失败";
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
			m_Status2 += "测试更新记录文件命令返回成功：";
			CPage1 newPage;
			tmpStr = newPage.HexPrintf(pcCmdReply.uReplyParam.testSingleCmd.Data);
			m_Status2 += tmpStr;
		}
		else
			m_Status2 = "测试更新记录文件命令返回超时";
	}
	else
		m_Status2 = "测试更新记录文件命令失败";

	UpdateData(FALSE);

// 	if(TestSendCmd(&pcCmdData) == 0)
// 		m_Status2 = "测试更新记录文件命令成功";
// 	else
// 		m_Status2 = "测试更新记录文件命令失败";
// 
// 	UpdateData(FALSE);
}
