#pragma once
#include "afxwin.h"


// CPwdRFDlg 对话框

class CPwdRFDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPwdRFDlg)

public:
	CPwdRFDlg(CWnd* pParent = NULL);   // 标准构造函数
	CPwdRFDlg(unsigned int& pwd,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPwdRFDlg();

// 对话框数据
	enum { IDD = IDD_PWD_RF_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
//	CString m_pwd;
	CString m_pwd1;
	bool * result;
	unsigned int m_pwd;
};
