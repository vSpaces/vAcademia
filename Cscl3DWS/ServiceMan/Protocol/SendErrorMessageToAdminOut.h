#include "CommonPacketOut.h"
#include "oms_context.h"

class SendErrorMessageToAdminOut : public Protocol::CCommonPacketOut
{
	public:
		SendErrorMessageToAdminOut( const wchar_t *apErrorText);
};
