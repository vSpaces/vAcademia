#include "CommonPacketOut.h"
#include "oms_context.h"

namespace MapManagerProtocol
{
	class PreviewRecordStopQueryOut : public Protocol::CCommonPacketOut
	{
		public:
			PreviewRecordStopQueryOut(unsigned int aiRecordID, unsigned int aiDuration);
	};
};