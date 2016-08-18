#pragma once

namespace SyncManProtocol{
	class CSyncMessageIn : public CCommonPacketIn
	{
	protected:
		unsigned int iObjectID;
		unsigned int iBornRealityID;
		unsigned short iSenderNameLen;
		MP_WSTRING pSenderName;
		BYTE *mpData;
		unsigned int iLocalDataSize;

	public:
		CSyncMessageIn(BYTE *aData, int aDataSize);
		~CSyncMessageIn();
		bool Analyse();
		unsigned int GetObjectID();
		unsigned int GetBornRealityID();
		const wchar_t* GetSenderName();
		BYTE *GetData();
		unsigned int GetDataLength();
	};
};