#pragma once


// CRegisterTagDlg 对话框

class CRegisterTagDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRegisterTagDlg)

public:
	CRegisterTagDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRegisterTagDlg();

// 对话框数据
	enum { IDD = IDD_REGISTER_TAG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_TID;
	CString m_FileID;
	CString m_Offset;
	CString m_DataLen;
	CString m_SlotNum;
	CString m_UpCycle;
	CString m_JianceCycle;
	CString m_SlotWidth;
	CString m_SlotSum;
	CString m_SendRecvIndicate;
	CString m_JianceTid;
	CString m_status;
	CString m_UII;
	CString m_filePathName;
	unsigned int m_memoryVolume;  // 存储器容量
	unsigned char m_inqMaxNum;  // 查询元素最大数量
	unsigned int m_inqMaxMatchNum;  // 查询结果最大匹配数量
	CString m_MIK;
	CString m_MEK;
	unsigned char m_pubKeyReader[69];
	unsigned char m_pubKeyTag[69];
	unsigned char m_priKeyTag[99];
	unsigned long m_pubLenTag;
	unsigned long m_priLenTag;
	
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedBtnChangeTagKey();
	afx_msg void OnBnClickedBtnImporeadPubkey();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedOk();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton17();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedBtnGetTagInfo();
	afx_msg void OnBnClickedBtnGetChannel();
	afx_msg void OnBnClickedBtnGetPower();
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButton12();
	afx_msg void OnBnClickedButton13();

	/*-----------------线程函数------------------*/
	static void GetTagChannelThread(LPVOID lparam);
	static void SetTagChannelThread(LPVOID lparam);
	static void GetTagPowerThread(LPVOID lparam);
	static void SetTagPowerThread(LPVOID lparam);
	static void GetTagInfoThread(LPVOID lparam);
	static void SetTagInfoThread(LPVOID lparam);
	static void StartJianceThread(LPVOID lparam);
};

