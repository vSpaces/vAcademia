#pragma once

#include <windows.h>
#include <winnt.h>
#include <windef.h>
#include <atlbase.h>
#include <atltime.h>

#include "SyncMan.h"

#include "CommonPacketOut.h"

using namespace Protocol;

class CGetRecordListOut : public CCommonPacketOut
{
public:
	CGetRecordListOut( sync::syncCountRecordInfo& totalCountRec, unsigned int auRecordCount, sync::syncRecordInfo** appRecordsInfo);
	~CGetRecordListOut(void);
};
