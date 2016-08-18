#pragma once

#include <windows.h>
#include <winnt.h>
#include <windef.h>
#include <atlbase.h>
#include <atltime.h>

#include "CommonPacketIn.h"

using namespace Protocol;

class CRecordEditorProtocolIn : public CCommonPacketIn
{
public:
	CRecordEditorProtocolIn( BYTE *apData, int apDataSize);
	~CRecordEditorProtocolIn(void);
	bool Analyse();
};
