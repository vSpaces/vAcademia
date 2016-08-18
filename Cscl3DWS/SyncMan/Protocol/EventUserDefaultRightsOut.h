#pragma once

namespace SyncManProtocol
{
	class CEventUserDefaultRightsOut : public CCommonPacketOut
	{
	public:		
		CEventUserDefaultRightsOut(unsigned int auEventID, unsigned int  auCapsMask);
	
	};
};