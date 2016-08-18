#pragma once

#include "IObjectReceivedHandler.h"
#include "RMContext.h"
#include "CommonPacketIn.h"


class CCreateAvatarHandler:IObjectReceivedHandler
{
public:

	CCreateAvatarHandler();

	bool Handle(Protocol::CCommonPacketIn* apQueryIn, oms::omsContext* pContext, unsigned int* createdAvatarID);
	void HandleObjects( std::vector<ZONE_OBJECT_INFO *>& zonesInfo);

	void ReceivedHandle(unsigned int auRealityID, std::vector<ZONE_OBJECT_INFO *>& zonesInfo, std::vector<ZONE_OBJECT_INFO *>& syncZonesInfo);

private:
	oms::omsContext* m_pContext;
	unsigned int* m_createdAvatarID;
};