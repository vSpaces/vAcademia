#pragma once

namespace SyncManProtocol{

	class CQueryEventRecordsOut : public CCommonPacketOut
	{

	public:
		CQueryEventRecordsOut(unsigned int auID, int aiBegin, int aiCount, const wchar_t* apwcFilter, const wchar_t* apwcOrderBy, sync::syncIRecordManagerCallback2 *pCallback);
	};
};