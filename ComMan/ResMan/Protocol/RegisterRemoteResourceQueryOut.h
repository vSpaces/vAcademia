#pragma once
#include "CommonPacketOut.h"

using namespace Protocol;

namespace ResManProtocol
{
	class CRegisterRemoteResourceQueryOut : public CCommonPacketOut
	{
	public:
		CRegisterRemoteResourceQueryOut(const wchar_t *aFilePath);
	};
}
