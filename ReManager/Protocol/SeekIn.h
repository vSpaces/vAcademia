#pragma once

#include <windows.h>
#include <winnt.h>
#include <windef.h>
#include <atlbase.h>
#include <atltime.h>

#include "CommonPacketIn.h"

using namespace Protocol;

class CSeekIn :
	public CCommonPacketIn
{
public:
	CSeekIn( BYTE *apData, int apDataSize);
	~CSeekIn(void);
	bool Analyse();

	unsigned long GetSeekPos();
private:
	unsigned long m_seekPos;

	BYTE* m_pData;
	int m_DataSize;

};
