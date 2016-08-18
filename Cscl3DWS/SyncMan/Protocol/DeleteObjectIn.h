#pragma once

namespace SyncManProtocol{
	class CDeleteObjectIn : public CCommonPacketIn
	{
	protected:
		unsigned int uiObjectID;
		unsigned int uiBornRealityID;

	public:
		CDeleteObjectIn(BYTE *aData, int aDataSize);
		virtual bool Analyse();
		unsigned int GetObjectID() { return uiObjectID;};
		unsigned int GetBornRealityID() { return uiBornRealityID;};
	};
};