#include "stdafx.h"
#include "Serial.h"
#include "SerialDlg.h"
#include "afxdialogex.h"
#include "io.h"
using namespace std;

void CSerialDlg::OnSize(UINT nType, int cx, int cy)
{
	// TODO: 在此处添加消息处理程序代码
	if (cx<(m_originalRectSize.Width()) || 
		cy< ( m_originalRectSize.Height()) )
	{
		//theApp.GetMainWnd()->MoveWindow(m_originalRectSize);
		return;
	}
	CDialogEx::OnSize(nType, cx, cy);
	CWnd *pWnd;
	//	CWnd *pWndBtn;
	pWnd = GetDlgItem(IDC_LIST1);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_LIST2);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_LIST3);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小

	}
	pWnd = GetDlgItem(IDC_STATIC_GROUP1);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小

	}
	pWnd = GetDlgItem(IDC_EDIT10);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小

	}
	pWnd = GetDlgItem(IDC_EDIT25);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小

	}
	pWnd = GetDlgItem(IDC_EDIT3);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小

	}
	pWnd = GetDlgItem(IDC_STATIC_GROUP2);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_BUTTON2);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_BUTTON4);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_BUTTON10);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	for (int i = IDC_STATIC_C1;i<=IDC_STATIC_C5;i++)
	{
		pWnd = GetDlgItem(i);
		if (pWnd)
		{
			CRect rect;
			pWnd->GetWindowRect(&rect);
			ScreenToClient(rect);
			rect.left = rect.left*cx/m_rect.Width();
			rect.right=rect.right*cx/m_rect.Width();
			rect.top=rect.top*cy/m_rect.Height();
			rect.bottom=rect.bottom*cy/m_rect.Height();
			pWnd->MoveWindow(rect);//设置控件大小
		}
	}
	pWnd = GetDlgItem(IDC_COMBO1);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height()+150;
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_COMBO2);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height()+100;
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_COMBO3);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height()+100;
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_COMBO4);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height()+100;
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_COMBO5);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height()+100;
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_BUTTON3);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_STATIC_GOODSINFO);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_STATIC_RECFILE);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_BUTTON12);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_BUTTON6);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_BUTTON11);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_BUTTON5);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_BUTTON13);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_BUTTON14);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_BUTTON7);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	for (int i = IDC_STATIC_C6;i<=IDC_STATIC_C10;i++)
	{
		pWnd = GetDlgItem(i);
		if (pWnd)
		{
			CRect rect;
			pWnd->GetWindowRect(&rect);
			ScreenToClient(rect);
			rect.left = rect.left*cx/m_rect.Width();
			rect.right=rect.right*cx/m_rect.Width();
			rect.top=rect.top*cy/m_rect.Height();
			rect.bottom=rect.bottom*cy/m_rect.Height();
			pWnd->MoveWindow(rect);//设置控件大小
		}
	}
	pWnd = GetDlgItem(IDC_EDIT7);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_EDIT1);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_EDIT4);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_EDIT5);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_EDIT6);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_EDIT8);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_EDIT9);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_EDIT11);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_EDIT_RXDATA);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDCANCEL);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_BUTTON8);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_COMBO6);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height()+100;
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_COMBO7);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height()+100;
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_COMBO8);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height()+100;
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_COMBO9);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height()+100;
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_COMBO10);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height()+100;
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_STATIC_LEN1);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_STATIC_LEN2);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_STATIC_LEN3);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_STATIC_SECU1);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_STATIC_SECU2);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_STATIC1);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_BUTTON16);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_STATIC_SEQATTR);
	pWnd = GetDlgItem(IDC_STATIC_CREAT);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_TESTBOARD);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_STATIC_SEQATTR);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_BUTTON1);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_READERTEST);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_CMDTEST);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_BUTTON15);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_STATIC_REGISTERTAG);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_STATIC_TID);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_EDIT12);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_BTN_REGISTER_TAG);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_BTN_SAFE_SESSION);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_STATIC_MIK);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_EDIT18);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_STATIC_MEK);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_EDIT19);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_BTN_GETTAGINFO);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_BTN_WAKEUPONETAG);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_BTN_CLEANALLTAG);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_BTN_FILESYS_FORMAT);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_BTN_FILE_DELETE);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_BTN_IMPOTAG_PUBKEY);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_BTN_KILL_TAG);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_BTN_KEY_WIRELESS);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_STATIC_RX2);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_STATIC_RX3);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_STATIC_RX4);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_STATIC_RX5);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_BTN_UPDATE_PWD);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_EDIT_ADMIN_PWD);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_EDIT_PWD_INDEX);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_COMBO_UPDATE_PWD_MODE);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height()+100;

		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_EDIT_NEW_PWD);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_STATIC_LINE1);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_STATIC_LINE2);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_STATIC_LINE3);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_BTN_WRITE_LARGE_FILE);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_OpenSocket);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_CloseSocket);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_STATIC);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_COMBO12);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height()+100;
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_IPEDIT);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_IPMASKEDIT);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_IPPORTEDIT);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_STATICIP);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_STATICIPMASK);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_STATICPORT);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_SEARCHRECORD);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_SEARCHCONDITION);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_WAKEUP);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}
	pWnd = GetDlgItem(IDC_WAKEUPNUM);
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		rect.left = rect.left*cx/m_rect.Width();
		rect.right=rect.right*cx/m_rect.Width();
		rect.top=rect.top*cy/m_rect.Height();
		rect.bottom=rect.bottom*cy/m_rect.Height();
		pWnd->MoveWindow(rect);//设置控件大小
	}


	GetClientRect(&m_rect);//将变化后的对话框大小设为旧大小
	InvalidateRect(&m_rect,1);
}