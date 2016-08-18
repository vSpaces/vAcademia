// GdiPlusInit.cpp: implementation of the CGdiPlusInit class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GdiPlusInit.h"
#include <GdiPlus.h>

#pragma comment(lib, "gdiplus.lib")

#ifndef _DEBUG
#pragma comment(lib, "delayimp.lib")
#pragma comment(linker, "/delayload:gdiplus.dll")
#endif

CGdiPlusInit::CGdiPlusInit()
{
    m_bPesent = true;
	Init();
}

void CGdiPlusInit::Init()
{
	Gdiplus::GdiplusStartupInput input;
	__try
	{
		Gdiplus::GdiplusStartup(&m_token, &input, 0);
	}
	__except(1)
	{
		m_bPesent = false;
	}
}

void CGdiPlusInit::Shutdown()
{
	if ( m_bPesent) Gdiplus::GdiplusShutdown( m_token);
}

CGdiPlusInit::~CGdiPlusInit()
{
    Shutdown();
}
