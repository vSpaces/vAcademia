#include "StdAfx.h"
#include "..\include\DesktopMan.h"
#include "..\include\view\DrawWindow.h"
#include "..\include\selectors\IDesktopSelector.h"
#include "..\include\selectors\DesktopRectSelector.h"
#include "..\include\selectors\ClipboardSelector.h"
#include "..\include\selectors\DropFileSelector.h"
#include <windef.h>
#include <algorithm>
#include "../include/view/RemoteDesktopWindow.h"
#include "../include/MemLeaks.h"
#include "../include/selectors/WndSelector.h"

BOOL SaveToFile(LPCSTR fn, HBITMAP hBitmap);

#define     SelectBitmap(hdc, hbm)  ((HBITMAP)SelectObject((hdc), (HGDIOBJ)(HBITMAP)(hbm)))

#define		MODE_COUNT		3
#define		DRAG_MODE		2
#define		CLIPBOARD_MODE	1
#define		DESKTOP_MODE	0

CDesktopManager::CDesktopManager(oms::omsContext* aContext) : //trayController( this), 
	MP_STRING_INIT(sTarget),
	MP_VECTOR_INIT(vecCallbacks)
{
	context = aContext;
	pDrawWindow = NULL;
	fProportion = 1.33f;
//	trayController.Start();
	currentSelector = NULL;
	desktopRectSelector = NULL;
	clipboardRectSelector = NULL;
	dropFileSelector = NULL;
	isBuffer = false;
	currentMode = 0;
}

CDesktopManager::~CDesktopManager()
{
//	trayController.HideTrayIcon();

	if( desktopRectSelector != NULL)	
		MP_DELETE( desktopRectSelector);

	//((ViskoeThread::CThreadImpl<CTrayController>&)trayController).Detach();

	SAFE_RELEASE( pDrawWindow);
}

void CDesktopManager::Release()
{
	/*trayController.HideTrayIcon();
	ClearCapturedImage();
	SendMessage( trayController.m_hWnd, WM_DESTROY, 0, 0);*/
	MP_DELETE_THIS;
}

void CDesktopManager::SetScreenCaptureObjectTarget( LPCSTR lpcTarget, float proportion)
{
	if( !lpcTarget)
		return;
	fProportion = proportion;
	sTarget = lpcTarget;
	currentMode = DESKTOP_MODE;
	StartDefineRegion(); 
//	trayController.ShowTrayIcon();
}

void CDesktopManager::SetScreenCaptureObjectTarget( LPCSTR lpcTarget,int mode, float proportion, LPCWSTR filePath)
{
	if( !lpcTarget)
		return;

	fProportion = proportion;
	sTarget = lpcTarget;

	isBuffer = mode== CLIPBOARD_MODE?true:false;

	currentMode = mode % MODE_COUNT;
	// создаем нужный селектор
	if (mode % MODE_COUNT == DESKTOP_MODE)
	{
		currentMode = mode;
		StartDefineRegion();
		return;
	}
	if (mode % MODE_COUNT == CLIPBOARD_MODE)
	{
		if( clipboardRectSelector == NULL)
		{
			MP_NEW_V( clipboardRectSelector, CClipboardSelector, context);
			clipboardRectSelector->Subscribe( this);
		}

		currentSelector = clipboardRectSelector;
	}
	else if ((mode % MODE_COUNT == DRAG_MODE) && (filePath!=NULL))
	{		
		if( dropFileSelector == NULL)
		{
			MP_NEW_V( dropFileSelector, CDropFileSelector, context);
			dropFileSelector->Subscribe( this);
		} 

		if (dropFileSelector != NULL)
		{
			std::wstring path = filePath;
			dropFileSelector->SetFilePath(path);
		}

		currentSelector = dropFileSelector;
	}

	ShowPreviewWindow(mode);
}


//! Добавляет обработчик нотификационных событий
bool CDesktopManager::AddCallback( desktopIDesktopManCallbacks* apCallback)
{
	VecCallbacks::iterator it = std::find( vecCallbacks.begin(), vecCallbacks.end(), apCallback);
	if( it == vecCallbacks.end())
		vecCallbacks.push_back( apCallback);
	return true;
}

bool CDesktopManager::Update()
{
	return true;
}

//! Удаляет обработчик нотификационных событий
bool CDesktopManager::RemoveCallback( desktopIDesktopManCallbacks* apCallback)
{
	VecCallbacks::iterator it = find( vecCallbacks.begin(), vecCallbacks.end(), apCallback);
	if( it != vecCallbacks.end())
	{
		vecCallbacks.erase( it);
		return true;
	}
	return false;
}

DWORD get_2power(DWORD val, BOOL bUpper)
{
	if( val == 0)	
		return 0;
	DWORD	stVal = 0x80000000;
	while( !(val&stVal))
		stVal >>= 1;

	if( val&(~stVal))
		return bUpper?(stVal<<1):stVal;
	else
		return bUpper?stVal:(stVal>>1);
}

void	CDesktopManager::ShowPreviewWindow(int aMode)
{
	if( !currentSelector)
		return;

	RECT rcCapture = currentSelector->GetCurrentSelectingRect();
	if( rcCapture.right<=rcCapture.left)
		return;
	if( rcCapture.bottom<=rcCapture.top)
		return;

	if( !pDrawWindow)
	{
		CDialogTemplate::SetResourceInstance( (HINSTANCE)_Module.m_hInstResource);
		MP_NEW_V2( pDrawWindow, CDrawWindow, context, false);
		pDrawWindow->Subscribe(this);
	}
	if (aMode >= MODE_COUNT)
	{
		pDrawWindow->SetLayout(TEXTURE_PREVIEW_MODE);
	}
	else
	{
		pDrawWindow->SetLayout(NORMAL_PREVIEW_MODE);
	}

	pDrawWindow->ShowPreview( currentSelector);
	
}

void	CDesktopManager::StartDefineRegion()
{
	CancelSelection();

	// создаем нужный селектор
	if( desktopRectSelector == NULL)
	{
		MP_NEW_V( desktopRectSelector, CDesktopRectSelector, context );
		desktopRectSelector->Subscribe( this);
	}
	currentSelector = desktopRectSelector;

	ATLASSERT( currentSelector);
	currentSelector->StartSourceSelection();
}

void	CDesktopManager::CancelSelection()
{
	if( currentSelector != NULL)
		currentSelector->CancelSourceSelection();
}

void	CDesktopManager::RecapSelection()
{
	if( currentSelector != NULL)
		currentSelector->RecapFrame();

	ShowPreviewWindow(currentMode);
}

void	CDesktopManager::OnPreviewWindowCancel()
{
}

void	CDesktopManager::OnPreviewWindowOK(int aScaleMode)
{
	if( !currentSelector)
		return;

	IDesktopFrame* pIDesktopFrame = currentSelector->GetCurrentFrame();
	if( pIDesktopFrame == NULL)
		return;

	HBITMAP hCapturedImage = pIDesktopFrame->GetHBitmap();
	if( hCapturedImage == 0)
		return;


	HDC hScreenDC = GetDC( 0);
	if( hScreenDC)
	{
		HDC hBackHDC = CreateCompatibleDC( hScreenDC);
		if( hBackHDC)
		{
			HBITMAP oldBitmap = SelectBitmap( hBackHDC, hCapturedImage);

			//! Тут надо создать новую картинку с шириной и высотой кратной степени двойки
			int imageWidth = pIDesktopFrame->GetWidth();
			int imageHeight = pIDesktopFrame->GetHeight();
			int imageProportedWidth = imageWidth;
			int imageProportedHeight = imageHeight;

			if (fProportion != -1)
			{
				if( fProportion > 1.0f)
					imageProportedWidth = (int)((float)imageWidth / fProportion);
				else if (fProportion > 0)
					imageProportedHeight = (int)((float)imageHeight * fProportion);
			}

			int imageP2Width = get_2power( imageProportedWidth, TRUE);
			if( imageP2Width > MAX_TEXTURE_SIZE)
				imageP2Width = MAX_TEXTURE_SIZE;
			int imageP2Height = get_2power( imageProportedHeight, TRUE);
			if( imageP2Height > MAX_TEXTURE_SIZE)
				imageP2Height = MAX_TEXTURE_SIZE;
			int textureSize = max(imageP2Width, imageP2Height);

			float widthK = (float)textureSize / imageProportedWidth;
			float heightK = (float)textureSize / imageProportedHeight;

			float k = min(widthK, heightK);

			HDC hTexturedImageHDC = CreateCompatibleDC( hScreenDC);
			if( hTexturedImageHDC)
			{
				HBITMAP hTexturedImage = CreateCompatibleBitmap( hScreenDC
									, textureSize
									, textureSize);

				if( hTexturedImage)
				{
					HBITMAP oldTexturedBitmap = SelectBitmap( hTexturedImageHDC, hTexturedImage);

					RECT rcTexture = {0, 0, textureSize, textureSize};
					::FillRect( hTexturedImageHDC, &rcTexture, (HBRUSH)GetStockObject( WHITE_BRUSH));

					int strechedImageWidth;
					int strechedImageHeight;

					if ((fProportion == 0) || (fProportion == -1))
					{
						strechedImageWidth = textureSize;
						strechedImageHeight = textureSize;
					}
					else
					{
						strechedImageWidth = (int)(imageProportedWidth * k);
						strechedImageHeight = (int)(imageProportedHeight * k);
					}


					if (aScaleMode == SCALE_MODE_FIT) {
						SetStretchBltMode( hTexturedImageHDC, HALFTONE);
						SetStretchBltMode( hBackHDC, HALFTONE);
						::StretchBlt(hTexturedImageHDC
							, (textureSize - strechedImageWidth)/2
							, (textureSize - strechedImageHeight)/2
							, strechedImageWidth
							, strechedImageHeight
							, hBackHDC
							, 0
							, 0
							, imageWidth
							, imageHeight
							, SRCCOPY);
					}
					else {
						SetStretchBltMode( hTexturedImageHDC, HALFTONE);
						SetStretchBltMode( hBackHDC, HALFTONE);
						::StretchBlt(hTexturedImageHDC
							, 0
							, 0
							, textureSize
							, textureSize
							, hBackHDC
							, 0
							, 0
							, imageWidth
							, imageHeight
							, SRCCOPY);
						
					}

					BITMAPINFO bmpInfo;
					ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
					bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
					GetDIBits(hTexturedImageHDC,hTexturedImage,0,0,NULL,&bmpInfo,DIB_RGB_COLORS); 
					
					if(bmpInfo.bmiHeader.biSizeImage<=0)
						bmpInfo.bmiHeader.biSizeImage=bmpInfo.bmiHeader.biWidth*abs(bmpInfo.bmiHeader.biHeight)*(bmpInfo.bmiHeader.biBitCount+7)/8;

					LPVOID bits = NULL;
					if( bmpInfo.bmiHeader.biSizeImage > 0 )
					{
						bits = MP_NEW_ARR( BYTE, bmpInfo.bmiHeader.biSizeImage);
						if(bits != NULL)
						{
							bmpInfo.bmiHeader.biCompression=BI_RGB;
							GetDIBits( hTexturedImageHDC, hTexturedImage, 0, bmpInfo.bmiHeader.biHeight, bits, &bmpInfo, DIB_RGB_COLORS);

							byte* btStart = (byte*)bits;
							byte bpp = bmpInfo.bmiHeader.biBitCount/8;
							byte temp = 0;
							for (unsigned int i=0; i<bmpInfo.bmiHeader.biSizeImage; i+=bpp, btStart+=bpp)
							{
								temp = btStart[0];
								btStart[0] = btStart[2];
								btStart[2] = temp;
							}

							VecCallbacks::iterator it = vecCallbacks.begin(), end = vecCallbacks.end();
							for (; it!=end; ++it)
							{
								(*it)->onApplyFrameRequest( sTarget.c_str(), bpp, bmpInfo.bmiHeader.biWidth, bmpInfo.bmiHeader.biHeight, bits, pDrawWindow->GetQuality());
							}

							SAFE_DELETE_ARRAY(bits);
						}
					}

					SelectBitmap( hTexturedImageHDC, oldTexturedBitmap);
					DeleteObject( hTexturedImage);
				}
				DeleteDC( hTexturedImageHDC);
			}

			SelectBitmap( hBackHDC, oldBitmap);
			DeleteDC( hBackHDC);
		}
		::ReleaseDC( 0, hScreenDC);
	}
}

void CDesktopManager::OnSelectionCompleted(int resultCode)
{
	ShowPreviewWindow(currentMode);
}

void CDesktopManager::OnSelectionEscape()
{
}