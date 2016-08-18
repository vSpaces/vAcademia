#include "stdafx.h"
#include "SendIPadChangeTokenOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ServiceManProtocol;


CSendIPadChangeTokenOut::CSendIPadChangeTokenOut( int aToken)
{
	data.clear();
	data.add(aToken);
}

