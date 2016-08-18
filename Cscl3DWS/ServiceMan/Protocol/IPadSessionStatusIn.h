#pragma once

#include "IPadSessionBaseIn.h"
using namespace Protocol;

namespace ServiceManProtocol{
	class CIPadSessionStatusIn : public CIPadSessionBaseIn
	{
	protected:
		int status;
	public:
		CIPadSessionStatusIn( BYTE *aData, int aDataSize);
		~CIPadSessionStatusIn();
		bool Analyse();
		int getStatus() const;
	};
};