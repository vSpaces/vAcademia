#include "stdafx.h"
#include "DeleteRecordsOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CDeleteRecordsOut::CDeleteRecordsOut(  unsigned int *pRecIDs, unsigned int count)
{
	data.clear();
	data.add( (unsigned int)count);
	data.addData( count*sizeof(unsigned int), (BYTE*)pRecIDs);
}

