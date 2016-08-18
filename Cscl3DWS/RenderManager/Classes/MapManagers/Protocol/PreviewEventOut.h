#include "CommonPacketOut.h"
#include "oms_context.h"

class PreviewEventOut : public Protocol::CCommonPacketOut
{
public:
	PreviewEventOut( unsigned int aiRecordID, unsigned int aiStartTime, wchar_t *wsLogin, wchar_t *wsText);
};
