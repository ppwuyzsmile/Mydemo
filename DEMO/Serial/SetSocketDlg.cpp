// SetSocketDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Serial.h"
#include "SetSocketDlg.h"
#include "SerialDlg.h"
#include "afxdialogex.h"
#include <WinSock.h>
#include "..\..\API����\APIDLL\apidll.h"

SOCKET sockRet;//����IPͨ�ųɹ����ص�SOCKETȫ���׽��֣����������շ����ݵĲ���

struct{
	CString AddressIP;
	int Port;
}paramIP;

// CSetSocketDlg �Ի���

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


// CSetSocketDlg ��Ϣ�������


void CSetSocketDlg::OnBnClickedSetip()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	CSerialDlg *pdlg= ((CSerialDlg *)(theApp.m_pMainWnd));
	paramIP.AddressIP = m_AddressIP;
	paramIP.Port = m_Port;

	HANDLE startThread;
	startThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SocketConnectThreadFunc, &paramIP, 0, NULL);
	
	DWORD Ret = WaitForSingleObject(startThread, 1000);
	if (Ret == WAIT_TIMEOUT)
	{
		pdlg->m_statusbar = "������ͨ��ʧ��";
		pdlg->UpdateData(FALSE);
		CDialogEx::OnOK();//������Ի����˳�
		return;
	}

	pdlg->GetDlgItem(IDC_OpenSocket)->EnableWindow(FALSE);
	pdlg->m_statusbar = "������ͨ�ųɹ�";
	pdlg->m_OpenCommunication = TRUE;
	pdlg->UpdateData(FALSE);
	CDialogEx::OnOK();//������Ի����˳�
}

void CSetSocketDlg::SocketConnectThreadFunc(LPVOID lparam)
{
	CSetSocketDlg *pdlg = new CSetSocketDlg;

	char *addressIP = (LPSTR)(LPCTSTR)(paramIP.AddressIP);//��CString����ת��Ϊchar*����
	int Ret = StartConnect(paramIP.Port, addressIP);
	if(Ret == 0)
	{
		Sleep(1500);
	}
}