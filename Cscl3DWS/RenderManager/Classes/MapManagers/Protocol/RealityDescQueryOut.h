#pragma once
#include "MapServerDataBlock.h"

namespace MapManagerProtocol
{
	class CRealityDescQueryOut : public CMapServerPacketOutBase
	{
	public:
		CRealityDescQueryOut(unsigned int aRealityID);
	};
}
