#pragma once

namespace SyncManProtocol{
	class CPlayerClosedIn : public CCommonPacketIn
	{
	protected:
		unsigned int seanceID;
	public:
		CPlayerClosedIn(BYTE *aData, int aDataSize):CCommonPacketIn(aData, aDataSize){};
		//~CPlayerClosed();
		bool Analyse();
		unsigned int GetSeanceID() const;
	};
};