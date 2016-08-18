#include "CommonPacketOut.h"

using namespace Protocol;

namespace ServiceManProtocol
{
	class CSendIPadObjectFullStateOut : public Protocol::CCommonPacketOut
	{
	public:
		CSendIPadObjectFullStateOut( unsigned int aPacketID, unsigned int aToken, BYTE* aBinState, int aBinStateSize);
	};
};