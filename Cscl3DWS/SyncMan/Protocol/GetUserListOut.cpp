#include "stdafx.h"
#include "getUserListOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CGetUserListOut::CGetUserListOut(unsigned int aRealityID)
{
	data.clear();
	data.add(aRealityID);
}

