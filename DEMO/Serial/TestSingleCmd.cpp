// TestSingleCmd.cpp : 实现文件
//

#include "stdafx.h"
#include "Serial.h"
#include "TestSingleCmd.h"
#include "afxdialogex.h"


// CTestSingleCmd 对话框

IMPLEMENT_DYNAMIC(CTestSingleCmd, CDialogEx)

CTestSingleCmd::CTestSingleCmd(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestSingleCmd::IDD, pParent)
{

}

CTestSingleCmd::~CTestSingleCmd()
{
}

void CTestSingleCmd::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TAB2, m_Tab);
}


BEGIN_MESSAGE_MAP(CTestSingleCmd, CDialogEx)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB2, &CTestSingleCmd::OnTcnSelchangeTab2)
END_MESSAGE_MAP()


// CTestSingleCmd 消息处理程序
BOOL CTestSingleCmd::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//为Tab Control增加三个页面
	m_Tab.InsertItem(0, _T("单命令集合一"));
	m_Tab.InsertItem(1, _T("单命令集合二"));
	m_Tab.InsertItem(2, _T("单命令集合三"));

	//创建三个对话框
	m_page1.Create(IDD_DIALOG1, &m_Tab);
	m_page2.Create(IDD_DIALOG2, &m_Tab);
	m_page3.Create(IDD_DIALOG3, &m_Tab);
	//设定在Tab内显示的范围
	CRect rc;
	m_Tab.GetClientRect(rc);
	rc.top += 20;
	rc.bottom -= 0;
	rc.left += 0;
	rc.right -= 0;
	m_page1.MoveWindow(&rc);
	m_page2.MoveWindow(&rc);
	m_page3.MoveWindow(&rc);

	//把对话框对象指针保存起来
	pDialog[0] = &m_page1;
	pDialog[1] = &m_page2;
	pDialog[2] = &m_page3;
	//显示初始页面
	pDialog[0]->ShowWindow(SW_SHOW);
	pDialog[1]->ShowWindow(SW_HIDE);
	pDialog[2]->ShowWindow(SW_HIDE);
	//保存当前选择
	m_CurSelTab = 0;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CTestSingleCmd::OnTcnSelchangeTab2(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	//把当前的页面隐藏起来
	pDialog[m_CurSelTab]->ShowWindow(SW_HIDE);
	//得到新的页面索引
	m_CurSelTab = m_Tab.GetCurSel();
	//把新的页面显示出来
	pDialog[m_CurSelTab]->ShowWindow(SW_SHOW);

	*pResult = 0;
}
