//#include "DataBuffer.h"
#pragma once

#include "CommonPacketIn.h"
#include <string>
using namespace Protocol;

namespace ServiceManProtocol{
	class CNotifyMethodIn : public CCommonPacketIn
	{
	protected:
		//const wchar_t* wsResult;
		std::wstring wsResult;
		unsigned int rmmlID;
	public:
		CNotifyMethodIn( BYTE *aData, int aDataSize);
		~CNotifyMethodIn();
		bool Analyse();
		const wchar_t* GetMethodResult() const;
		unsigned int getRmmlID() const;
	};
};