#include "HandlerBase.h"
#include "RealityInfo.h"

class CMapManager;

class CDeleteObjectHandler: public CHandleBase
{
public:
	CDeleteObjectHandler(CCommonPacketIn* apQueryIn);
	bool Handle(unsigned int* aObjectID, unsigned int *aBornRealityID, ILogWriter *apLogWriter);
};