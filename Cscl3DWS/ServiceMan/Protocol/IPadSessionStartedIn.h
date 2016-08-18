#pragma once

#include "IPadSessionBaseIn.h"
using namespace Protocol;

namespace ServiceManProtocol{
	class CIPadSessionStartedIn : public CIPadSessionBaseIn
	{
	public:
		CIPadSessionStartedIn( BYTE *aData, int aDataSize);
		~CIPadSessionStartedIn();
		bool Analyse();
	};
};