#include "stdafx.h"
#include "PlayOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CPlayOut::CPlayOut(unsigned int auID)
{
	data.clear();
	data.add( auID);
}

