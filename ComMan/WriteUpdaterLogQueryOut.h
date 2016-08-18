#pragma once
#include "CommonPacketOut.h"

using namespace Protocol;

namespace ResManProtocol
{
	class CWriteUpdaterLogQueryOut : public CCommonPacketOut
	{
	public:
		CWriteUpdaterLogQueryOut(const char *aLogValue);
	};
}
