#pragma once

namespace SyncManProtocol{
	class CDeleteRecordOut : public CCommonPacketOut
	{
	public:
		CDeleteRecordOut();
		CDeleteRecordOut(unsigned int aRecordID);
	
	};
};