//#include "DataBuffer.h"
#pragma once

#include "CommonPacketIn.h"
#include <string>
using namespace Protocol;

namespace ServiceManProtocol{
	class CTOInfoIn : public CCommonPacketIn
	{
	protected:
		int minutes;
	public:
		CTOInfoIn( BYTE *aData, int aDataSize);
		~CTOInfoIn();
		bool Analyse();
		unsigned int getMinutes() const;
	};
};