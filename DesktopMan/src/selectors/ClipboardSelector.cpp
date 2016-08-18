#include "StdAfx.h"
#include "../../include/selectors/DesktopFrame.h"
#include "../../include/IDesktopMan.h"
#include "../../include/selectors/ClipboardSelector.h"
#include <WindowsX.h>

CClipboardSelector::CClipboardSelector(oms::omsContext* apContext)
{
	context = apContext;
	currentFrame = NULL;
}

CClipboardSelector::~CClipboardSelector()
{
	ClearCapturedImage();
}

RECT CClipboardSelector::GetCurrentSelectingRect()
{
	if ( OpenClipboard(NULL) )
	{
		HBITMAP handle = (HBITMAP)GetClipboardData(CF_BITMAP);

		void* imgBits = NULL;
		BITMAPINFO bmpInfo;

		ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));

		bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);

		//дескриптор(описание) контекста устройства (DC) для рабочей области заданного окна
		HDC hScreenDC = GetDC(0);

		//извлекает биты заданного совместимого точечного рисунка и копирует их в буфер как  DIB, используя заданный формат.
		GetDIBits(hScreenDC,handle,0,0,NULL,&bmpInfo,DIB_RGB_COLORS); 

		if(bmpInfo.bmiHeader.biSizeImage<=0)
			bmpInfo.bmiHeader.biSizeImage=bmpInfo.bmiHeader.biWidth*abs(bmpInfo.bmiHeader.biHeight)*(bmpInfo.bmiHeader.biBitCount+7)/8;

		CloseClipboard();

		if( bmpInfo.bmiHeader.biSizeImage != 0)		
		{
			imgBits = MP_NEW_ARR( BYTE, bmpInfo.bmiHeader.biSizeImage);
			if(imgBits != NULL)
			{
				bmpInfo.bmiHeader.biCompression=BI_RGB;
				GetDIBits(hScreenDC,handle,0,bmpInfo.bmiHeader.biHeight,imgBits, &bmpInfo, DIB_RGB_COLORS);

				MP_NEW_V2( currentFrame, CDesktopFrame, imgBits, bmpInfo);

				CRect rect(0,0,bmpInfo.bmiHeader.biHeight,bmpInfo.bmiHeader.biWidth);

				return rect;
			}
		}
	}

	CRect rect(0,0,0,0);
	return rect;
}

// возвращает последний полученный кадр
IDesktopFrame* CClipboardSelector::GetCurrentFrame()
{
	return currentFrame;
}

void CClipboardSelector::ClearCapturedImage()
{
	if( currentFrame != NULL)
	{
		MP_DELETE( currentFrame);
		currentFrame = NULL;
	}
}