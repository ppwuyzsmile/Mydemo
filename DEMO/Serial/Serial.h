
// Serial.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSerialApp:
// �йش����ʵ�֣������ Serial.cpp
//

class CSerialApp : public CWinApp
{
public:
	CSerialApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
public:
	unsigned int m_pwd;
	//--------------add for gdiplus library initail----------------------------
	ULONG_PTR	m_gdiplusToken;
	//--------------add for gdiplus library initail----------------------------
	virtual int ExitInstance();
};

extern CSerialApp theApp;