#include "stdafx.h"
#include "GetZonesStaticObjectsQueryOut.h"

using namespace MapManagerProtocol;

CGetZonesStaticObjectsQueryOut::CGetZonesStaticObjectsQueryOut(unsigned int aPrevRealityID, unsigned int aRealityID, ZoneID zone)
{
	Add(aPrevRealityID);
	Add(aRealityID);	
	Add(zone.id);
}
