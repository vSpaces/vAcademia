#pragma once

#include "CommonPacketIn.h"
using namespace Protocol;

namespace ServiceManProtocol{
	class ComputerConfigurationIn : public CCommonPacketIn
	{
	protected:
		unsigned int code;
	public:
		ComputerConfigurationIn( BYTE *aData, int aDataSize);
		~ComputerConfigurationIn();
		bool Analyse();
		unsigned int getCode() const;
	};
};