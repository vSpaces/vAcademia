#include "MapServerDataBlock.h"
#include "ZoneId.h"

namespace MapManagerProtocol
{
	class CGetZonesObjectsDescQueryOut : public CMapServerPacketOutBase
	{
	public:
		CGetZonesObjectsDescQueryOut(unsigned int aPrevRealityID, unsigned int aRealityID, 
			ZoneID aCurrentZone, const std::vector<ZoneID>& aFullQueryZones);
	};
};