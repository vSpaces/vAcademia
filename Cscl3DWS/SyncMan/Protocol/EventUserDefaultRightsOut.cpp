#include "stdafx.h"
#include "EventUserDefaultRightsOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CEventUserDefaultRightsOut::CEventUserDefaultRightsOut(unsigned int auEventID, unsigned int  auCapsMask)
{
	data.clear();
	data.add( auEventID);
	data.add( auCapsMask);
}


