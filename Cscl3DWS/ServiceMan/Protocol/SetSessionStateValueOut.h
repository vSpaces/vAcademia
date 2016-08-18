#include "CommonPacketOut.h"
#include "ServiceMan.h"

using namespace Protocol;

namespace ServiceManProtocol
{
	class CSetSessionStateValueOut : public CCommonPacketOut
	{
	public:
		CSetSessionStateValueOut(service::E_SESSION_STATE_TYPES auState, unsigned short auValue);
	};
};