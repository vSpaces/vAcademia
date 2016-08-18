//#include "DataBuffer.h"
#pragma once

#include "CommonPacketIn.h"
#include <string>
using namespace Protocol;

namespace ServiceManProtocol{
	class CLogoutIn : public CCommonPacketIn
	{
	protected:
		unsigned int code;
	public:
		CLogoutIn( BYTE *aData, int aDataSize);
		~CLogoutIn();
		bool Analyse();
		unsigned int getCode() const;
	};
};