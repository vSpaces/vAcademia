#include "CommonPacketOut.h"

using namespace Protocol;

namespace ServiceManProtocol
{
	class CSendIPadLogoutOut : public Protocol::CCommonPacketOut
	{
	public:
		CSendIPadLogoutOut( int aCode);
	};
};