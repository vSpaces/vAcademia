#pragma once
#include "DesktopPacketOut.h"

namespace SharingManProtocol
{
	class RecordOut : public CDesktopPacketOut
	{
	public:
		RecordOut(unsigned short sessionID, unsigned int aPlayRecordID, unsigned int aRecordID);
		RecordOut(unsigned short sessionID, unsigned int aRecordID, LPCSTR alpFileName);
		RecordOut(unsigned short sessionID);
		~RecordOut(void);
	};
};
