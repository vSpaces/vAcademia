
#include "StdAfx.h"
#include "RMContext.h"
#include "ZonesHandler.h"
#include "GetZonesDescQueryIn.h"
#include "GlobalSingletonStorage.h"
#include "CommonPacketIn.h"
#include "MapManagerServer.h"

using namespace MapManagerProtocol;
/*
Пришел ответ с описанием зон.
Нужно теперь послать запрос для получения объектов.
*/
bool CZonesHandler::Handle(Protocol::CCommonPacketIn* apQueryIn)
{
	unsigned int uPrevRealityID = gRM->mapMan->GetPrevRealityID();
	unsigned int uRealityID = gRM->mapMan->GetCurrentRealityID();
	ZoneID currentZone = gRM->mapMan->GetCurrentZoneID();

	CGetZonesDescQueryIn zoneQueryIn( (BYTE*)apQueryIn->GetData(), apQueryIn->GetDataSize());
	if(!zoneQueryIn.Analyse())
	{
		g->lw.WriteLn("[ANALYSE FALSE] CGetZonesDescQueryIn");
		ATLASSERT( false);
		return false;
	}

	unsigned int uiQueryRealityID = zoneQueryIn.GetRealityID();
	unsigned int uiQueryCurrentZoneID = zoneQueryIn.GetCurrentZoneID();

	// Если запрос устарел - уходим
	if (uiQueryRealityID != uRealityID || uiQueryCurrentZoneID != currentZone.id)
		return true;

	// Если земля не пришла - уходим
	if (!zoneQueryIn.CurrentZoneHasGround())
	{
		gRM->mapMan->HandleTeleportError(0, L"Нет земли", L"");
		return true;
	}

	CVectorZones* pActiveZones = gRM->mapMan->GetActiveZones();

	unsigned short globalZoneCount;

	std::vector<ZoneID> fullQueryZones;

	globalZoneCount = zoneQueryIn.GetCountGlobalZone();
	for (int k = 0; k < globalZoneCount; k++)
	{
		unsigned int zoneID;
		zoneQueryIn.GetGlobalZoneInfo((unsigned short)k, zoneID);
		fullQueryZones.push_back(zoneID);
	}

	pActiveZones->clear();
	for (int k = 0; k < globalZoneCount; k++)
	{
		unsigned int zoneID;
		zoneQueryIn.GetGlobalZoneInfo((unsigned short)k, zoneID);
		gRM->mapMan->AddZoneIfNotExists( zoneID);
		pActiveZones->push_back( gRM->mapMan->GetZone( zoneID));
	}
	gRM->mapMan->ActivateZones();

	// Послали запрос объектов по зонам
	gRM->mapManServer->get_zones_objects_start( uPrevRealityID, uRealityID, currentZone, fullQueryZones);

	/*CMapDataAsynchObject* apQueryInNew = gRM->mapManServer->get_zones_objects_start(
		uPrevRealityID, uRealityID, currentZone, fullQueryZones);
	assert(apQueryInNew);*

	zoneDescResults.insert(std::map<CMapDataAsynchObject *, ZonesDescQueryResult *>::value_type(apQueryInNew, pResult));
	activeDataAsynchObjects.push_back(apQueryInNew);*/

	return true;
}