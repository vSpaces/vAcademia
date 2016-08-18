#pragma once

namespace SyncManProtocol{
	class CGettingEventRecordsIn : public CCommonPacketIn
	{
		unsigned int iCountRecord;
		unsigned int totalCount;
	public:
		syncRecordExtendedInfo **pRecordInfo;

		CGettingEventRecordsIn( BYTE *aData, int aDataSize);
		~CGettingEventRecordsIn();
		bool Analyse();
		unsigned int GetCountRecord()  const;
		unsigned int GetTotalCountRecord()  const;
		sync::syncIRecordManagerCallback2 *pCallBack;
	};
};