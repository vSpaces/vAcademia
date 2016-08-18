//#include "DataBuffer.h"
#pragma once

#include "CommonPacketIn.h"
#include <string>
using namespace Protocol;

namespace ServiceManProtocol{
	class CGetServiceIn : public CCommonPacketIn
	{
	protected:
		MP_WSTRING wsName;
		MP_WSTRING wsDescription;
		unsigned int rmmlID;
	public:
		CGetServiceIn( BYTE *aData, int aDataSize);
		~CGetServiceIn();
		bool Analyse();
		const wchar_t* GetServiceName() const;
		const wchar_t* GetServiceDescription() const;
		unsigned int getRmmlID() const;
	};
};