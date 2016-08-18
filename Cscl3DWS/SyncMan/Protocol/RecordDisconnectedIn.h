#pragma once

namespace SyncManProtocol{
	class CRecordDisconnectedIn  : public CCommonPacketIn
	{
		unsigned int muRecordID;
		bool isAuthor;
		bool isNeedToAsk;
	public:
		CRecordDisconnectedIn( BYTE* aData, int aDataSize);
		~CRecordDisconnectedIn();
		bool Analyse();
		unsigned int getRecordID() const  {return muRecordID; } 
		bool  GetIsAuthor() const {return isAuthor; } 
		bool  GetIsNeedToAsk() const {return isNeedToAsk; } 
	};
};