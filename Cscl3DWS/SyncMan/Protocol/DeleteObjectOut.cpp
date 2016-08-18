#include "stdafx.h"
#include "DeleteObjectOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CDeleteObjectOut::CDeleteObjectOut(unsigned int auiObjectID, unsigned int auiBornRealityID)
{
	data.clear();
	data.add(auiObjectID);
	data.add(auiBornRealityID);
}

