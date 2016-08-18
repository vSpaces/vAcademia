#include "CommonPacketOut.h"

using namespace Protocol;

namespace ServiceManProtocol
{
	class CSendIPadSlideSrcChangedOut : public Protocol::CCommonPacketOut
	{
	public:
		CSendIPadSlideSrcChangedOut( unsigned int aPacketID, unsigned int aToken, float aProportion, float aScaleX, float aScaleY, const wchar_t *aSrc);
	};
};