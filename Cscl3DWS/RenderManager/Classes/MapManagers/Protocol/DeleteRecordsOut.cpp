#include "stdafx.h"
#include "DeleteRecordsOut.h"

using namespace MapManagerProtocol;

CDeleteRecordsOut::CDeleteRecordsOut(  unsigned int *pRecIDs, unsigned int count)
{
	data.clear();
	data.add( (unsigned int)count);
	data.addData( count*sizeof(unsigned int), (BYTE*)pRecIDs);
}

