#include "stdafx.h"
#include "DeleteRecordSegmentOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CDeleteRecordSegmentOut::CDeleteRecordSegmentOut( unsigned long begPos, unsigned long endPos)
{
	data.clear();
	data.add( begPos);
	data.add( endPos);
}

