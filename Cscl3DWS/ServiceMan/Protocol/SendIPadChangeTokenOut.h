#include "CommonPacketOut.h"

using namespace Protocol;

namespace ServiceManProtocol
{
	class CSendIPadChangeTokenOut : public Protocol::CCommonPacketOut
	{
	public:
		CSendIPadChangeTokenOut(int aToken);
	};
};