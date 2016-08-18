#pragma once

namespace SyncManProtocol{
	class CPlayOut : public CCommonPacketOut
	{
	public:
		CPlayOut();
		CPlayOut(unsigned int auID);
	};
};