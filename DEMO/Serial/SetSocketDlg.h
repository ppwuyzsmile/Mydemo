#pragma once


// CSetSocketDlg 对话框

class CSetSocketDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSetSocketDlg)

public:
	CSetSocketDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetSocketDlg();

// 对话框数据
	enum { IDD = IDD_SETSOCKET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSetip();
	int m_Port;
	CString m_AddressIP;
	CString m_MaskIP;
	static void SocketConnectThreadFunc(LPVOID lparam);
};
