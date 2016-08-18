#include "stdafx.h"
#include "DeleteRecordOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CDeleteRecordOut::CDeleteRecordOut(unsigned int aRecordID)
{
	data.clear();
	data.add(aRecordID);
}

