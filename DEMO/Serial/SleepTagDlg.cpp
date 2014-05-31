// SleepTagDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Serial.h"
#include "XPButton.h"
#include "SleepTagDlg.h"
#include "afxdialogex.h"
#include "SerialDlg.h"
#include "Page1.h"
#include "pcCommand.h"
#include "..\..\API二期\APIDLL\apidll.h"


// CSleepTagDlg 对话框

IMPLEMENT_DYNAMIC(CSleepTagDlg, CDialogEx)

CSleepTagDlg::CSleepTagDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSleepTagDlg::IDD, pParent)
	, m_sleepExOneTagTID(_T("a1a2a3a4a5a6a7a8"))
	, m_sleepExOnesTagDataBuffer(_T("00"))
{

	m_sleepTimeAllTag = _T("01");
	m_sleepCode = _T("00000000");
	m_sleepExOnesTagCode = _T("00000000");
	m_sleepExOnesTagFileID = _T("a105");
	m_sleepExOnesTagOffset = _T("00");
	m_sleepExOnesTagDataLen = _T("00");
	m_sleepExOnesTagSleepTime = _T("01");
	m_sleepExOneTagCode = _T("00000000");
	m_sleepExOneTagSleepTime = _T("01");
	m_listenTime = _T("01");
	m_sleepExOneTagListenTime = _T("01");
	m_sleepExOnesTagListenTime = _T("01");
}

CSleepTagDlg::~CSleepTagDlg()
{
}

void CSleepTagDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT22, m_sleepExOneTagTID);
	/*DDV_MaxChars(pDX, m_sleepExOneTagTID, 8);*/
	DDX_Text(pDX, IDC_EDIT17, m_sleepExOnesTagDataBuffer);
	DDX_Text(pDX, IDC_EDIT1, m_sleepTimeAllTag);
	DDX_Text(pDX, IDC_EDIT2, m_sleepCode);
	DDX_Text(pDX, IDC_EDIT13, m_sleepExOnesTagCode);
	DDX_Text(pDX, IDC_EDIT14, m_sleepExOnesTagFileID);
	DDX_Text(pDX, IDC_EDIT15, m_sleepExOnesTagOffset);
	DDX_Text(pDX, IDC_EDIT16, m_sleepExOnesTagDataLen);
	DDX_Text(pDX, IDC_EDIT20, m_sleepExOnesTagSleepTime);
	DDX_Text(pDX, IDC_EDIT23, m_sleepExOneTagCode);
	DDX_Text(pDX, IDC_EDIT24, m_sleepExOneTagSleepTime);
	DDX_Text(pDX, IDC_EDIT7, m_listenTime);
	DDX_Text(pDX, IDC_EDIT8, m_sleepExOneTagListenTime);
	DDX_Text(pDX, IDC_EDIT9, m_sleepExOnesTagListenTime);
}


BEGIN_MESSAGE_MAP(CSleepTagDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSleepTagDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CSleepTagDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CSleepTagDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CSleepTagDlg 消息处理程序


BOOL CSleepTagDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}



void CSleepTagDlg::OnBnClickedOk()	// 休眠所有标签
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	CPage1 newPage;
	BYTE sleepTimeAllTag = newPage.StringToHex(m_sleepTimeAllTag);
	UINT sleepCode = newPage.StringToHex(m_sleepCode);
	BYTE listenTime = newPage.StringToHex(m_listenTime);//毫秒
	//调用休眠所有标签API
	int Ret = rfSleepAllTag(sleepTimeAllTag, sleepCode, listenTime);

	switch(Ret)
	{
	case IF_TYPE_SLEEP_ALL_TAG | SLEEP_TAG_FAILED:
		AfxMessageBox("休眠所有标签命令发送失败");
		break;
	case IF_TYPE_SLEEP_ALL_TAG | SLEEP_TAG_TIMEOUT:
		AfxMessageBox("休眠所有标签超时");
		break;
	case IF_TYPE_SLEEP_ALL_TAG | SLEEP_TAG_SUCCESS:
		{
			AfxMessageBox("设置成功");
			CDialogEx::OnOK();
		}
		break;
	default:
		AfxMessageBox("未知错误，请重新设置");
		break;
	}
}


void CSleepTagDlg::OnBnClickedButton1()	// 休眠除某一类标签外所有标签
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CSerialDlg newDlg;
	CPage1 newPage;
	BYTE dataBuffer[80];
	BYTE sleepExOnesTagSleepTime = newPage.StringToHex(m_sleepExOnesTagSleepTime);
	UINT sleepExOnesTagCode = newPage.StringToHex(m_sleepExOnesTagCode);
	WORD sleepExOnesTagFileID = newPage.StringToHex(m_sleepExOnesTagFileID);
	BYTE sleepExOnesTagOffset = newPage.StringToHex(m_sleepExOnesTagOffset);
	BYTE sleepExOnesTagDataLen = newPage.StringToHex(m_sleepExOnesTagDataLen);
	BYTE BufferLen = newDlg.StrToHex(m_sleepExOnesTagDataBuffer, (char*)dataBuffer);
	BYTE listenTime = newPage.StringToHex(m_sleepExOnesTagListenTime); //毫秒

	if((sleepExOnesTagDataLen > 80) || (sleepExOnesTagDataLen > BufferLen))
	{
		MessageBox("数据长度不能大于80，且输入的数据长度不能大于数据内容的真实长度");
		return;
	}
	
	//开始调用API函数
	int Ret = rfSleepExOnesTag(sleepExOnesTagSleepTime, 
							   sleepExOnesTagCode,
							   listenTime,
							   sleepExOnesTagFileID,
							   sleepExOnesTagOffset,
							   sleepExOnesTagDataLen,
							   dataBuffer);

	switch(Ret)
	{
	case IF_TYPE_SLEEP_EX_ONES_TAG | SLEEP_EX_ONES_TAG_STATE_FAILED:
		AfxMessageBox("发送休眠除某一类标签外所有标签命令错误");
		break;
	case IF_TYPE_SLEEP_EX_ONES_TAG | SLEEP_EX_ONES_TAG_STATE_TIMEOUT:
		AfxMessageBox("休眠除某一类标签外所有标签超时");
		break;
	case IF_TYPE_SLEEP_EX_ONES_TAG | SLEEP_EX_ONES_TAG_STATE_SUCCESS:
		{
			AfxMessageBox("设置成功");
			CDialogEx::OnOK();
		}
		break;
	default:
		AfxMessageBox("未知错误，请重新设置");
		break;
	}
}



void CSleepTagDlg::OnBnClickedButton2()	// 休眠除某个标签外所有标签
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	CSerialDlg newDlg;
	CPage1 newPage;
	BYTE TID[8];

	BYTE sleepExOneTagSleepTime = newPage.StringToHex(m_sleepExOneTagSleepTime);
	UINT sleepExOneTagCode = newPage.StringToHex(m_sleepExOneTagCode);
	BYTE tidLen = newDlg.StrToHex(m_sleepExOneTagTID, (char*)TID);
	BYTE listenTime = newPage.StringToHex(m_sleepExOneTagListenTime); //毫秒

	int Ret = rfSleepExOneTag(TID, sleepExOneTagSleepTime, sleepExOneTagCode, listenTime);
	switch(Ret)
	{
	case IF_TYPE_SLEEP_EX_ONE_TAG | SLEEP_EX_ONE_TAG_STATE_FAILED:
		AfxMessageBox("发送休眠某个标签以外命令错误");
		break;
	case IF_TYPE_SLEEP_EX_ONE_TAG | SLEEP_EX_ONE_TAG_STATE_TIMEOUT:
		AfxMessageBox("休眠除某个标签外所有标签超时");
		break;
	case IF_TYPE_SLEEP_EX_ONE_TAG | SLEEP_EX_ONE_TAG_STATE_SUCCESS:
		{
			AfxMessageBox("设置成功");
			CDialogEx::OnOK();
		}
		break;
	default:
		AfxMessageBox("未知错误，请重新设置");
		break;
	}
}
