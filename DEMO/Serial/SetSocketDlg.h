#pragma once


// CSetSocketDlg �Ի���

class CSetSocketDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSetSocketDlg)

public:
	CSetSocketDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSetSocketDlg();

// �Ի�������
	enum { IDD = IDD_SETSOCKET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSetip();
	int m_Port;
	CString m_AddressIP;
	CString m_MaskIP;
	static void SocketConnectThreadFunc(LPVOID lparam);
};
