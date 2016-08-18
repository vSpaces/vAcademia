#pragma once
#include "DesktopPacketOut.h"

namespace SharingManProtocol
{
	class HideSession : public CDesktopPacketOut
	{
	public:
		HideSession(unsigned short sessionID, LPCSTR alpFileName, bool  sessionState);
		~HideSession(void);
	};
};
