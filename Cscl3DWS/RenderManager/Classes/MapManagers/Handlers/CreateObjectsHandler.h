
#include "HandlerBase.h"
#include "IObjectReceivedHandler.h"

class CCreateObjectsHandler:IObjectReceivedHandler
{
public:
	CCreateObjectsHandler(){};

	bool Handle(CCommonPacketIn* apQueryIn);

	void ReceivedHandle(unsigned int auRealityID, std::vector<ZONE_OBJECT_INFO *>& zonesInfo, std::vector<ZONE_OBJECT_INFO *>& syncZonesInfo);
};