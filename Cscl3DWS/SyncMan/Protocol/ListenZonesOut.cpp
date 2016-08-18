#include "stdafx.h"
#include "ListenZonesOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CListenZonesOut::CListenZonesOut(unsigned int aRealityID,
								 unsigned int aZoneCount, unsigned long* aZones)
{
	data.add( aRealityID);
	for (unsigned int i = 0;  i < aZoneCount;  i++)
		data.add( aZones[ i]);
}
