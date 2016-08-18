#pragma once

namespace SyncManProtocol{
	class CReceiveBadSyncDataIn : public CCommonPacketIn
	{
	protected:
		unsigned int uObjectID;
		unsigned int uBornRealityID;
		unsigned int uRealityID;


	public:
		CReceiveBadSyncDataIn(BYTE *aData, int aDataSize );
		bool Analyse();
		unsigned int GetObjectID();
		unsigned int GetBornRealityID();
		unsigned int GetRealityID();
	};
};
