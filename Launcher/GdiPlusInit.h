#pragma once

// GdiPlusInit.h: interface for the CGdiPlusInit class.
//
//////////////////////////////////////////////////////////////////////

class CGdiPlusInit  
{
public:
	CGdiPlusInit();
	virtual ~CGdiPlusInit();
	void Init();
	void Shutdown();
	bool Good(){ return m_bPesent; }
private:
	bool m_bPesent;
	ULONG_PTR m_token;
};
