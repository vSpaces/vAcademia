#include "CommonPacketOut.h"
#include "oms_context.h"

class SetPreviewDefaultImageOut : public Protocol::CCommonPacketOut
{
	public:
		SetPreviewDefaultImageOut(unsigned int aiRecordID, wchar_t *wcFileName);
};
