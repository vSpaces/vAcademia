#pragma once

namespace SyncManProtocol{
	class CUpdateRecordInfoOut : public CCommonPacketOut
	{
	public:
		CUpdateRecordInfoOut();
		//~CListenZonezOut();
		CUpdateRecordInfoOut(unsigned int aRecordID, const wchar_t *aName,  const wchar_t *aSubject, const wchar_t *aLesson, const wchar_t *aLevel, const wchar_t *aPreview, const wchar_t *aDescript, bool isReal, const wchar_t *aTags);
	
	};
};