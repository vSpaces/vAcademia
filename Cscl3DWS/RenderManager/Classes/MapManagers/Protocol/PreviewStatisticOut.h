#include "CommonPacketOut.h"

class PreviewStatisticOut : public Protocol::CCommonPacketOut
{
public:
	PreviewStatisticOut( unsigned int aiRecordID, unsigned int aiStartTime, wchar_t *wsStatistic);
};
