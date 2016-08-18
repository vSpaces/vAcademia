#include "stdafx.h"
#include "../../include/RecordServer/Protocol/DeleteRecordQueryOut.h"

CDeleteRecordQueryOut::CDeleteRecordQueryOut(unsigned int aRecordID)
{
	GetDataBuffer().addData( &aRecordID, sizeof(aRecordID));
}