//#include "DataBuffer.h"
#pragma once

#include "CommonPacketIn.h"
#include <string>
using namespace Protocol;

namespace SyncManProtocol{
	class CEventUserRightsInfoIn : public CCommonPacketIn
	{
	protected:
		unsigned int uiEventID;
		MP_WSTRING sJSONEventUserCaps;

	public:
		CEventUserRightsInfoIn( BYTE *aData, int aDataSize);
		~CEventUserRightsInfoIn();
		bool Analyse();		
		inline unsigned int GetEventID() {return uiEventID; }
		const wchar_t* GetEventUserCaps() const;
	};
};