//#include "DataBuffer.h"
#pragma once

#include "CommonPacketIn.h"
#include <string>
using namespace Protocol;

namespace ServiceManProtocol{
	class CSetSessionStateResultIn : public CCommonPacketIn
	{
	protected:
		unsigned short type;
		unsigned short code;
	public:
		CSetSessionStateResultIn( BYTE *aData, int aDataSize);
		~CSetSessionStateResultIn();
		bool Analyse();
		unsigned short getType();
		unsigned short getCode();
	};
};