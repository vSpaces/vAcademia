#include "DesktopPacketOut.h"

namespace SharingManProtocol
{
	class StartSendFrameOut: public CDesktopPacketOut
	{
	public:
		StartSendFrameOut( LPCSTR alpName);
	};
};
