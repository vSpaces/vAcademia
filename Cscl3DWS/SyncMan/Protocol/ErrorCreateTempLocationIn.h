//#include "DataBuffer.h"
#pragma once

#include "CommonPacketIn.h"
#include <string>
using namespace Protocol;

namespace SyncManProtocol{
	class CErrorCreateTempLocationIn : public CCommonPacketIn
	{
	protected:
		unsigned int errorCode;

	public:
		CErrorCreateTempLocationIn( BYTE *aData, int aDataSize);
		~CErrorCreateTempLocationIn();
		bool Analyse();		
		inline unsigned int GetErrorCode() {return errorCode; }
	};
};