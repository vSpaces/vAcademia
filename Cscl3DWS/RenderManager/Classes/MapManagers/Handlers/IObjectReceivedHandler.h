#pragma once

#include "MapManagerZone.h"

// интерфейс для отправки хэндлеру списка обработанных объектов
class IObjectReceivedHandler
{
public:
	virtual void ReceivedHandle(unsigned int auRealityID, std::vector<ZONE_OBJECT_INFO *>& zonesInfo, std::vector<ZONE_OBJECT_INFO *>& syncZonesInfo) = 0;
};