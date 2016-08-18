#pragma once

namespace SyncManProtocol{
	class CDeleteRecordIntoRecycleBinOut : public CCommonPacketOut
	{
	public:
		CDeleteRecordIntoRecycleBinOut();
		CDeleteRecordIntoRecycleBinOut(unsigned int aRecordID);
	
	};
};