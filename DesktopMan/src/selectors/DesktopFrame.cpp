#include "StdAfx.h"

#include "../../include/selectors/DesktopFrame.h"
#include "../../include/MemLeaks.h"

CDesktopFrame::CDesktopFrame( HBITMAP ahBitmap)
{
	hCapturedImage = ahBitmap;

	BITMAPINFO bmpInfo;
	ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
	bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);

	HDC hScreenDC = GetDC( 0);
	GetDIBits(hScreenDC,ahBitmap,0,0,NULL,&bmpInfo,DIB_RGB_COLORS); 
	

	bitmapSize = CSize( bmpInfo.bmiHeader.biWidth, bmpInfo.bmiHeader.biHeight);
	dataSize = bmpInfo.bmiHeader.biSizeImage;
	if(dataSize <= 0)
		dataSize=bmpInfo.bmiHeader.biWidth*abs(bmpInfo.bmiHeader.biHeight)*(bmpInfo.bmiHeader.biBitCount+7)/8;
	::ReleaseDC( 0, hScreenDC);
	data = NULL;
}

CDesktopFrame::CDesktopFrame( void* pImgData, BITMAPINFO &bmpInfo)
{
	data = MP_NEW_ARR( BYTE, bmpInfo.bmiHeader.biSizeImage);
	memcpy(data, pImgData, bmpInfo.bmiHeader.biSizeImage);
	dataSize = bmpInfo.bmiHeader.biSizeImage;
	bitmapSize = CSize( bmpInfo.bmiHeader.biWidth, bmpInfo.bmiHeader.biHeight);
}

CDesktopFrame::~CDesktopFrame()
{
	if( hCapturedImage != 0)
		DeleteObject( hCapturedImage);
	hCapturedImage = 0;
	SAFE_DELETE_ARRAY( data);
}

unsigned int	CDesktopFrame::GetWidth()
{
	return bitmapSize.cx;
}

unsigned int	CDesktopFrame::GetHeight()
{
	return bitmapSize.cy;
}

HBITMAP	CDesktopFrame::GetHBitmap()
{
	if (data == NULL)
		return NULL;

	void *pBits = NULL;
	BITMAPINFO info;
	HBITMAP hImage = CreateDIBSection(bitmapSize.cx, bitmapSize.cy, 32, &pBits, info, true);
	memcpy(pBits, data, dataSize);

	return hImage;
}

void*	CDesktopFrame::GetData()
{
	if( data)
		return data;

	BITMAPINFO bmpInfo;
	ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
	bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);

	HDC hScreenDC = GetDC( 0);
	GetDIBits(hScreenDC,hCapturedImage,0,0,NULL,&bmpInfo,DIB_RGB_COLORS); 

	if(bmpInfo.bmiHeader.biSizeImage<=0)
		bmpInfo.bmiHeader.biSizeImage=bmpInfo.bmiHeader.biWidth*abs(bmpInfo.bmiHeader.biHeight)*(bmpInfo.bmiHeader.biBitCount+7)/8;

	if( bmpInfo.bmiHeader.biSizeImage == 0)
		return 0;
	data = MP_NEW_ARR( BYTE, bmpInfo.bmiHeader.biSizeImage);
	if(data == NULL)
		return NULL;

	dataSize = bmpInfo.bmiHeader.biSizeImage;

	bmpInfo.bmiHeader.biCompression=BI_RGB;
	GetDIBits(hScreenDC,hCapturedImage,0,bmpInfo.bmiHeader.biHeight,data, &bmpInfo, DIB_RGB_COLORS);

	::ReleaseDC( 0, hScreenDC);

	return data;
}

unsigned int	CDesktopFrame::GetDataSize()
{
	return dataSize;
}

IDesktopFrame*	CDesktopFrame::Clone()
{
//	HDC hScreenDC = GetDC(0);
//	void *imgBits = NULL;
//	BITMAPINFO info;

// 	BITMAPINFO bmpInfo;
// 	ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
// 	bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
// 	GetDIBits(hScreenDC,hCapturedImage,0,0,NULL,&bmpInfo,DIB_RGB_COLORS); 
// 	bitmapSize = CSize( bmpInfo.bmiHeader.biWidth, bmpInfo.bmiHeader.biHeight);
/*
	HDC hDescDC = CreateCompatibleDC( hScreenDC);
	HBITMAP hDestImage = CreateDIBSection(bitmapSize.cx, bitmapSize.cy, 32, &pBits, info, true); //CreateCompatibleBitmap( hScreenDC, bitmapSize.cx, bitmapSize.cy);
	HBITMAP hOldDescBitmap = (HBITMAP)::SelectObject( hDescDC, (HBITMAP)hDestImage);

	HDC hSourceDC = CreateCompatibleDC( hScreenDC);
	HBITMAP hSourceDescBitmap = (HBITMAP)::SelectObject( hSourceDC, (HBITMAP)hCapturedImage);

	::BitBlt( hDescDC, 0, 0, bitmapSize.cx, bitmapSize.cy, hSourceDC, 0, 0, SRCCOPY);

	::SelectObject( hSourceDC, (HBITMAP)hSourceDescBitmap);
	::SelectObject( hDescDC, (HBITMAP)hOldDescBitmap);

	::DeleteDC( hSourceDC);
	::DeleteDC( hDescDC);

	::ReleaseDC( 0, hScreenDC);
*/
	BITMAPINFO bmpInfo;
	ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
	bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = bitmapSize.cx;
	bmpInfo.bmiHeader.biHeight = bitmapSize.cy;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	bmpInfo.bmiHeader.biSizeImage = bmpInfo.bmiHeader.biWidth*abs(bmpInfo.bmiHeader.biHeight)*bmpInfo.bmiHeader.biBitCount/8;

	MP_NEW_P2(df, CDesktopFrame, data, bmpInfo);
	return df;
}

HBITMAP CDesktopFrame::CreateDIBSection( unsigned int auWidth, unsigned int auHeight, unsigned int auBPP, void** lppBits, BITMAPINFO &bmpInfo, bool abReverseHMode)
{
	ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
	bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = auWidth;
	bmpInfo.bmiHeader.biHeight = abReverseHMode ? auHeight : -((int)auHeight);
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = auBPP;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	HDC hScreenDC = GetDC(0);
	HBITMAP hbm = ::CreateDIBSection(hScreenDC, &bmpInfo
		, DIB_RGB_COLORS,(void **)lppBits, NULL, 0);
	::ReleaseDC( 0, hScreenDC);

	return hbm;
}

void	CDesktopFrame::Release()
{
	MP_DELETE_THIS;
}