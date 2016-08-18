#pragma once

#include "IPadSessionBaseIn.h"
using namespace Protocol;

namespace ServiceManProtocol{
	class CIPadSessionStopedIn : public CIPadSessionBaseIn
	{
	protected:
		unsigned int code;
	public:
		CIPadSessionStopedIn( BYTE *aData, int aDataSize);
		~CIPadSessionStopedIn();
		bool Analyse();
//		unsigned int getCode() const;
	};
};