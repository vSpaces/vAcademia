#pragma once

namespace SyncManProtocol{
	class CEventGetUserRightOut : public CCommonPacketOut
	{
	public:		
		CEventGetUserRightOut(unsigned int auEventID);
	
	};
};