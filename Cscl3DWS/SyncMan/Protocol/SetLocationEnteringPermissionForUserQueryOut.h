#pragma once

namespace SyncManProtocol
{
	class CSetLocationEnteringPermissionForUserQueryOut : public CCommonPacketOut
	{
	public:
		CSetLocationEnteringPermissionForUserQueryOut(const wchar_t* apwcUserLogin, const wchar_t* apwcLocationID, unsigned int auRealityID, bool abAllow);
	};
};