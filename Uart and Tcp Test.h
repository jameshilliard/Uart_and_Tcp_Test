
// Uart and Tcp Test.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CUartandTcpTestApp:
// �йش����ʵ�֣������ Uart and Tcp Test.cpp
//

class CUartandTcpTestApp : public CWinApp
{
public:
	CUartandTcpTestApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CUartandTcpTestApp theApp;