#include "MapServerDataBlock.h"
#include "oms_context.h"

namespace MapManagerProtocol{
	class CGetObjectsDescQueryOut : public CMapServerPacketOutBase
	{
	public:
		CGetObjectsDescQueryOut(unsigned int aRealityID, unsigned int aCount, cm::cmObjectIDs *aObjects);
	};
};