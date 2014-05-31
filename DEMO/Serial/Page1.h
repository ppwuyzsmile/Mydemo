#pragma once
#include "xpbutton.h"
#include "pcCommand.h"

// CPage1 对话框

class CPage1 : public CDialogEx
{
	DECLARE_DYNAMIC(CPage1)

public:
	CPage1(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPage1();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_Rid1;
	CString m_Tid1;
	CString m_Status;
	CString m_DataContent;
	CString m_DataLength;
	CString m_Q;
	CString m_N;
	CString m_SNslot;
	CString m_SNsuccess;
	CString m_RN8;
	CString m_TimeWidth;
	CString m_Indicate;
	CString m_StartTime;
	CString m_EndTime;
	CString m_ChannelNum;
	CString m_ReadyToken;
	CString m_SleepToken;
	CString m_SleepTime;
	CString m_ListenTime;
	CString m_FileNum;
	CString m_PianYi;

	CXPButton m_TestButton1;
	CXPButton m_TestButton2;
	CXPButton m_TestButton3;
	CXPButton m_TestButton6;
	CXPButton m_TestButton7;
	CXPButton m_TestButton8;
	CXPButton m_TestButton9;
	CXPButton m_TestButton10;
	CXPButton m_TestButton12;
	CXPButton m_TestButton13;

	char Rid1[3], Tid1[8], DataContent[256];
	UINT ReadyToken, SleepToken;
	short FileNum, SNsuccess, TimeWidth, StartTime, EndTime;
	BYTE ChannelNum, SleepTime, ListenTime, PianYi, DataLength, Q, N, SNslot, RN8, Indicate;

	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButton12();
	afx_msg void OnBnClickedButton13();

	static void InitSingleCmd1(PC_CMD_DATA& pcCmdData);
	void InitParameter1();
	int StringToHex(CString strTmp);
	int SingleCmdRecvFun(PC_CMD_REPLY *pcCmdReply, PC_CMD_TYPE pcCmdType, unsigned int TimeOut);
	CString HexPrintf(unsigned char *DataBuffer);
};
