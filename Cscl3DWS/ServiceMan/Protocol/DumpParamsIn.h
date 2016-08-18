//#include "DataBuffer.h"
#pragma once

#include "CommonPacketIn.h"
#include <string>
using namespace Protocol;

namespace ServiceManProtocol{
	class CDumpParamsIn : public CCommonPacketIn
	{
	protected:
		unsigned int code;
	public:
		CDumpParamsIn( BYTE *aData, int aDataSize);
		~CDumpParamsIn();
		bool Analyse();
		unsigned int getCode() const;
	};
};