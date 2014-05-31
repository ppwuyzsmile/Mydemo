#pragma once
#include "Page1.h"
#include "Page2.h"
#include "Page3.h"

// CTestSingleCmd 对话框

class CTestSingleCmd : public CDialogEx
{
	DECLARE_DYNAMIC(CTestSingleCmd)

public:
	CTestSingleCmd(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTestSingleCmd();

// 对话框数据
	enum { IDD = IDD_TESTSINGLECMD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_Tab;
	int m_CurSelTab; 
	CPage1 m_page1; 
	CPage2 m_page2;
	CPage3 m_page3;
	CDialog* pDialog[3];  //用来保存对话框对象指针
	virtual BOOL OnInitDialog();
	afx_msg void OnTcnSelchangeTab2(NMHDR *pNMHDR, LRESULT *pResult);
};
