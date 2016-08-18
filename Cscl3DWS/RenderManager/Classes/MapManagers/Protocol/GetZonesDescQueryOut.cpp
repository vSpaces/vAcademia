#include "stdafx.h"
#include "GetZonesDescQueryOut.h"

using namespace MapManagerProtocol;

CGetZonesDescQueryOut::CGetZonesDescQueryOut( unsigned int aRealityID, const ZoneID& aQueryZone) : CMapServerPacketOutBase()
{
	Add(aRealityID);
	Add(aQueryZone.id);
}

