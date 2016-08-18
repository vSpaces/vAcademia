#pragma once
#include "MapServerDataBlock.h"

namespace MapManagerProtocol
{
	class CDeleteObjectsByNameQueryOut : public CMapServerPacketOutBase
	{
	public:
		CDeleteObjectsByNameQueryOut(int objCount, CComString &aName);
	};
}
