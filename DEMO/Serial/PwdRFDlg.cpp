// PwdRFDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Serial.h"
#include "PwdRFDlg.h"
#include "afxdialogex.h"


// CPwdRFDlg �Ի���

IMPLEMENT_DYNAMIC(CPwdRFDlg, CDialogEx)

CPwdRFDlg::CPwdRFDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPwdRFDlg::IDD, pParent)
//	, m_pwd(_T(""))
	, m_pwd(6)
{

}

CPwdRFDlg::CPwdRFDlg( unsigned int& pwd, CWnd* pParent /*= NULL*/ )
	: CDialogEx(CPwdRFDlg::IDD, pParent)
//	, m_pwd(_T(""))
{

}

CPwdRFDlg::~CPwdRFDlg()
{
}

void CPwdRFDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Text(pDX, IDC_EDIT1, m_pwd);
	DDX_Text(pDX, IDC_EDIT1, m_pwd);
}


BEGIN_MESSAGE_MAP(CPwdRFDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CPwdRFDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CPwdRFDlg ��Ϣ�������


void CPwdRFDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData();
	//��������еĿ��������֤
	theApp.m_pwd = m_pwd;
	
	CDialogEx::OnOK();
	
}
