#pragma once
#include "xpbutton.h"
#include "pcCommand.h"


// CPage3 对话框

class CPage3 : public CDialogEx
{
	DECLARE_DYNAMIC(CPage3)

public:
	CPage3(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPage3();

// 对话框数据
	enum { IDD = IDD_DIALOG3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_Status3;
	CString m_Rid3;
	CString m_Tid3;
	CString m_FindCondition;
	CString m_BoaderCastData;
	CString m_TimeWidth;
	CString m_SendTimeNum;
	CString m_SendRecvIndicate;
	CString m_SensorID;
	CString m_SensorState;
	CString m_DeactiveCode;
	CString m_AdminPWD;
	CString m_PWDIndex;
	CString m_Mode;
	CString m_NewPWD;
	CString m_ReadyToken;
	CString m_RandomLen;
	CString m_FileID;
	CString m_Offset;
	CString m_DataLength;
	CString m_TimeNum;
	CString m_UpCycle;
	CString m_MonitorCycle;

	char Rid3[3], Tid3[8], FindCondition[128], BoaderCastData[128];
	short FileID, TimeNum, UpCycle, MonitorCycle, TimeWidth, SendTimeNum, PWDIndex;
	BYTE Offset, DataLength, SendRecvIndicate, SensorID, SensorState, Mode, RandomLen;
	UINT DeactiveCode, AdminPWD, NewPWD, ReadyToken;

	CXPButton m_Page3Button1;
	CXPButton m_Page3Button2;
	CXPButton m_Page3Button3;
	CXPButton m_Page3Button4;
	CXPButton m_Page3Button5;
	CXPButton m_Page3Button6;
	CXPButton m_Page3Button7;
	CXPButton m_Page3Button8;
	CXPButton m_Page3Button9;

	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton9();

	static void InitSingleCmd3(PC_CMD_DATA& pcCmdData);
	void InitParameter3();
};
