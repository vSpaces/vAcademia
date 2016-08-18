#include "StdAfx.h"
#include "SyncMan.h"
#include "CreateAvatarHandler.h"
#include "MapManagerServer.h"

CCreateAvatarHandler::CCreateAvatarHandler():
m_pContext(NULL),
m_createdAvatarID(NULL)
{

}

void CCreateAvatarHandler::HandleObjects( std::vector<ZONE_OBJECT_INFO *>& zonesInfo)
{
	vecServerZoneObjects::iterator it = zonesInfo.begin();
	vecServerZoneObjects::iterator itEnd = zonesInfo.end();
	for (;  it != itEnd;  it++)
	{
		ZONE_OBJECT_INFO* info = *it;
		if (info == NULL)
			continue;

		// регистрирует описание синхронизируемого объекта в SyncMan
		info->isMyAvatar = (*m_createdAvatarID == info->complexObjectID.objectID);
		if (info->isMyAvatar)
		{
			gRM->mapMan->SetMainObjectCoords(info->translation.x, info->translation.y, info->translation.z);

			char* avatarLocation;
			float avatarX;
			float avatarY;
			float avatarZ;

			m_pContext->mpMapMan->GetAvatarPosition( avatarX, avatarY, avatarZ, &avatarLocation);
			m_pContext->mpSyncMan->SetAvatarPosition(avatarX, avatarY, avatarLocation);
		}
		m_pContext->mpSyncMan->PutMapObject( *info);

		MP_DELETE(info);
		*it = NULL;
	}
	zonesInfo.clear();
}

bool CCreateAvatarHandler::Handle( CCommonPacketIn* apQueryIn, oms::omsContext* pContext, unsigned int* createdAvatarID)
{
	m_pContext = pContext;
	m_createdAvatarID = createdAvatarID; 

	std::vector<ZONE_OBJECT_INFO *>	zonesInfo;
	std::vector<ZONE_OBJECT_INFO *>	syncZonesInfo;
	gRM->mapManServer->get_zones_objects_end(false, apQueryIn, 0, zonesInfo, syncZonesInfo, false /*не откидывать пакеты из другой реальности*/, this);

	return true;
}

void CCreateAvatarHandler::ReceivedHandle(unsigned int auRealityID, std::vector<ZONE_OBJECT_INFO *>& zonesInfo, std::vector<ZONE_OBJECT_INFO *>& syncZonesInfo)
{
	if (*m_createdAvatarID == 0xFFFFFFFF)
	{
		if (syncZonesInfo.size() != 0)
			*m_createdAvatarID = syncZonesInfo[0]->complexObjectID.objectID;
		else if (zonesInfo.size() != 0)
			*m_createdAvatarID = zonesInfo[0]->complexObjectID.objectID;
	}

	gRM->mapMan->SetConnectedRealityID( auRealityID);
	HandleObjects( syncZonesInfo);
	HandleObjects( zonesInfo);
}