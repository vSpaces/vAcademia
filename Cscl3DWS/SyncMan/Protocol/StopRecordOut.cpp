#include "stdafx.h"
#include "StopRecordOut.h"

using namespace SyncManProtocol;

CStopRecordOut::CStopRecordOut(unsigned int aRecordID)
{
	data.clear();
	data.add( aRecordID);
}
