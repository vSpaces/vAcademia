#pragma once
#include "CommonPacketOut.h"

using namespace Protocol;

namespace ResManProtocol
{
	class CFileInfoQueryOut : public CCommonPacketOut
	{
	public:
		CFileInfoQueryOut(unsigned char *aResPath);
	};
}
