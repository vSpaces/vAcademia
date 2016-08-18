#include "stdafx.h"
#include "SendIPadObjectFullStateOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ServiceManProtocol;


CSendIPadObjectFullStateOut::CSendIPadObjectFullStateOut( unsigned int aPacketID, unsigned int aToken, BYTE* aBinState, int aBinStateSize)
{
	data.clear();
	data.add( aPacketID);
	data.add( aToken);
	data.addData( aBinStateSize, aBinState);
}

