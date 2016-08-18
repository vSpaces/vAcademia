#pragma once

namespace SyncManProtocol{
	class CStopRecordOut : public CCommonPacketOut
	{
	public:
		CStopRecordOut(unsigned int aRecordID);
	};
};