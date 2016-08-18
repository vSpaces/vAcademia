//#include "DataBuffer.h"
#pragma once

#include "CommonPacketIn.h"
#include <string>

using namespace Protocol;

namespace SyncManProtocol
{

	class CLocationEnteredIn : public Protocol::CCommonPacketIn
	{
	public:
		CLocationEnteredIn( BYTE *aData, int aDataSize);
		~CLocationEnteredIn();

	public:
		bool Analyse();
		const wchar_t* GetLocationID() const;
		const wchar_t* GetJSONDescription() const;

	private:
		std::wstring wsLocationID;
		std::wstring wsJSONDescription;
	};

};