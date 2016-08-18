#pragma once

namespace SyncManProtocol{
	class CStopOut //: public CCommonPacketOut
	{
	public:
		CStopOut();
		unsigned char* GetData();
		unsigned short GetDataSize();
	
	};
};