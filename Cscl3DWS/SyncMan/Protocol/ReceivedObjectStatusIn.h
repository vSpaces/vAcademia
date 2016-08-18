#pragma once

namespace SyncManProtocol{
	class CReceivedObjectStatusIn : public CCommonPacketIn
	{
	protected:
		unsigned int uiObjectID;
		unsigned int uiBornRealityID;
		int miStatus;

	public:
		CReceivedObjectStatusIn(BYTE *aData, int aDataSize);
		virtual bool Analyse();
		unsigned int GetObjectID() { return uiObjectID;};
		unsigned int GetBornRealityID() { return uiBornRealityID;};
		int GetStatus(){ return miStatus; }
	};
};