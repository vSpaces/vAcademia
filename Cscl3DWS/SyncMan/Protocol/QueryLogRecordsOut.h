#pragma once

namespace SyncManProtocol{

	class CQueryLogRecordsOut : public CCommonPacketOut
	{

	public:
		CQueryLogRecordsOut(unsigned int auID, int aiBegin, int aiCount, const wchar_t* apwcFilter, const wchar_t* apwcOrderBy, sync::syncIRecordManagerCallback * pCallback);
	};
};