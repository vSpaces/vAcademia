#pragma once

#include <windows.h>
#include <winnt.h>
#include <windef.h>
#include <atlbase.h>
#include <atltime.h>

#include "SyncMan.h"

#include "CommonPacketOut.h"

using namespace Protocol;

class CGetRecordInfoOut : public CCommonPacketOut
{
public:
	CGetRecordInfoOut( sync::syncRecordInfo* appRecordInfo);
	~CGetRecordInfoOut(void);
};
