#pragma once

#include "IPadSessionBaseIn.h"
using namespace Protocol;

namespace ServiceManProtocol{
	class CIPadSessionBinStateIn : public CIPadSessionBaseIn
	{
	protected:
		unsigned char type;
		unsigned char* binState;
		unsigned int binStateSize;
	public:
		CIPadSessionBinStateIn( BYTE *aData, int aDataSize);
		~CIPadSessionBinStateIn();
		bool Analyse();
		unsigned char GetType() const;
		unsigned char *GetBinState() const;
		unsigned int GetBinStateSize() const;
	};
};