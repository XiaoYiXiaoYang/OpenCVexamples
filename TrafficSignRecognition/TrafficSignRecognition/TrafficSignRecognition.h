
// TrafficSignRecognition.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTrafficSignRecognitionApp: 
// �йش����ʵ�֣������ TrafficSignRecognition.cpp
//

class CTrafficSignRecognitionApp : public CWinApp
{
public:
	CTrafficSignRecognitionApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTrafficSignRecognitionApp theApp;