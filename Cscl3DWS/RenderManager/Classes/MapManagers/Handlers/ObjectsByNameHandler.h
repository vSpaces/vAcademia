
#include "HandlerBase.h"
#include "IObjectReceivedHandler.h"

class CMapManager;

class CObjectsByNameHandler:IObjectReceivedHandler
{
public:
	bool Handle(CCommonPacketIn* apQueryIn);

	void ReceivedHandle(unsigned int auRealityID, vecServerZoneObjects& zonesInfo, vecServerZoneObjects&	syncZonesInfo);
};