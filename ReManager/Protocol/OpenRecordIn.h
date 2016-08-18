#pragma once

#include <windows.h>
#include <winnt.h>
#include <windef.h>
#include <atlbase.h>
#include <atltime.h>

#include "CommonPacketIn.h"

using namespace Protocol;

class COpenRecordIn : public CCommonPacketIn
{
public:
	COpenRecordIn( BYTE *apData, int apDataSize);
	~COpenRecordIn(void);
	bool Analyse();
	std::wstring GetURL();

private:
	std::wstring m_URL;
	BYTE *m_pData;
	int m_DataSize;
};
