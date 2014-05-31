// TestSingleCmd.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Serial.h"
#include "TestSingleCmd.h"
#include "afxdialogex.h"


// CTestSingleCmd �Ի���

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


// CTestSingleCmd ��Ϣ�������
BOOL CTestSingleCmd::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//ΪTab Control��������ҳ��
	m_Tab.InsertItem(0, _T("�������һ"));
	m_Tab.InsertItem(1, _T("������϶�"));
	m_Tab.InsertItem(2, _T("���������"));

	//���������Ի���
	m_page1.Create(IDD_DIALOG1, &m_Tab);
	m_page2.Create(IDD_DIALOG2, &m_Tab);
	m_page3.Create(IDD_DIALOG3, &m_Tab);
	//�趨��Tab����ʾ�ķ�Χ
	CRect rc;
	m_Tab.GetClientRect(rc);
	rc.top += 20;
	rc.bottom -= 0;
	rc.left += 0;
	rc.right -= 0;
	m_page1.MoveWindow(&rc);
	m_page2.MoveWindow(&rc);
	m_page3.MoveWindow(&rc);

	//�ѶԻ������ָ�뱣������
	pDialog[0] = &m_page1;
	pDialog[1] = &m_page2;
	pDialog[2] = &m_page3;
	//��ʾ��ʼҳ��
	pDialog[0]->ShowWindow(SW_SHOW);
	pDialog[1]->ShowWindow(SW_HIDE);
	pDialog[2]->ShowWindow(SW_HIDE);
	//���浱ǰѡ��
	m_CurSelTab = 0;

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CTestSingleCmd::OnTcnSelchangeTab2(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//�ѵ�ǰ��ҳ����������
	pDialog[m_CurSelTab]->ShowWindow(SW_HIDE);
	//�õ��µ�ҳ������
	m_CurSelTab = m_Tab.GetCurSel();
	//���µ�ҳ����ʾ����
	pDialog[m_CurSelTab]->ShowWindow(SW_SHOW);

	*pResult = 0;
}
