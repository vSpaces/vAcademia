#pragma once

class CAlphaRegion
{
public:
	CAlphaRegion();
	virtual ~CAlphaRegion();

public:
	bool Create( HBITMAP ahBitmap);	
	HRGN GetRegion();

protected:
	HRGN	m_hRegion;
};

