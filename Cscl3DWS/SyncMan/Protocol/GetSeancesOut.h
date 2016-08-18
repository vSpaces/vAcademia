#pragma once

namespace SyncManProtocol{
	class CGetSeancesOut : public CCommonPacketOut
	{
	public:
		CGetSeancesOut();
		CGetSeancesOut(unsigned int auID, unsigned int auIDSeancesObject);
	
	};
};