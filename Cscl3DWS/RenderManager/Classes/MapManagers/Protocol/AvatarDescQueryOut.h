#pragma once
#include "MapServerDataBlock.h"

namespace MapManagerProtocol
{
	class CAvatarDescQueryOut : public CMapServerPacketOutBase
	{
	public:
		CAvatarDescQueryOut(const wchar_t* apwcStartURL);
	};
}
