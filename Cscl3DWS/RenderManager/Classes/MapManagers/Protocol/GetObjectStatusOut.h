#include "MapServerDataBlock.h"

namespace MapManagerProtocol{
	class CGetObjectStatusOut : public CMapServerPacketOutBase
	{
	public:
		CGetObjectStatusOut(unsigned int auRealityID, unsigned int auObjectID, unsigned int auBornRealityID);
	};
};