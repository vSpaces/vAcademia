#pragma once

namespace SyncManProtocol{
	class CStartRecordOut : public CCommonPacketOut
	{
	public:
		CStartRecordOut();
		CStartRecordOut(int aiRecordMode, int aiParentRealityID, const wchar_t *aLocationName, const wchar_t *apwcRecordName, const wchar_t *apwcLanguage);
	
	};
};