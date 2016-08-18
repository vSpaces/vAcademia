#include "CommonPacketOut.h"
#include "oms_context.h"

class PreviewChatMessageOut : public Protocol::CCommonPacketOut
{
	public:
		PreviewChatMessageOut( unsigned int aiRecordID, unsigned int aiStartTime, int aType, wchar_t *wsLogin, wchar_t *wsText, bool abCloudedEnabled);
};
