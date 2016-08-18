#pragma once
#include "CommonPacketOut.h"

using namespace Protocol;

namespace ResManProtocol
{
	class CReadRemoteResourceQueryOut : public CCommonPacketOut
	{
	public:
		CReadRemoteResourceQueryOut(const wchar_t* aResPath, DWORD aPos, DWORD aCount);
	};
}
