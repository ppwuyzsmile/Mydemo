#pragma once


// CSleepTagDlg �Ի���

class CSleepTagDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSleepTagDlg)

public:
	CSleepTagDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSleepTagDlg();

// �Ի�������
	enum { IDD = IDD_SLEEPTIME_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
//	int m_sleepTime;
	// �������б�ǩ�������߿���
//	unsigned int m_sleepCode;
	// �������б�ǩ��������ʱ��
//	unsigned char m_sleepTimeAllTag;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	// ���߳�ĳһ���ǩ�����б�ǩ��������
//	unsigned int m_sleepExOnesTagCode;
	// ���߳�ĳһ���ǩ�����б�ǩ�����ļ���ʶ��
//	short m_sleepExOnesTagFileID;
	// ���߳�ĳһ���ǩ�����б�ǩ����ƫ��
//	unsigned char m_sleepExOnesTagOffset;
	// ���߳�ĳһ���ǩ�����б�ǩ�������ݳ���
//	unsigned char m_sleepExOnesTagDataLen;
	// ���߳�ĳһ���ǩ�����б�ǩ������������
	CString m_sleepExOnesTagDataBuffer;
	// ���߳�ĳһ���ǩ�����б�ǩ��������ʱ��
//	unsigned char m_sleepExOnesTagSleepTime;
	// ���߳�ĳ����ǩ���б�ǩ����TID
	CString m_sleepExOneTagTID;
	// ���߳�ĳ����ǩ�����б�ǩ�������߿���
//	unsigned int m_sleepExOneTagCode;
	// ���߳�ĳ����ǩ�����б�ǩ��������ʱ��
//	unsigned char m_sleepExOneTagSleepTime;
	
	CString m_sleepTimeAllTag;
	CString m_sleepCode;
	CString m_sleepExOnesTagCode;
	CString m_sleepExOnesTagFileID;
	CString m_sleepExOnesTagOffset;
	CString m_sleepExOnesTagDataLen;
	CString m_sleepExOnesTagSleepTime;
	CString m_sleepExOneTagCode;
	CString m_sleepExOneTagSleepTime;
	CString m_listenTime;
	CString m_sleepExOneTagListenTime;
	CString m_sleepExOnesTagListenTime;
};
