//#include "DataBuffer.h"
#pragma once

#include "CommonPacketIn.h"
#include <string>
using namespace Protocol;

namespace SyncManProtocol{
	class CEventUserRightUpdateIn : public CCommonPacketIn
	{
	protected:
		unsigned int uiEventID;
		MP_WSTRING sJSONEventUserCaps;

	public:
		CEventUserRightUpdateIn( BYTE *aData, int aDataSize);
		~CEventUserRightUpdateIn();
		bool Analyse();		
		inline unsigned int GetEventID() {return uiEventID; }
		const wchar_t* GetChangedEventUserCaps() const;
	};
};