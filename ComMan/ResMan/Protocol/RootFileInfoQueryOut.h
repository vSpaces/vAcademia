#pragma once
#include "CommonPacketOut.h"

using namespace Protocol;

namespace ResManProtocol
{
	class CRootFileInfoQueryOut : public CCommonPacketOut
	{
	public:
		CRootFileInfoQueryOut();
	};
}
