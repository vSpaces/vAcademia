#include "stdafx.h"
#include "SendIPadLogoutOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ServiceManProtocol;


CSendIPadLogoutOut::CSendIPadLogoutOut( int aCode)
{
	data.clear();
	data.add( aCode);
}

