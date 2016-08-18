#include "stdafx.h"
#include "RegisterServiceOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ServiceManProtocol;


CRegisterServiceOut::CRegisterServiceOut(int pendingID, unsigned int rmmlID)
{
	data.clear();

	data.add(pendingID);
	data.add(rmmlID);
}

