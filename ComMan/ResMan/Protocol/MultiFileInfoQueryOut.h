#pragma once
#include "CommonPacketOut.h"

using namespace Protocol;

namespace ResManProtocol
{
	class CMultiFileInfoQueryOut : public CCommonPacketOut
	{
	public:
		CMultiFileInfoQueryOut( const wchar_t* aResPath);
	};
}
