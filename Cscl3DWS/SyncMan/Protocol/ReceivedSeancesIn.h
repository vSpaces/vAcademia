#pragma once

namespace SyncManProtocol{

	class CReceivedSeancesIn : public CCommonPacketIn
	{
		unsigned int iSeancesCount;
		unsigned int IDLogObject;

	public:
		CReceivedSeancesIn( BYTE *aData, int aDataSize);
		~CReceivedSeancesIn();
		bool Analyse();
		unsigned int GetSeancesCount() const;
		unsigned int GetIDLogObject()  const;
		syncSeanceInfo **pSeanceInfo;
	};
};