#pragma once

namespace SyncManProtocol{
	class CLogSeanceCreatedIn : public CCommonPacketIn
	{
	protected:
		unsigned int logSeanceObjectID;
		unsigned int seanceID;
		unsigned int seanceMode;
		MP_WSTRING sAuthor;
		MP_WSTRING sSeanceURL;

	public:
		CLogSeanceCreatedIn( BYTE *aData, int aDataSize);
		~CLogSeanceCreatedIn();
		bool Analyse();
		unsigned int GetSeanceID() const;
		unsigned int GetSeanceMode() const;
		const wchar_t* GetAuthor() const;
		const wchar_t* GetSeanceURL() const;
	};
};