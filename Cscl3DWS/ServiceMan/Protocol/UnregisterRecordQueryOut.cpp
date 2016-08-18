#include "stdafx.h"
#include "UnregisterRecordQueryOut.h"

CUnregisterRecordQueryOut::CUnregisterRecordQueryOut( unsigned int aRecordID)
{
	GetDataBuffer().addData( &aRecordID, sizeof(aRecordID));
}