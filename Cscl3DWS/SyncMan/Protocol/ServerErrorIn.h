//#include "DataBuffer.h"
#pragma once

#include "CommonPacketIn.h"
#include <string>
using namespace Protocol;

namespace SyncManProtocol{
	class CServerErrorIn : public CCommonPacketIn
	{
	protected:
		unsigned int mErrorCode;

	public:
		CServerErrorIn( BYTE *aData, int aDataSize);
		~CServerErrorIn();
		bool Analyse();
		unsigned int GetErrorCode() const;
	};
};