#pragma once
#include "DesktopPacketOut.h"

namespace SharingManProtocol
{
	class CRemoteDesktopOut : public CDesktopPacketOut
	{
	public:
		CRemoteDesktopOut(unsigned short aSessionID, unsigned char aType, unsigned short aFirstParam, unsigned int aSecondParam, unsigned char aCode);
	};
};
