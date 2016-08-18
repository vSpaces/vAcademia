#pragma once

#include <windows.h>
#include <winnt.h>
#include <windef.h>
#include <atlbase.h>
#include <atltime.h>

#include "CommonPacketIn.h"

using namespace Protocol;

class CGetRecordInfoIn :
	public CCommonPacketIn
{
public:
	CGetRecordInfoIn( BYTE *apData, int apDataSize);
	~CGetRecordInfoIn(void);
	bool Analyse();

	int GetRecordID();

private:
	int m_recordID;
	BYTE* m_pData;
	int m_DataSize;

};
