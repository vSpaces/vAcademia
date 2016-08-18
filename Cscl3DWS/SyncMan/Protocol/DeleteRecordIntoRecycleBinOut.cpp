#include "stdafx.h"
#include "DeleteRecordIntoRecycleBinOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CDeleteRecordIntoRecycleBinOut::CDeleteRecordIntoRecycleBinOut(unsigned int aRecordID)
{
	data.clear();
	data.add(aRecordID);
}

