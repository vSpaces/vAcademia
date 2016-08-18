#pragma once

#include "CommonPacketIn.h"
using namespace Protocol;

namespace SyncManProtocol{
	class CRecordEditorOpenIn : public CCommonPacketIn
	{
	protected:
		int iErrorCode;
		int iRecordDuration;
		MP_WSTRING sRecordName;
		MP_WSTRING sRecordDescription;
		MP_WSTRING sAuthor;
		MP_WSTRING sCreationDate;
		MP_WSTRING sLocation;
		MP_WSTRING sLanguage;
		MP_WSTRING sURL;

	public:
		CRecordEditorOpenIn( BYTE *aData, int aDataSize);
		bool Analyse();		
		const int GetErrorCode() const;
		const int GetRecordDuration() const;
		const std::wstring GetRecordName() const;
		const std::wstring GetRecordDescription() const;
		const std::wstring GetRecordCreationDate() const;
		const std::wstring GetRecordAuthor() const;
		const std::wstring GetRecordLanguage() const;
		const std::wstring GetRecordLocation() const;
		const std::wstring GetRecordURL() const;
	};
};