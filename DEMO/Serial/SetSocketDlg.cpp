// SetSocketDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Serial.h"
#include "SetSocketDlg.h"
#include "SerialDlg.h"
#include "afxdialogex.h"
#include <WinSock.h>
#include "..\..\API二期\APIDLL\apidll.h"

SOCKET sockRet;//查找IP通信成功返回的SOCKET全局套接字，用于网口收发数据的参数

struct{
	CString AddressIP;
	int Port;
}paramIP;

// CSetSocketDlg 对话框

IMPLEMENT_DYNAMIC(CSetSocketDlg, CDialogEx)

CSetSocketDlg::CSetSocketDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSetSocketDlg::IDD, pParent)
	, m_Port(7086)
	, m_AddressIP("192.168.1.200")
	, m_MaskIP("255.255.255.255")
{

}

CSetSocketDlg::~CSetSocketDlg()
{
}

void CSetSocketDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT3, m_Port);
	DDX_Text(pDX, IDC_EDIT1, m_AddressIP);
	DDX_Text(pDX, IDC_EDIT2, m_MaskIP);
}


BEGIN_MESSAGE_MAP(CSetSocketDlg, CDialogEx)
	ON_BN_CLICKED(IDC_SetIP, &CSetSocketDlg::OnBnClickedSetip)
END_MESSAGE_MAP()


// CSetSocketDlg 消息处理程序


void CSetSocketDlg::OnBnClickedSetip()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CSerialDlg *pdlg= ((CSerialDlg *)(theApp.m_pMainWnd));
	paramIP.AddressIP = m_AddressIP;
	paramIP.Port = m_Port;

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SocketConnectThreadFunc, &paramIP, 0, NULL);
	
	DWORD Ret = WaitForSingleObject(startThread, 1000);
	if (Ret == WAIT_TIMEOUT)
	{
		pdlg->m_statusbar = "打开网口通信失败";
		pdlg->UpdateData(FALSE);
		CDialogEx::OnOK();//设置完对话框退出
		return;
	}

	pdlg->GetDlgItem(IDC_OpenSocket)->EnableWindow(FALSE);
	pdlg->m_statusbar = "打开网口通信成功";
	pdlg->m_OpenCommunication = TRUE;
	pdlg->UpdateData(FALSE);
	CDialogEx::OnOK();//设置完对话框退出
}

void CSetSocketDlg::SocketConnectThreadFunc(LPVOID lparam)
{
	CSetSocketDlg *pdlg = new CSetSocketDlg;

	char *addressIP = (LPSTR)(LPCTSTR)(paramIP.AddressIP);//将CString类型转换为char*类型
	int Ret = StartConnect(paramIP.Port, addressIP);
	if(Ret == 0)
	{
		Sleep(1500);
	}
}