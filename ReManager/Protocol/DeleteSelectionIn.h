#pragma once

#include <windows.h>
#include <winnt.h>
#include <windef.h>
#include <atlbase.h>
#include <atltime.h>

#include "CommonPacketIn.h"

using namespace Protocol;

class CDeleteSelectionIn : public CCommonPacketIn
{
public:
	CDeleteSelectionIn( BYTE *apData, int apDataSize);
	~CDeleteSelectionIn(void);
	bool Analyse();

	unsigned long GetBeginPos();
	unsigned long GetEndPos();

private:
	unsigned long m_beginPos;
	unsigned long m_endPos;

	BYTE* m_pData;
	unsigned int m_DataSize;

};
