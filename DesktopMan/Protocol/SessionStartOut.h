#include "DesktopPacketOut.h"

namespace SharingManProtocol
{
	class CSessionStartOut : public CDesktopPacketOut
	{
	public:
		CSessionStartOut( LPCSTR alpName, bool canRemoteAccess);
		CSessionStartOut( LPCSTR alpName, LPCSTR alpFileName, unsigned int aRecordID, bool isPaused, unsigned int aSeekTime, unsigned int aServerVersion);
	};
};