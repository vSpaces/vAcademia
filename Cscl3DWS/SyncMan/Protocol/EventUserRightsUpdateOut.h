#pragma once

namespace SyncManProtocol{
	class CEventUserRightsUpdateOut : public CCommonPacketOut
	{
	public:		
		CEventUserRightsUpdateOut(unsigned int auEventID, wchar_t *auCapsDescr);
	
	};
};