#pragma once
#include "afxwin.h"


// CPwdRFDlg �Ի���

class CPwdRFDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPwdRFDlg)

public:
	CPwdRFDlg(CWnd* pParent = NULL);   // ��׼���캯��
	CPwdRFDlg(unsigned int& pwd,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPwdRFDlg();

// �Ի�������
	enum { IDD = IDD_PWD_RF_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
//	CString m_pwd;
	CString m_pwd1;
	bool * result;
	unsigned int m_pwd;
};
