#pragma once

#include "IDesktopSelector.h"

class CDesktopFrame : public IDesktopFrame
{
public:
	CDesktopFrame( HBITMAP ahBitmap);
	CDesktopFrame( void* pImgData, BITMAPINFO &bmpInfo);
	virtual ~CDesktopFrame();

	// реализация IDesktopFrame
public:
	void			Release();
	IDesktopFrame*	Clone();
	unsigned int	GetWidth();
	unsigned int	GetHeight();
	HBITMAP			GetHBitmap();
	void*			GetData();
	unsigned int	GetDataSize();

private:
	HBITMAP CreateDIBSection( unsigned int auWidth, unsigned int auHeight, unsigned int auBPP, void** lppBits, BITMAPINFO &bmpInfo, bool abReverseHMode);

	CSize				bitmapSize;
	//! Captured Image
	HBITMAP				hCapturedImage;
	//
	void*				data;
	unsigned int		dataSize;
};