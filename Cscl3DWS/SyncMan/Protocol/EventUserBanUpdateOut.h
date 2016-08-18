#pragma once

namespace SyncManProtocol{
	class CEventUserBanUpdateOut : public CCommonPacketOut
	{
	public:
		CEventUserBanUpdateOut(unsigned int auID, wchar_t *apwcDescr);
	
	};
};