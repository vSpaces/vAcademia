
#include "StdAfx.h"
#include "RMContext.h"
#include "CreateObjectsHandler.h"

bool CCreateObjectsHandler::Handle(CCommonPacketIn* apQueryIn)
{
	std::vector<ZONE_OBJECT_INFO *> zonesInfo;
	std::vector<ZONE_OBJECT_INFO *> syncZonesInfo;

	gRM->mapManServer->get_zones_objects_end(false, apQueryIn, gRM->mapMan->GetCurrentRealityID(), zonesInfo, syncZonesInfo, true, this);

	return true;
}

void CCreateObjectsHandler::ReceivedHandle(unsigned int auRealityID, std::vector<ZONE_OBJECT_INFO *>& zonesInfo, std::vector<ZONE_OBJECT_INFO *>& syncZonesInfo)
{
	gRM->mapMan->UpdateSyncObjects( syncZonesInfo);

	if (zonesInfo.size() > 0)
		gRM->mapMan->UpdateObjectsInfo(zonesInfo);
}

