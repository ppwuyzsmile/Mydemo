#pragma once


// CSleepTagDlg 对话框

class CSleepTagDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSleepTagDlg)

public:
	CSleepTagDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSleepTagDlg();

// 对话框数据
	enum { IDD = IDD_SLEEPTIME_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
//	int m_sleepTime;
	// 休眠所有标签——休眠口令
//	unsigned int m_sleepCode;
	// 休眠所有标签——休眠时间
//	unsigned char m_sleepTimeAllTag;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	// 休眠除某一类标签外所有标签——口令
//	unsigned int m_sleepExOnesTagCode;
	// 休眠除某一类标签外所有标签——文件标识符
//	short m_sleepExOnesTagFileID;
	// 休眠除某一类标签外所有标签——偏移
//	unsigned char m_sleepExOnesTagOffset;
	// 休眠除某一类标签外所有标签——数据长度
//	unsigned char m_sleepExOnesTagDataLen;
	// 休眠除某一类标签外所有标签——数据内容
	CString m_sleepExOnesTagDataBuffer;
	// 休眠除某一类标签外所有标签——休眠时间
//	unsigned char m_sleepExOnesTagSleepTime;
	// 休眠除某个标签所有标签——TID
	CString m_sleepExOneTagTID;
	// 休眠除某个标签外所有标签——休眠口令
//	unsigned int m_sleepExOneTagCode;
	// 休眠除某个标签外所有标签——休眠时间
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
