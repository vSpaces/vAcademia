#pragma once

class DESKTOPMAN_API CSelectorBase
{
public:
	CSelectorBase(void);
	~CSelectorBase(void);

	void MakeWhiteBitmap(void* imgBits, int width, int height);
	HBITMAP CreateDIBSection( unsigned int auWidth, unsigned int auHeight, unsigned int auBPP, void** lppBits, BITMAPINFO &bmpInfo, bool abReverseHMode);

private:
	HDC			m_hScreenDC;
};
