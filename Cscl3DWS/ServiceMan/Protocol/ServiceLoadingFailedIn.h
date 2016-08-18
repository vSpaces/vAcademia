//#include "DataBuffer.h"
#pragma once

#include "CommonPacketIn.h"
#include <string>
using namespace Protocol;

namespace ServiceManProtocol{
	class CServiceLoadingFailedIn : public CCommonPacketIn
	{
	protected:
		unsigned int rmmlID;
		unsigned int errorCode;
	public:
		CServiceLoadingFailedIn( BYTE *aData, int aDataSize);
		~CServiceLoadingFailedIn();
		bool Analyse();
		unsigned int getRmmlID() const;
		unsigned int getErrorCode() const;
	};
};