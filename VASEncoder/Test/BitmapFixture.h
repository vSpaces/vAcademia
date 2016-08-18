#pragma once

#include <WTypes.h>
#include <atlbase.h>
#include <atlapp.h>
#include <atlgdi.h>
#include <atlmisc.h>

class CBitmapFixture
{
public:
	CBitmapFixture(LPWSTR alpwFileName);
	~CBitmapFixture(void);

public:
	CSize		dimensions;
	LONG		bpp;
	void*		lpBits;
};
