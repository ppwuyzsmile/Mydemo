#pragma once
#include "Page1.h"
#include "Page2.h"
#include "Page3.h"

// CTestSingleCmd �Ի���

class CTestSingleCmd : public CDialogEx
{
	DECLARE_DYNAMIC(CTestSingleCmd)

public:
	CTestSingleCmd(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTestSingleCmd();

// �Ի�������
	enum { IDD = IDD_TESTSINGLECMD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_Tab;
	int m_CurSelTab; 
	CPage1 m_page1; 
	CPage2 m_page2;
	CPage3 m_page3;
	CDialog* pDialog[3];  //��������Ի������ָ��
	virtual BOOL OnInitDialog();
	afx_msg void OnTcnSelchangeTab2(NMHDR *pNMHDR, LRESULT *pResult);
};
