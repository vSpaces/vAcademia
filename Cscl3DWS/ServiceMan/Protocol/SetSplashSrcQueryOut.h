#include "CommonPacketOut.h"
#include "oms_context.h"

class SetSplashSrcQueryOut : public Protocol::CCommonPacketOut
{
	public:
		SetSplashSrcQueryOut(unsigned int aiRecordID, LPCSTR alpcFileName);
};
