#include "StdAfx.h"
#include "SplashImage.h"
#include "mdump/dump.h"
#include "resource.h"
#include "GdiPlusInit.h"
#include "AlphaRegion.h"

#define DELETE_GDI_OBJECT( p_gdi_obj) { if ( p_gdi_obj) { DeleteObject( p_gdi_obj);  p_gdi_obj = NULL;} }

using namespace Gdiplus;

CSplashImage::CSplashImage()
{
	m_animatedDotIndex = 1;
	m_animatedDotsCount = 22;
	m_animatedDotsDirectionRight = true;		
	m_iLanguage = ENG;
	m_iCadr = 0;
	m_iLoadCadr = 0;
	m_startTime = GetTickCount();
	m_currTime = m_startTime;
	m_loadCadrAlpha = 1.0f;
	m_nextLoadCadrAlpha = 0.0f;
	m_cadrAlpha = 1.0f;
	m_nextCadrAlpha = 0.0f;
	m_hBitmapSplash = NULL;
	m_hOldTextBmp = NULL;
	m_hOldFont = NULL;	
	m_hTextDC = NULL;
	m_hTextBitmapDC = NULL;
	m_hTextBmp = NULL;
	m_hFont = NULL;
	m_fontBold = NULL;
	m_font = NULL;
	m_stringFormat = NULL;
	m_textBrush = NULL;
	m_textInfoBrush = NULL;
	m_alphaRegion = NULL;
	m_gdiPlusInit = NULL;	
	m_bLoaded = FALSE;
}

CSplashImage::~CSplashImage()
{
	UnLoad();
}

//////////////////////////////////////////////////////////////////////////
// Loads the PNG containing the splash image into a HBITMAP.
HBITMAP CSplashImage::LoadSplashImage( unsigned int auResourceID)
{
	ULONG gdiPlusHandle;
	
	HBITMAP hBitmapSplash = NULL;	

	// initialize return value
	IStream * ipStream = NULL;

	LPCTSTR lpName = MAKEINTRESOURCE(auResourceID);
	LPCTSTR lpType = _T("PNG");

	// find the resource
	HRSRC hrsrc = FindResource(NULL, lpName, lpType);
	if (hrsrc == NULL)
		return NULL;

	// load the resource
	DWORD dwResourceSize = SizeofResource(NULL, hrsrc);
	HGLOBAL hglbImage = LoadResource(NULL, hrsrc);
	if (hglbImage == NULL)
	{
		DeleteObject( hrsrc);
		return NULL;
	}

	// lock the resource, getting a pointer to its data
	LPVOID pvSourceResourceData = LockResource(hglbImage);
	if (pvSourceResourceData == NULL)
	{
		FreeResource(hglbImage);
		DeleteObject( hrsrc);
		return NULL;
	}

	// allocate memory to hold the resource data
	HGLOBAL hgblResourceData = GlobalAlloc(GMEM_MOVEABLE, dwResourceSize);
	if (hgblResourceData == NULL)
	{
		FreeResource(hglbImage);
		DeleteObject( hrsrc);
		return NULL;
	}

	// get a pointer to the allocated memory
	LPVOID pvResourceData = GlobalLock(hgblResourceData);
	if (pvResourceData == NULL)
	{		
		GlobalFree(hgblResourceData);
		FreeResource(hglbImage);
		DeleteObject( hrsrc);
		return NULL;
	}

	// copy the data from the resource to the new memory block
	CopyMemory(pvResourceData, pvSourceResourceData, dwResourceSize);
	GlobalUnlock(hgblResourceData);

	// create a stream on the HGLOBAL containing the data
	if (FAILED(CreateStreamOnHGlobal(hgblResourceData, TRUE, &ipStream)))
	{
		if ( hgblResourceData)
			GlobalFree(hgblResourceData);
		if ( hglbImage)
			FreeResource(hglbImage);
		if ( hrsrc)
			DeleteObject( hrsrc);
		return NULL;
	}

	// no need to unlock or free the resource	
	IStream * ipImageStream = ipStream;

	if (ipImageStream == NULL)
	{
		if ( hgblResourceData)
			GlobalFree(hgblResourceData);
		if ( hglbImage)
			FreeResource(hglbImage);
		if ( hrsrc)
			DeleteObject( hrsrc);
		return NULL;
	}

	GpBitmap*	bitmap = NULL;
	Gdiplus::DllExports::GdipCreateBitmapFromStream( ipImageStream, &bitmap);
	if( !bitmap)
	{
		ipImageStream->Release();
		FreeResource(hglbImage);
		GlobalFree(hgblResourceData);
		return NULL;
	}
	
	GpStatus result = Gdiplus::DllExports::GdipCreateHBITMAPFromBitmap( bitmap, &hBitmapSplash, RGB(0,0,0));
		
	DeleteObject( bitmap);
	ipImageStream->Release();	
	
	GlobalFree(hgblResourceData);
	FreeResource(hglbImage);	
	DeleteObject( hrsrc);

	return hBitmapSplash;
}

// Loads the PNG containing the splash image into a HBITMAP.
HBITMAP CSplashImage::LoadSplashImage( LPCTSTR lpName)
{	
	WCHAR res[ 2 * MAX_PATH];
	wcscpy( res, m_sRootPath);
	wcscat( res, L"\\");
	USES_CONVERSION;
	wcscat( res, A2W(lpName));
		
	Bitmap *bm = new Bitmap(res); 
	HBITMAP hbm;
	Color color = Color( RGB(0,0,0));
	bm->GetHBITMAP( color, &hbm);
	delete bm;
	return hbm;
}


//////////////////////////////////////////////////////////////////////////

void CSplashImage::SetRootPath( WCHAR *sRootPath)
{
	wcscpy( m_sRootPath, sRootPath);
}

void CSplashImage::Load()
{
	// инициализация GDI+
	m_gdiPlusInit = new CGdiPlusInit();
	m_alphaRegion = new CAlphaRegion();
	m_hBitmapSplash = LoadSplashImage(IDB_SPLASH);	
	if ( m_hBitmapSplash == NULL)
		return;
	
	m_alphaRegion->Create( m_hBitmapSplash);

	m_hBitmapsDot[ 0] = LoadSplashImage(IDB_SPLASH_DOT1);
	m_hBitmapsDot[ 1] = LoadSplashImage(IDB_SPLASH_DOT2);
	m_hBitmapsDot[ 2] = LoadSplashImage(IDB_SPLASH_DOT3);
	
	m_infoCadr[ 0].hBitmap = LoadSplashImage(IDB_CADR1);
	m_infoCadr[ 1].hBitmap = LoadSplashImage(IDB_CADR2);
	m_infoCadr[ 2].hBitmap = LoadSplashImage(IDB_CADR3);

	if ( m_iLanguage == RUS)
	{		
		m_infoCadr[ 0].hTextBitmap = LoadSplashImage(IDB_TEXT1_R);
		m_infoCadr[ 1].hTextBitmap = LoadSplashImage(IDB_TEXT2_R);
		m_infoCadr[ 2].hTextBitmap = LoadSplashImage(IDB_TEXT3_R);

		m_infoCadr[ 0].hLoadTextBitmap = LoadSplashImage(IDB_T_LIBRARIES_R);
		m_infoCadr[ 1].hLoadTextBitmap = LoadSplashImage(IDB_T_SCRIPTS_R);
		m_infoCadr[ 2].hLoadTextBitmap = LoadSplashImage(IDB_T_INTERF_R);
	}
	else if ( m_iLanguage == ENG)
	{
		m_infoCadr[ 0].hTextBitmap = LoadSplashImage(IDB_TEXT1);
		m_infoCadr[ 1].hTextBitmap = LoadSplashImage(IDB_TEXT2);
		m_infoCadr[ 2].hTextBitmap = LoadSplashImage(IDB_TEXT3);

		m_infoCadr[ 0].hLoadTextBitmap = LoadSplashImage(IDB_T_LIBRARIES);
		m_infoCadr[ 1].hLoadTextBitmap = LoadSplashImage(IDB_T_SCRIPTS);
		m_infoCadr[ 2].hLoadTextBitmap = LoadSplashImage(IDB_T_INTERF);
	}
	else if ( m_iLanguage == KZH)
	{
		m_infoCadr[ 0].hTextBitmap = NULL;
		m_infoCadr[ 1].hTextBitmap = NULL;
		m_infoCadr[ 2].hTextBitmap = NULL;

		m_infoCadr[ 0].hLoadTextBitmap = NULL;
		m_infoCadr[ 1].hLoadTextBitmap = NULL;
		m_infoCadr[ 2].hLoadTextBitmap = NULL;
	}
	m_bLoaded = TRUE;
}

void CSplashImage::UnLoad()
{	
	SAFE_DELETE( m_stringFormat);
	SAFE_DELETE( m_fontBold);
	SAFE_DELETE( m_font);
	SAFE_DELETE( m_textBrush);
	SAFE_DELETE( m_textInfoBrush);

	if (m_infoCadr[ 2].hLoadTextBitmap != NULL)
	{
		DELETE_GDI_OBJECT( m_infoCadr[ 2].hLoadTextBitmap);
	}
	if (m_infoCadr[ 1].hLoadTextBitmap != NULL)
	{
		DELETE_GDI_OBJECT( m_infoCadr[ 1].hLoadTextBitmap);
	}
	if (m_infoCadr[ 0].hLoadTextBitmap != NULL)
	{
		DELETE_GDI_OBJECT( m_infoCadr[ 0].hLoadTextBitmap);
	}
	if (m_infoCadr[ 2].hTextBitmap != NULL)
	{
		DELETE_GDI_OBJECT( m_infoCadr[ 2].hTextBitmap);
	}
	if (m_infoCadr[ 1].hTextBitmap != NULL)
	{
		DELETE_GDI_OBJECT( m_infoCadr[ 1].hTextBitmap);
	}
	if (m_infoCadr[ 0].hTextBitmap != NULL)
	{
		DELETE_GDI_OBJECT( m_infoCadr[ 0].hTextBitmap);
	}
	DELETE_GDI_OBJECT( m_infoCadr[ 2].hBitmap);
	DELETE_GDI_OBJECT( m_infoCadr[ 1].hBitmap);
	DELETE_GDI_OBJECT( m_infoCadr[ 0].hBitmap);
	DELETE_GDI_OBJECT( m_hBitmapsDot[ 2]);
	DELETE_GDI_OBJECT( m_hBitmapsDot[ 1]);
	DELETE_GDI_OBJECT( m_hBitmapsDot[ 0]);
	DELETE_GDI_OBJECT( m_hBitmapSplash);

	SAFE_DELETE( m_alphaRegion);
	SAFE_DELETE( m_gdiPlusInit);
	m_bLoaded = FALSE;
}

BOOL CSplashImage::IsLoaded()
{
	return m_bLoaded;
}

HRGN CSplashImage::GetRegion()
{
	return m_alphaRegion->GetRegion();
}

void CSplashImage::SetSplashImage(HWND hwndSplash, float alpha, int aLoadingPercent)
{
	if ( !m_bLoaded)
		return;

	m_loadingPercent = aLoadingPercent;	
	m_currTime = GetTickCount();
	if ( m_currTime - m_startTime > 5000)
	{
		m_cadrAlpha -= 0.05f;	
		m_nextCadrAlpha += 0.05f;
	}

	if ( m_nextCadrAlpha >= 1.0f)
	{
		m_cadrAlpha = 1.0f;
		m_iCadr++;
		m_nextCadrAlpha = 0.0f;
		if ( m_iCadr == 3)
			m_iCadr = 0;
		m_startTime = m_currTime;
	}	

	if ( m_loadingPercent > 65 && m_iLoadCadr == 1)
	{
		m_loadCadrAlpha = (75 - m_loadingPercent) / 10.0f;
		m_nextLoadCadrAlpha = (m_loadingPercent - 65) / 10.0f;
		if ( m_nextLoadCadrAlpha > 0.95f)
		{
			m_loadCadrAlpha = 1.0f;
			m_nextLoadCadrAlpha = 0.0f;
			m_iLoadCadr++;			
		}
	}
	else if ( m_loadingPercent > 35 && m_iLoadCadr == 0)
	{
		m_loadCadrAlpha = (45 - m_loadingPercent) / 10.0f;
		m_nextLoadCadrAlpha = (m_loadingPercent - 25) / 10.0f;
		if ( m_nextLoadCadrAlpha > 0.95f)
		{
			m_loadCadrAlpha = 1.0f;
			m_nextLoadCadrAlpha = 0.0f;
			m_iLoadCadr++;			
		}
	}

	// get the size of the bitmap
	BITMAP bm;
	GetObject(m_hBitmapSplash, sizeof(bm), &bm);
	m_sizeSplash.cx = bm.bmWidth;	
	m_sizeSplash.cy = bm.bmHeight;
	if (m_iLanguage == KZH)
	{
		m_sizeSplash.cy = m_sizeSplash.cy * 0.65;
	}

	// get the primary monitor's info
	POINT ptZero = { 0 };
	HMONITOR hmonPrimary = MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);
	MONITORINFO monitorinfo = { 0 };
	monitorinfo.cbSize = sizeof(monitorinfo);
	GetMonitorInfo(hmonPrimary, &monitorinfo);
	DeleteObject(hmonPrimary);

	// center the splash screen in the middle of the primary work area
	const RECT & rcWork = monitorinfo.rcWork;
	POINT ptOrigin;
	ptOrigin.x = rcWork.left + (rcWork.right - rcWork.left - m_sizeSplash.cx) / 2;
	ptOrigin.y = rcWork.top + (rcWork.bottom - rcWork.top - m_sizeSplash.cy) / 2;

	// create a memory DC holding the splash bitmap
	m_hdcScreen = GetDC(NULL);

	m_hdcMem = CreateCompatibleDC(m_hdcScreen);

	RECT rec = {0, 0, m_sizeSplash.cx, m_sizeSplash.cy};

	m_hDrawBitmap = CreateCompatibleBitmap( m_hdcScreen, m_sizeSplash.cx, m_sizeSplash.cy);
	m_hbmpOld = (HBITMAP) SelectObject(m_hdcMem, m_hDrawBitmap);

	// Copy bitmap to drawing DC
	m_hdcBitmap = CreateCompatibleDC(m_hdcScreen);
	m_hbmpBitMapOld = (HBITMAP) SelectObject(m_hdcBitmap, m_hBitmapSplash);
	::BitBlt( m_hdcMem, 0, 0, m_sizeSplash.cx, m_sizeSplash.cy, m_hdcBitmap, 0, 0, SRCCOPY);

	Bitmap backBuffer(m_sizeSplash.cx, m_sizeSplash.cy);
	Graphics temp( &backBuffer);
	m_pGraphicTemp = &temp;
		
	int mode = SetBkMode(m_hdcMem, OPAQUE);
	
	AnimatedDots();	

	m_hTextBitmapDC = CreateCompatibleDC(m_hdcScreen);
	//m_hTextDC = CreateCompatibleDC(m_hdcScreen);
	
	ShowTextInfoCadr( m_iCadr, m_iLoadCadr, m_cadrAlpha, m_loadCadrAlpha);

	if ( m_nextCadrAlpha > 0.0f || m_nextLoadCadrAlpha > 0.0f)
		ShowTextInfoCadr( (m_iCadr + 1) % 3, (m_iLoadCadr + 1) % 3, m_nextCadrAlpha, m_nextLoadCadrAlpha);

	HBITMAP bitmap;
	Color color_ = Color( 0,0,0,0);	
	backBuffer.GetHBITMAP( color_, &bitmap);
	m_hOldTextBmp = (HBITMAP) SelectObject(m_hTextBitmapDC, bitmap);	
	
	BOOL b;	

	// use the source image's alpha channel for blending
	BLENDFUNCTION blend = { 0 };
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = (BYTE) ( 255 * alpha);
	blend.AlphaFormat = AC_SRC_ALPHA;

	b = AlphaBlend( m_hdcMem, 0, 0, m_sizeSplash.cx, m_sizeSplash.cy, m_hTextBitmapDC, 0, 0, m_sizeSplash.cx, m_sizeSplash.cy - 0, blend);	

	ShowImgInfoCadr( m_iCadr, m_cadrAlpha);

	if ( m_nextCadrAlpha > 0.0f)
		ShowImgInfoCadr( (m_iCadr + 1) % 3, m_nextCadrAlpha);

	// paint the window (in the right location) with the alpha-blended bitmap
	b = UpdateLayeredWindow(hwndSplash, m_hdcScreen, &ptOrigin, &m_sizeSplash,
						m_hdcMem, &ptZero, RGB(255, 0, 0), &blend, ULW_ALPHA);
	
	DeleteObject( bitmap);
	
	SetBkMode(m_hdcMem, mode);
	// delete temporary objects	
	if ( m_hTextDC != NULL && m_hOldFont != NULL)
		SelectObject(m_hTextDC,m_hOldFont);	

	DELETE_GDI_OBJECT(m_hFont);		

	if ( m_hOldTextBmp != NULL)
		SelectObject(m_hTextBitmapDC,m_hOldTextBmp);

	DELETE_GDI_OBJECT(m_hTextBmp);	
	DeleteDC(m_hTextBitmapDC);
	if ( m_hTextDC != NULL)
		DeleteDC(m_hTextDC);	

	SelectObject(m_hdcBitmap, m_hbmpBitMapOld);
	DeleteDC(m_hdcBitmap);
	SelectObject(m_hdcMem, m_hbmpOld);	
	DELETE_GDI_OBJECT(m_hDrawBitmap);
	DeleteDC(m_hdcMem);
	
	ReleaseDC(NULL, m_hdcScreen);
	m_hOldTextBmp = NULL;
	m_hOldFont = NULL;		
}

void CSplashImage::AnimatedDots()
{
	// TODO: Add any drawing code here...
	unsigned int startX = 18;
	unsigned int startY = 117;//81;
	unsigned int dotSizes[] = {5,7,8};
	unsigned int dotsDistance = 10;

	for (unsigned int i = 0; i < m_animatedDotsCount; i++)
	{
		unsigned int sizeIndex = 0;
		if( i == m_animatedDotIndex)
			sizeIndex = 2;
		else
		{
#ifdef ANIMATED_ONEDIRECTION
			if( i == m_animatedDotIndex - 1 || (i == (m_animatedDotsCount-1) && m_animatedDotIndex == 0))
				sizeIndex = 1;
#else
			if( m_animatedDotsDirectionRight)
			{
				if( i == m_animatedDotIndex - 1)
					sizeIndex = 1;
			}
			else
			{
				if( i == m_animatedDotIndex + 1)
					sizeIndex = 1;
			}
#endif
		}

		unsigned int dotSize = dotSizes[sizeIndex];
		unsigned int dotHalfSize = (dotSize >> 1);

		unsigned int dotLeft = startX+dotsDistance*i-dotHalfSize;
		unsigned int dotTop = startY-dotSize;

		SelectObject(m_hdcBitmap, m_hBitmapsDot[sizeIndex]);
		::BitBlt( m_hdcMem, dotLeft, dotTop, dotSize, dotSize, m_hdcBitmap, 0, 0, SRCCOPY);
	}
}

void CSplashImage::ShowImgInfoCadr( int aiCadr, float alpha)
{
	if ( alpha < 0.1f)
		return;
	ATLASSERT( aiCadr >= 0 && aiCadr < 3);
	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = BYTE (255 * alpha);  // half of 0xff = 50% transparency 
	bf.AlphaFormat = AC_SRC_ALPHA; 	

	BITMAP bm;
	GetObject(m_infoCadr[ aiCadr].hBitmap, sizeof(bm), &bm);
	SIZE sizeSplashCadr;
	sizeSplashCadr.cx = bm.bmWidth;
	sizeSplashCadr.cy = bm.bmHeight;
	SelectObject(m_hdcBitmap, m_infoCadr[ aiCadr].hBitmap);
	
	AlphaBlend( m_hdcMem, 250, 11, sizeSplashCadr.cx, sizeSplashCadr.cy, m_hdcBitmap, 0, 0, sizeSplashCadr.cx, sizeSplashCadr.cy, bf);
}

void CSplashImage::ShowTextInfoCadr( int aiCadr, int aiLoadCadr, float alpha, float loadAlpha)
{
	USES_CONVERSION;
	if ( m_fontBold == NULL)
	{
		HDC hdc = m_pGraphicTemp->GetHDC();
		LOGFONTA dfontBold = {14, 6, 0, 0,
			FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, "Tahoma"};	
		m_fontBold = new Font( hdc, &dfontBold);		
		m_pGraphicTemp->ReleaseHDC( hdc);
	}	
	
	if ( m_stringFormat == NULL)
		m_stringFormat = new StringFormat();

	if ( loadAlpha > 0.05f)
	{
		int ialpha = (int) 255 * loadAlpha;		
		TCHAR sPercent[ MAX_PATH];
		itoa( m_loadingPercent, sPercent, 10);
		strcat( sPercent, "%");	

		if ( m_textBrush != NULL)
		{
			delete m_textBrush;
			m_textBrush = NULL;
		}
		m_textBrush = new SolidBrush(Color(255, 254, 255, 157));
		
		const wchar_t * ws = A2W( sPercent);
		m_pGraphicTemp->DrawString( ws, -1, m_fontBold, PointF(15.0f, 82.0f), m_textBrush);

		if (m_infoCadr[ aiLoadCadr].hLoadTextBitmap != NULL)
		{
			BITMAP bm;
			GetObject(m_infoCadr[ aiLoadCadr].hLoadTextBitmap, sizeof(bm), &bm);
			SIZE sizeSplashCadr;
			sizeSplashCadr.cx = bm.bmWidth;
			sizeSplashCadr.cy = bm.bmHeight;

			SelectObject(m_hdcBitmap, m_infoCadr[ aiLoadCadr].hLoadTextBitmap);
			BLENDFUNCTION bf;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			bf.SourceConstantAlpha = ialpha;
			bf.AlphaFormat = AC_SRC_ALPHA; 			
			AlphaBlend( m_hdcMem, 68, 84, sizeSplashCadr.cx, sizeSplashCadr.cy, m_hdcBitmap, 0, 0, sizeSplashCadr.cx, sizeSplashCadr.cy, bf);
		}
	}

	if ( alpha > 0.05f)
	{
		int ialpha = (int) 255 * alpha;

		if (m_infoCadr[ aiCadr].hTextBitmap != NULL)
		{
			BITMAP bm;
			GetObject(m_infoCadr[ aiCadr].hTextBitmap, sizeof(bm), &bm);
			SIZE sizeSplashCadr;
			sizeSplashCadr.cx = bm.bmWidth;
			sizeSplashCadr.cy = bm.bmHeight;

			SelectObject(m_hdcBitmap, m_infoCadr[ aiCadr].hTextBitmap);
			BLENDFUNCTION bf;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			bf.SourceConstantAlpha = ialpha;
			bf.AlphaFormat = AC_SRC_ALPHA;

			AlphaBlend( m_hdcMem, 15, 132, sizeSplashCadr.cx, sizeSplashCadr.cy, m_hdcBitmap, 0, 0, sizeSplashCadr.cx, sizeSplashCadr.cy, bf);
		}
	}
}


std::vector<std::string> CSplashImage::wrap(std::string text, int width) 
{
	std::vector<std::string> result;

	std::string remaining = text;

	while (remaining.length() > 0)
	{
		int index = getSplitIndex(remaining, width);

		if (index == -1)
			break;

		result.push_back(remaining.substr(0, index));

		remaining = remaining.substr(index);

		if (index == 0)
			break;
	}

	return result;
}

int CSplashImage::getSplitIndex(std::string bigString, int width)
{
	int index = -1;
	int lastSpace = -1;
	std::string smallString = "";
	boolean spaceEncountered = false;
	boolean maxWidthFound = false;

	for (int i=0; i < bigString.length(); i++)
	{
		char current = bigString[ i];// charAt(i);
		smallString += current;

		if (current == ' ')
		{
			lastSpace = i;
			spaceEncountered = true;
		}

		//RECT rect = {0, 0, 0, 0};
		SIZE lsize;
		//DrawText( m_hTextDC, smallString.c_str(), -1, &rect, DT_CALCRECT | DT_NOPREFIX);
		GetTextExtentPoint32( m_hTextDC, smallString.c_str(), smallString.length(), &lsize);
		int linewidth = lsize.cx;//rect.right - rect.left;

		if ( linewidth > width)
		{
			if (spaceEncountered)
				index = lastSpace+1;
			else
				index = i;
			maxWidthFound = true;
			break;
		}
	}

	if (!maxWidthFound)
		index = bigString.length();

	return index;
}