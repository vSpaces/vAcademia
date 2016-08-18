#include "stdafx.h"
#include "QueryObjectOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CQueryObjectOut::CQueryObjectOut(unsigned int aObjectID, unsigned int aBornRealityID,unsigned int aRealityID, long long aCreatingTime, bool checkFrame)
{
	data.clear();
	data.addData(sizeof( aObjectID), (BYTE*)&aObjectID);
	data.addData(sizeof( aBornRealityID), (BYTE*)&aBornRealityID);
	data.addData(sizeof( aRealityID), (BYTE*)&aRealityID);
	data.addData(sizeof( aCreatingTime), (BYTE*)&aCreatingTime);
	data.addData(sizeof( checkFrame), (BYTE*)&checkFrame);
}