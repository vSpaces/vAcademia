
#include "StdAfx.h"
#include "RMContext.h"
#include "ObjectsByNameHandler.h"

bool CObjectsByNameHandler::Handle(CCommonPacketIn* apQueryIn)
{
	std::vector<ZONE_OBJECT_INFO *> zonesInfo;
	std::vector<ZONE_OBJECT_INFO *> syncZonesInfo;

	gRM->mapManServer->get_zones_objects_end(false, apQueryIn, gRM->mapMan->GetCurrentRealityID(), zonesInfo, syncZonesInfo, true, this);

	return true;
}

void CObjectsByNameHandler::ReceivedHandle(unsigned int auRealityID, vecServerZoneObjects& zonesInfo, vecServerZoneObjects& syncZonesInfo)
{
	gRM->mapMan->UpdateSyncObjects(syncZonesInfo);

	if (zonesInfo.size() > 0)
		gRM->mapMan->UpdateObjectsInfo(zonesInfo);
}