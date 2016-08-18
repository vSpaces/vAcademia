#include "DesktopPacketOut.h"

class VASEFramesDifPack;

namespace SharingManProtocol
{
	class CFrameOut : public CDesktopPacketOut
	{
	public:
		CFrameOut(unsigned short sessionID, unsigned int aTime, VASEFramesDifPack* pack, bool abKeyFrame, bool useNewCodec);
	};
};