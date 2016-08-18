#pragma once
#include "CommonPacketOut.h"

using namespace Protocol;

namespace ResManProtocol
{
	class CUploadFileInfoQueryOut : public CCommonPacketOut
	{
	public:
		CUploadFileInfoQueryOut(const wchar_t* aResPath);
	};
}
