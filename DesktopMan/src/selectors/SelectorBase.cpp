#include "StdAfx.h"
#include "..\..\include\selectors\selectorbase.h"

CSelectorBase::CSelectorBase(void)
{
	m_hScreenDC = GetDC(0);
}

CSelectorBase::~CSelectorBase(void)
{
	ReleaseDC( 0, m_hScreenDC);
}

HBITMAP CSelectorBase::CreateDIBSection( unsigned int auWidth, unsigned int auHeight, unsigned int auBPP, void** lppBits, BITMAPINFO &bmpInfo, bool abReverseHMode)
{
	ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
	bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = auWidth;
	bmpInfo.bmiHeader.biHeight = abReverseHMode ? auHeight : -((int)auHeight);
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = auBPP;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	HBITMAP hbm = ::CreateDIBSection(m_hScreenDC, &bmpInfo
		, DIB_RGB_COLORS,(void **)lppBits, NULL, 0);

	return hbm;
}

void CSelectorBase::MakeWhiteBitmap(void* imgBits, int width, int height)
{
	if (!m_hScreenDC)
		return;
	memset(imgBits, 255, width*height*4);
}