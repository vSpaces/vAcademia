#pragma once
#include "CommonPacketOut.h"

using namespace Protocol;

namespace ResManProtocol
{
	class CRootFileInfo2QueryOut : public CCommonPacketOut
	{
	public:
		CRootFileInfo2QueryOut();
	};
}
