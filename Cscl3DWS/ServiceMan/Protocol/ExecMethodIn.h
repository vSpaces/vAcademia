//#include "DataBuffer.h"
#pragma once

#include "CommonPacketIn.h"
#include <string>
using namespace Protocol;

namespace ServiceManProtocol{
	class CExecMethodIn : public CCommonPacketIn
	{
	protected:
		MP_WSTRING wsResult;
		unsigned int rmmlID;
		unsigned int requestID;
	public:
		CExecMethodIn( BYTE *aData, int aDataSize);
		~CExecMethodIn();
		bool Analyse();
		const wchar_t* GetMethodResult() const;
		unsigned int getRmmlID() const;
		unsigned int getRequestID() const;
	};
};