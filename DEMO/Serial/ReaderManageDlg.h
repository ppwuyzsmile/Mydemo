#pragma once
#include "afxwin.h"

#define MYUPDATEFALSE (WM_USER + 20)

// CReaderManageDlg �Ի���

class CReaderManageDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CReaderManageDlg)

public:
	CReaderManageDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CReaderManageDlg();

// �Ի�������
	enum { IDD = IDD_READER_MANAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnGetreaderinfo();
	afx_msg void OnBnClickedOk();
	CString m_readInfo;
	// �����Ķ���������
//	CString m_baudRate_w;
	// �����Ķ���ID
	CString m_readerID_w;
	// �����Ķ�������
	CString m_readerName_w;
	virtual BOOL OnInitDialog();
	// �Ķ�����������Combo
	CComboBox m_cmbBaudRate;
	afx_msg void OnBnClickedBtnChangeReaderKey();
	unsigned char m_pubKeyReader[69];
	unsigned char m_priKeyReader[99];
	unsigned long m_pubLenReader;
	unsigned long m_priLenReader;
	unsigned char m_ecdh[0x42];
	afx_msg void OnBnClickedBtnImpotagPubkey();
	CString m_filePathName;
	unsigned char m_pubKeyTag[69];
	CString m_TID;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedBtnSetPower();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();

	/*-------------�̺߳���--------------*/
	static void GetReaderInfoThread(LPVOID lparam);
	static void SetReaderInfoThread(LPVOID lparam);
	static void SetChannelThread(LPVOID lparam);
	static void SetPowerThread(LPVOID lparam);
protected:
	afx_msg LRESULT OnMyupdatefalse(WPARAM wParam, LPARAM lParam);
public:
	BYTE m_IP1;
	BYTE m_IP2;
	BYTE m_IP3;
	BYTE m_IP4;
	BYTE m_MaskIP1;
	BYTE m_MaskIP2;
	BYTE m_MaskIP3;
	BYTE m_MaskIP4;
	int m_Port;
	BYTE m_Q;
	BYTE m_N;
	BYTE m_Gateway1;
	BYTE m_Gateway2;
	BYTE m_Gateway3;
	BYTE m_Gateway4;
	afx_msg void OnBnClickedSettcpip();
	afx_msg void OnBnClickedSetqn();
	CString m_Mac;
	afx_msg void OnBnClickedSetmac();
};
