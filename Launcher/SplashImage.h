#pragma once

#include <string>
#include <vector>

class CGdiPlusInit;
class CAlphaRegion;

#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }

struct InfoCadr
{
	LPTSTR shortInfo;
	LPTSTR sInfo;
	HBITMAP hBitmap;
	HBITMAP hTextBitmap;
	HBITMAP hLoadTextBitmap;
};

class CSplashImage
{
public:
	unsigned int m_animatedDotIndex;
	unsigned int m_animatedDotsCount;
	bool m_animatedDotsDirectionRight;
	HBITMAP m_hBitmapsDot[3];
	InfoCadr m_infoCadr[3];
	int m_iLanguage;
	int m_iCadr;
	int m_iLoadCadr;

public:
	CSplashImage();
	virtual ~CSplashImage();

protected:
	//IStream * CreateStreamOnResource(LPCTSTR lpName, LPCTSTR lpType);
	HBITMAP LoadSplashImage( unsigned int auResourceID);
	HBITMAP LoadSplashImage( LPCTSTR lpName);

public:
	void SetRootPath( WCHAR *sRootPath);
	void Load();
	void UnLoad();
	BOOL IsLoaded();
	HRGN GetRegion();
	void SetSplashImage(HWND hwndSplash, float alpha, int aLoadingPercent);
	void AnimatedDots();
	void ShowImgInfoCadr( int aiCadr, float alpha);
	void ShowTextInfoCadr( int aiCadr, int aiLoadCadr, float alpha, float loadAlpha);

private:
	std::vector<std::string> wrap(std::string text, int width);
	int getSplitIndex(std::string bigString, int width);

private:
	int m_loadingPercent;
	DWORD m_startTime;
	DWORD m_currTime;
	float m_cadrAlpha;
	float m_nextCadrAlpha;
	float m_loadCadrAlpha;
	float m_nextLoadCadrAlpha;
	BOOL m_bLoaded;

private:
	CGdiPlusInit *m_gdiPlusInit;
	CAlphaRegion *m_alphaRegion;
	WCHAR m_sRootPath[ MAX_PATH];
	HBITMAP m_hBitmapSplash;
	HDC m_hdcScreen;
	HDC m_hdcMem;
	SIZE m_sizeSplash;
	HBITMAP m_hbmpOld;

	HDC m_hdcBitmap;
	HBITMAP m_hDrawBitmap;	
	HBITMAP m_hbmpBitMapOld;

	HDC m_hTextBitmapDC;
	HDC m_hTextDC;
	HBITMAP m_hTextBmp;
	HBITMAP m_hOldTextBmp;	
	HFONT m_hOldFont;

	HFONT m_hFont;	

	Graphics *m_pGraphicTemp;
	Font *m_fontBold;
	Font *m_font;
	SolidBrush *m_textBrush;
	SolidBrush *m_textInfoBrush;
	StringFormat *m_stringFormat;
};
