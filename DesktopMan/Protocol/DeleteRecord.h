#pragma once
#include "DesktopPacketOut.h"

namespace SharingManProtocol
{
	class CDeleteRecord: public CDesktopPacketOut
	{
	public:
		CDeleteRecord(unsigned int aRecordID);
	};
}
