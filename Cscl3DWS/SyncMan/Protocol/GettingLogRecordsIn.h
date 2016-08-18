#pragma once

namespace SyncManProtocol{
	class CGettingLogRecordsIn : public CCommonPacketIn
	{
		unsigned int iCountRecord;
		syncCountRecordInfo totalCountInfo;
	public:
		syncRecordInfo **pRecordInfo;

		CGettingLogRecordsIn( BYTE *aData, int aDataSize);
		~CGettingLogRecordsIn();
		bool Analyse();
		unsigned int GetCountRecord()  const;
		syncCountRecordInfo GetTotalCountRecord()  const;
		sync::syncIRecordManagerCallback * pCallBack;
	};
};