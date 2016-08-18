#include "CommonPacketOut.h"
#include "oms_context.h"

class PreviewSetAudioTimeQueryOut : public Protocol::CCommonPacketOut
{
	public:
		PreviewSetAudioTimeQueryOut(unsigned int aiRecordID, unsigned int aiStartTime, LPCSTR alpcFileName);
};


class PreviewSetSplashSrcQueryOut : public Protocol::CCommonPacketOut
{
public:
	PreviewSetSplashSrcQueryOut(unsigned int aiRecordID, LPCSTR alpcFileName);
};
