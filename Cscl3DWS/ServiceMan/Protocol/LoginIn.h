//#include "DataBuffer.h"
#pragma once

#include "CommonPacketIn.h"
#include <string>
using namespace Protocol;

namespace ServiceManProtocol{
	class CLoginIn : public CCommonPacketIn
	{
	protected:
		const BYTE* bsResult;
		unsigned int rmmlID;
		unsigned int bsResultSize;
		unsigned int typeSendClientLog;
	public:
		CLoginIn( BYTE *aData, int aDataSize);
		~CLoginIn();
		bool Analyse();
		const BYTE* GetMethodResult() const;
		unsigned int GetMethodResultSize() const;
		unsigned int getRmmlID() const;
		//unsigned int getTypeSendClientLog() const;
	};
};