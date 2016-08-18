#pragma once

namespace SyncManProtocol{
	class CReceivedPrecisedURLIn : public CCommonPacketIn
	{
	protected:
		MP_WSTRING sURL;

	public:
		CReceivedPrecisedURLIn( BYTE *aData, int aDataSize);
		~CReceivedPrecisedURLIn();
		bool Analyse();
		const wchar_t* GetURL() const;
	};
};