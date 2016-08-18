#pragma once
#include "MapServerDataBlock.h"

namespace MapManagerProtocol
{
	class CDeleteObjectsByIDQueryOut : public CMapServerPacketOutBase
	{
	public:
		CDeleteObjectsByIDQueryOut(int objCount, unsigned int objectID, unsigned int realityID);
	};
}
