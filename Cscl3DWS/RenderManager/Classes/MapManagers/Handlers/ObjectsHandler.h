#include "HandlerBase.h"
#include "RMContext.h"
#include "IObjectReceivedHandler.h"

class CObjectsHandler:IObjectReceivedHandler
{
public:

	CObjectsHandler::CObjectsHandler();

	bool Handle(CCommonPacketIn* apQueryIn, oms::omsContext* pContext, unsigned int aRealityID, bool isStaticObjectsOnly = false);

	void ReceivedHandle(unsigned int auRealityID, std::vector<ZONE_OBJECT_INFO *>& zonesInfo, std::vector<ZONE_OBJECT_INFO *>& syncZonesInfo);

private:
	 oms::omsContext* m_pContext;
};