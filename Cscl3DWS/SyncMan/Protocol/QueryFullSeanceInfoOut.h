#pragma once

namespace SyncManProtocol{
	class CQueryFullSeanceInfoOut : public CCommonPacketOut
	{
	public:
		CQueryFullSeanceInfoOut();
		CQueryFullSeanceInfoOut(unsigned int auIDSeanceRmmlObject, unsigned int auLogRecordID);
	};
};