#pragma once
#include "oms_context.h"

namespace SyncManProtocol{

	class CPrecisedURLIn : public CCommonPacketIn
	{

	public:
		CPrecisedURLIn( BYTE *aData, int aDataSize);
		~CPrecisedURLIn();

	public:
		bool Analyse();
		const wchar_t* GetURL() const;
		const wchar_t* GetEnteredEventInfo() const;
		const unsigned int GetErrorCode() const;

	protected:
		MP_WSTRING	sURL;
		MP_WSTRING	sEnteredEventInfo;
		unsigned int	uErrorCode;
	};

};