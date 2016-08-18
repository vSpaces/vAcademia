#include "MapServerDataBlock.h"
#include "ZoneId.h"

namespace MapManagerProtocol{
	class CGetZonesDescQueryOut : public CMapServerPacketOutBase
	{
	public:
		CGetZonesDescQueryOut( unsigned int aRealityID, const ZoneID& aQueryZone);
	};
};