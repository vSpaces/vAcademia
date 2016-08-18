
#include "StdAfx.h"
#include "SyncMan.h"
#include "ObjectsHandler.h"

CObjectsHandler::CObjectsHandler():
m_pContext(NULL)
{

}

bool CObjectsHandler::Handle(CCommonPacketIn* apQueryIn, oms::omsContext* pContext, unsigned int aRealityID, bool isStaticObjectsOnly)
{
	std::vector<ZONE_OBJECT_INFO *>	zonesInfo;
	std::vector<ZONE_OBJECT_INFO *>	syncZonesInfo;

	m_pContext = pContext;

	gRM->mapManServer->get_zones_objects_end(isStaticObjectsOnly, apQueryIn, aRealityID, zonesInfo, syncZonesInfo, true, this);

	return true;
}

void CObjectsHandler::ReceivedHandle(unsigned int auRealityID, std::vector<ZONE_OBJECT_INFO *>& zonesInfo, std::vector<ZONE_OBJECT_INFO *>&	syncZonesInfo)
{
	gRM->mapMan->UpdateSyncObjects(syncZonesInfo);

	if (zonesInfo.size() > 0)
	{
		gRM->mapMan->UpdateObjectsInfo(zonesInfo);
	}

	m_pContext->mpSM->OnMapObjectsLoaded();

	CLogValue logValue(
		"[MAPMANAGER]: Handling of zone objects description. Object count: ",
		zonesInfo.size() + syncZonesInfo.size());
	m_pContext->mpLogWriter->WriteLnLPCSTR( logValue.GetData());
}