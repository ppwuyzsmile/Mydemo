#pragma once
#include "xpbutton.h"
#include "pcCommand.h"


// CPage2 对话框

class CPage2 : public CDialogEx
{
	DECLARE_DYNAMIC(CPage2)

public:
	CPage2(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPage2();

// 对话框数据
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_TimeSlot;
	CString m_Offset;
	CString m_Length;
	CString m_RecordPoint;
	CString m_Pianyi;
	CString m_SendIndicate;
	CString m_FileID;
	CString m_CMDType;
	CString m_Token;
	CString m_FileOffset;
	CString m_Type;
	CString m_DataContent;
	CString m_Tid2;
	CString m_Status2;
	CString m_Rid2;

	UINT Token;
	short TimeSlot, FileID, Offset;
	char Rid2[3], Tid2[8], DataContent[128];
	BYTE SendIndicate, CMDType, FileOffset, Type, Length, RecordPoint, Pianyi;

	CXPButton m_Page2Button1;
	CXPButton m_Page2Button2;
	CXPButton m_Page2Button3;
	CXPButton m_Page2Button4;
	CXPButton m_Page2Button5;
	CXPButton m_Page2Button6;
	CXPButton m_Page2Button7;
	CXPButton m_Page2Button8;
	CXPButton m_Page2Button9;
	CXPButton m_Page2Button10;
	CXPButton m_Page2Button12;

	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButton12();

	static void InitSingleCmd2(PC_CMD_DATA& pcCmdData);
	void InitParameter2();
};
