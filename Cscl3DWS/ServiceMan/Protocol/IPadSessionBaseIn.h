#pragma once

#include "CommonPacketIn.h"
using namespace Protocol;

namespace ServiceManProtocol{
	class CIPadSessionBaseIn : public CCommonPacketIn
	{
	protected:
		unsigned int packetID;
		unsigned int token;
		int idx;
	public:
		CIPadSessionBaseIn( BYTE *aData, int aDataSize);
		~CIPadSessionBaseIn();
		bool Analyse();
		unsigned int GetPacketID() const;
		unsigned int GetToken() const;
	};
};