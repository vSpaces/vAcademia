//#include "DataBuffer.h"
#pragma once

#include "CommonPacketIn.h"
#include <string>
using namespace Protocol;

namespace SyncManProtocol{
	class CEventUserBanUpdateIn : public CCommonPacketIn
	{
	protected:
		MP_WSTRING sEventName;
		MP_WSTRING sEventUrl;
		bool bBan;

	public:
		CEventUserBanUpdateIn( BYTE *aData, int aDataSize);
		~CEventUserBanUpdateIn();
		bool Analyse();		
		const wchar_t* GetEventName() const;
		const wchar_t* GetEventUrl() const;
		inline bool GetIsBan() {return bBan; }
	};
};