//#include "DataBuffer.h"
#pragma once

#include "CommonPacketIn.h"
#include <string>
using namespace Protocol;

namespace SyncManProtocol{
	class CInfoMessageIn : public CCommonPacketIn
	{
	protected:
		std::wstring sMessage;
		unsigned int uiMsgCode;		

	public:
		CInfoMessageIn( BYTE *aData, int aDataSize);
		~CInfoMessageIn();
		bool Analyse();
		const wchar_t* GetMessage() const;
		inline unsigned int GetMessageCode(){ return uiMsgCode;};	
	};
};