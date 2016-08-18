#include "MapServerDataBlock.h"
#include "ZoneId.h"

namespace MapManagerProtocol
{
	class CGetZonesStaticObjectsQueryOut : public CMapServerPacketOutBase
	{
	public:
		CGetZonesStaticObjectsQueryOut(unsigned int aPrevRealityID, unsigned int aRealityID, ZoneID zone);
	};
};