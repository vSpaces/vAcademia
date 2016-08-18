#pragma once

namespace SyncManProtocol{
	class CRecordConnectedIn  : public CCommonPacketIn
	{
		byte isAuthorOrAssistant;
		unsigned int uiDuration;
		unsigned int uiErrorCode;
		MP_WSTRING mpwcLocationName;
		MP_WSTRING mpwcAdditionalObjectInfo;
	public:
		CRecordConnectedIn( BYTE* aData, int aDataSize);
		~CRecordConnectedIn();
		bool Analyse();
		syncRecordInfo recordInfo;
		syncRecordInfo* getRecordInfo() {return &recordInfo; } 
		byte GetIsAuthorOrAssistant() const { return isAuthorOrAssistant; }
		const wchar_t* GetLocationName() const { return mpwcLocationName.c_str(); }
		unsigned int GetDuration() const { return uiDuration; }

		/** Возвращает код ошибки начала записи
			ERROR_NO_ERROR = 0
			ERROR_LOCATION_NOT_EXISTS = 25
			ERROR_RECORD_ALREADY_STARTED = 22
			ERROR_START_RECORD_FAILED = 21
		\return unsigned int
		\details
		*/
		unsigned int GetErrorCode() const { return uiErrorCode; }

		const wchar_t* GetAdditionalObjectInfo() const { return mpwcAdditionalObjectInfo.c_str(); }
	};
};