// XPButton.cpp : implementation file
//

#include "stdafx.h"
#include "XPButton.h"
#include "resource.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXPButton

CXPButton::CXPButton()
{
	m_BoundryPen.CreatePen(PS_INSIDEFRAME | PS_SOLID, 1, RGB(0, 0, 0));
	m_InsideBoundryPenLeft.CreatePen(PS_INSIDEFRAME | PS_SOLID, 3, RGB(250, 196, 88)); 
	m_InsideBoundryPenRight.CreatePen(PS_INSIDEFRAME | PS_SOLID, 3, RGB(251, 202, 106));
	m_InsideBoundryPenTop.CreatePen(PS_INSIDEFRAME | PS_SOLID, 2, RGB(252, 210, 121));
	m_InsideBoundryPenBottom.CreatePen(PS_INSIDEFRAME | PS_SOLID, 2, RGB(229, 151, 0));
	
	m_FillActive.CreateSolidBrush(RGB(223, 222, 236));
	m_FillInactive.CreateSolidBrush(RGB(222, 223, 236));
	
	m_InsideBoundryPenLeftSel.CreatePen(PS_INSIDEFRAME | PS_SOLID, 3, RGB(153, 198, 252)); 
	m_InsideBoundryPenTopSel.CreatePen(PS_INSIDEFRAME | PS_SOLID, 2, RGB(162, 201, 255));
	m_InsideBoundryPenRightSel.CreatePen(PS_INSIDEFRAME | PS_SOLID, 3, RGB(162, 189, 252));
	m_InsideBoundryPenBottomSel.CreatePen(PS_INSIDEFRAME | PS_SOLID, 2, RGB(162, 201, 255));
	
	m_bOver = m_bSelected = m_bTracking = m_bFocus = FALSE;
	
}

CXPButton::~CXPButton()
{
	m_BoundryPen.DeleteObject();
	m_InsideBoundryPenLeft.DeleteObject();
	m_InsideBoundryPenRight.DeleteObject();
	m_InsideBoundryPenTop.DeleteObject();
	m_InsideBoundryPenBottom.DeleteObject();
	
	m_FillActive.DeleteObject();
	m_FillInactive.DeleteObject();
	
	m_InsideBoundryPenLeftSel.DeleteObject();
	m_InsideBoundryPenTopSel.DeleteObject();
	m_InsideBoundryPenRightSel.DeleteObject();
	m_InsideBoundryPenBottomSel.DeleteObject();
	
}


BEGIN_MESSAGE_MAP(CXPButton, CButton)
	//{{AFX_MSG_MAP(CXPButton)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXPButton message handlers

//添加Owner Draw属性
void CXPButton::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CButton::PreSubclassWindow();
	ModifyStyle(0, BS_OWNERDRAW);//|BS_BITMAP
}

void CXPButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);
	}
	
	CButton::OnMouseMove(nFlags, point);
}


LRESULT CXPButton::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bOver = FALSE;
	m_bTracking = FALSE;
	InvalidateRect(NULL, FALSE);
	return 0;
}

LRESULT CXPButton::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
	m_bOver = TRUE;
	InvalidateRect(NULL);
	return 0;
}
/*
*	BOOL m_bOver;		//鼠标位于按钮之上时该值为true，反之为flase
*	BOOL m_bTracking;	//在鼠标按下没有释放时该值为true
*	BOOL m_bSelected;	//按钮被按下是该值为true
*	BOOL m_bFocus;		//按钮为当前焦点所在时该值为true
*/

void CXPButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC ButtonDC;
	CBitmap bitmapTrans;
	BITMAP bmp;
	CDC mem;
	CRect rc;
	DWORD uCtlId = lpDrawItemStruct->CtlID;
	//得到用于绘制按钮的DC
	ButtonDC.Attach(lpDrawItemStruct->hDC);
	//准备用于向按钮区域传输位图
	mem.CreateCompatibleDC(&ButtonDC);
	//获取按钮所占的矩形大小
	rc=lpDrawItemStruct->rcItem;
	//获取按钮目前所处的状态，根据不同的状态绘制不同的按钮
	UINT state1 = lpDrawItemStruct->itemState;
	//如果按钮已经得到焦点，绘制选中状态下的按钮
	//////////////////////////////////////////////////////////////////////////
	//从lpDrawItemStruct获取控件的相关信息
	CRect rect =  lpDrawItemStruct->rcItem;
	CDC *pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	int nSaveDC=pDC->SaveDC();
	UINT state = lpDrawItemStruct->itemState;
	POINT pt ;
	TCHAR strText[MAX_PATH + 1];
	::GetWindowText(m_hWnd, strText, MAX_PATH);

	//画按钮的外边框，它是一个半径为5的圆角矩形
	pt.x = 5;
	pt.y = 5;
	CPen* hOldPen = pDC->SelectObject(&m_BoundryPen);
	pDC->RoundRect(&rect, pt);

	//获取按钮的状态
	if (state & ODS_FOCUS)
	{
		m_bFocus = TRUE;
		m_bSelected = TRUE;
	}
	else
	{
		m_bFocus = FALSE;
		m_bSelected = FALSE;
	}
	if (state & ODS_SELECTED || state & ODS_DEFAULT)
	{
		m_bFocus = TRUE;
	}
	if (m_bOver)
	{
		if ( state&ODS_SELECTED)
		{
			m_bSelected =FALSE;
			switch (uCtlId)
			{
// 			case IDC_BUTTON16:
// 				bitmapTrans.LoadBitmap(IDB_BTN_HOVER);break;
// 			case  IDC_BTN_MINISIZE:
// 				bitmapTrans.LoadBitmap(IDB_MINI_DOWN);break;
// 			case  IDC_BTN_NEXT:
// 			case  IDC_BTN_BACK:
// 				bitmapTrans.LoadBitmap(IDB_BTN_DWON);break;
			default:
				bitmapTrans.LoadBitmap(IDB_BTN_PUSH);break;
			}

			bitmapTrans.GetBitmap(&bmp);
			CBitmap *old=mem.SelectObject(&bitmapTrans);
			//向按钮所在位置传输位图
			//使用StretcnBlt的目的是为了让位图随按钮的大小而改变
			ButtonDC.StretchBlt(rc.left,rc.top,rc.right,rc.bottom,&mem,0,0,bmp.bmWidth,bmp.bmHeight,SRCCOPY);
			mem.SelectObject(old);
			bitmapTrans.DeleteObject();
			
			
		}
		else
		{
			switch (uCtlId)
			{
// 			case IDC_BTN_CLOSE:
// 				bitmapTrans.LoadBitmap(IDB_CLOSE_HILIGHT);break;
// 			case  IDC_BTN_MINISIZE:
// 				bitmapTrans.LoadBitmap(IDB_MINI_HILIGHT);break;
// 			case  IDC_BTN_NEXT:
// 			case  IDC_BTN_BACK:
// 				bitmapTrans.LoadBitmap(IDB_BTN_HOVER);break;
			default:
				bitmapTrans.LoadBitmap(IDB_BTN_HOVER);break;
			}
			bitmapTrans.GetBitmap(&bmp);
			CBitmap *old=mem.SelectObject(&bitmapTrans);
			//向按钮所在位置传输位图
			//使用StretcnBlt的目的是为了让位图随按钮的大小而改变
			ButtonDC.StretchBlt(rc.left,rc.top,rc.right,rc.bottom,&mem,0,0,bmp.bmWidth,bmp.bmHeight,SRCCOPY);
			mem.SelectObject(old);
			bitmapTrans.DeleteObject();
			//设置文字背景为透明
			//ButtonDC.SetBkMode(TRANSPARENT);
		}
	}
	else
	{
		switch (uCtlId)
		{
// 		case IDC_BTN_CLOSE:
// 			bitmapTrans.LoadBitmap(IDB_CLOSE_NOR);break;
// 		case  IDC_BTN_MINISIZE:
// 			bitmapTrans.LoadBitmap(IDB_MINI_NOR);break;
// 		case  IDC_BTN_NEXT:
		case  IDC_BUTTON16:
			bitmapTrans.LoadBitmap(IDB_BTN_NORMAL);break;
		default:
			bitmapTrans.LoadBitmap(IDB_BTN_NORMAL);break;
		}
		bitmapTrans.GetBitmap(&bmp);
		CBitmap *old=mem.SelectObject(&bitmapTrans);
		//向按钮所在位置传输位图
		//使用StretcnBlt的目的是为了让位图随按钮的大小而改变
		ButtonDC.StretchBlt(rc.left,rc.top,rc.right,rc.bottom,&mem,0,0,bmp.bmWidth,bmp.bmHeight,SRCCOPY);
		mem.SelectObject(old);
		bitmapTrans.DeleteObject();
		//设置文字背景为透明
		//ButtonDC.SetBkMode(TRANSPARENT);
		//ButtonDC.DrawText("已选中",&rc,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	}
	//设置文字背景为透明
	ButtonDC.SetBkMode(TRANSPARENT);
	if (strText != NULL)
	{
		CFont* hFont = GetFont();
		CFont* hOldFont = pDC->SelectObject(hFont);
		CSize szExtent = pDC->GetTextExtent(strText, lstrlen(strText));
		CPoint pt( rect.CenterPoint().x - szExtent.cx / 2, rect.CenterPoint().y - szExtent.cy / 2);
		if (state & ODS_SELECTED) 
			pt.Offset(1, 1);
		int nMode = pDC->SetBkMode(TRANSPARENT);
		if (state & ODS_DISABLED)
			pDC->DrawState(pt, szExtent, strText, DSS_DISABLED, TRUE, 0, (HBRUSH)NULL);
		else
			pDC->DrawState(pt, szExtent, strText, DSS_NORMAL, TRUE, 0, (HBRUSH)NULL);
		pDC->SelectObject(hOldFont);
		pDC->SetBkMode(nMode);
		//ButtonDC.DrawText("下一步",&rc,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	}
}

//绘制按钮的底色
void CXPButton::DoGradientFill(CDC *pDC, CRect* rect)
{
	CBrush brBk[64];
	int nWidth = rect->Width();	
	int nHeight = rect->Height();
	CRect rct;
	int i;
	for ( i = 0; i < 64; i ++)
	{
		if (m_bOver)
		{
			if (m_bFocus)
				brBk[i].CreateSolidBrush(RGB(255 - (i / 4), 255 - (i / 4), 255 - (i / 3)));
			else
				brBk[i].CreateSolidBrush(RGB(255 - (i / 4), 255 - (i / 4), 255 - (i / 5)));
		}
		else
		{
			if (m_bFocus)
				brBk[i].CreateSolidBrush(RGB(255 - (i / 3), 255 - (i / 3), 255 - (i / 4)));
			else
				brBk[i].CreateSolidBrush(RGB(255 - (i / 3), 255 - (i / 3), 255 - (i / 5)));
		}
	}
	
	for (i = rect->top; i <= nHeight + 2; i ++) 
	{
		rct.SetRect(rect->left, i, nWidth + 2, i + 1);
		pDC->FillRect(&rct, &brBk[((i * 63) / nHeight)]);
	}
	
	for (i = 0; i < 64; i ++)
		brBk[i].DeleteObject();
}


//绘制按钮的内边框
void CXPButton::DrawInsideBorder(CDC *pDC, CRect* rect)
{
	CPen *pLeft, *pRight, *pTop, *pBottom;
	
	if (m_bSelected && !m_bOver)
	{
		pLeft = & m_InsideBoundryPenLeftSel;
		pRight = &m_InsideBoundryPenRightSel;
		pTop = &m_InsideBoundryPenTopSel;
		pBottom = &m_InsideBoundryPenBottomSel;
	}
	else
	{
		pLeft = &m_InsideBoundryPenLeft;
		pRight = &m_InsideBoundryPenRight;
		pTop = &m_InsideBoundryPenTop;
		pBottom = &m_InsideBoundryPenBottom;
	}
	
	CPoint oldPoint = pDC->MoveTo(rect->left, rect->bottom - 1);
	CPen* pOldPen = pDC->SelectObject(pLeft);
	pDC->LineTo(rect->left, rect->top + 1);
	pDC->SelectObject(pRight);
	pDC->MoveTo(rect->right - 1, rect->bottom - 1);
	pDC->LineTo(rect->right - 1, rect->top);
	pDC->SelectObject(pTop);
	pDC->MoveTo(rect->left - 1, rect->top);
	pDC->LineTo(rect->right - 1, rect->top);
	pDC->SelectObject(pBottom);
	pDC->MoveTo(rect->left, rect->bottom);
	pDC->LineTo(rect->right - 1, rect->bottom);
	pDC->SelectObject(pOldPen);
	pDC->MoveTo(oldPoint);

	if (m_bSelected && !m_bOver)
		DrawFocusRect(pDC->m_hDC,rect);
	
}


