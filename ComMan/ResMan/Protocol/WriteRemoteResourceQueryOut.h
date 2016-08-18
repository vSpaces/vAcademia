#pragma once
#include "CommonPacketOut.h"

using namespace Protocol;

namespace ResManProtocol
{
	class CWriteRemoteResourceQueryOut : public CCommonPacketOut
	{
	public:
		CWriteRemoteResourceQueryOut(LPCTSTR aResPath, DWORD dwPos, BYTE *aData, DWORD aSizeToWrite, DWORD aTotalSize);
	};
}
