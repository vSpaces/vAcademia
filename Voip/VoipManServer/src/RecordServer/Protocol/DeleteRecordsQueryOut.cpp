#include "stdafx.h"
#include "../../include/RecordServer/Protocol/DeleteRecordsQueryOut.h"

CDeleteRecordsQueryOut::CDeleteRecordsQueryOut(  unsigned int *pRecIDs, unsigned int count)
{
	//USES_CONVERSION;
	GetDataBuffer().addData( &count, sizeof(count));
	GetDataBuffer().addLargeArray((BYTE*)pRecIDs, count*sizeof(unsigned int));
	//data.addData( count*sizeof(unsigned int), (BYTE*)pRecIDs);
}

