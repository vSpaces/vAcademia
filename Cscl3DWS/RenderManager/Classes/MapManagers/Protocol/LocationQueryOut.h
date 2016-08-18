#pragma once
#include "CommonPacketOut.h"
#include "oms_context.h"

namespace MapManagerProtocol
{
	class CLocationQueryOut : public Protocol::CCommonPacketOut
	{
	public:
		CLocationQueryOut(unsigned int uiPointsCount, CVector3D points[], const wchar_t* apwcLocationName);
	};
}
