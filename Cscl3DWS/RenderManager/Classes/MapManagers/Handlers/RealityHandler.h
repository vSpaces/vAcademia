#include "HandlerBase.h"
#include "RealityInfo.h"

class CMapManager;

class CRealityHandler: public CHandleBase
{
public:
	CRealityHandler(CCommonPacketIn* apQueryIn);
	bool Handle(CRealityInfo &oRealityInfo, ILogWriter *apLogWriter);
};