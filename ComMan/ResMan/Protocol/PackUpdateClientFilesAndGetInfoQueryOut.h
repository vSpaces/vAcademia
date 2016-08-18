#pragma once
#include "CommonPacketOut.h"

using namespace Protocol;

namespace ResManProtocol
{
	class CPackUpdateClientFilesAndGetInfoQueryOut : public CCommonPacketOut
	{
	public:
		CPackUpdateClientFilesAndGetInfoQueryOut(const wchar_t *aCurVersion);
	};
}
