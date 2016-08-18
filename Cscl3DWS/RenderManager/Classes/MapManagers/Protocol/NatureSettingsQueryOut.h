#pragma once
#include "MapServerDataBlock.h"

namespace MapManagerProtocol
{
	class CNatureSettingsQueryOut : public CMapServerPacketOutBase
	{
	public:
		CNatureSettingsQueryOut(unsigned int aZoneID);
	};
}
