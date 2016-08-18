#pragma once
#include "CommonPacketIn.h"

using namespace Protocol;

namespace ResManProtocol
{
	class CWriteRemoteResourceIn : public CCommonPacketIn
	{
	protected:
		BYTE errorID;
		DWORD packetID;
		BYTE unpackStatus;
		DWORD writeBytesCount;
	public:
		CWriteRemoteResourceIn( BYTE *aData, int aDataSize);
		virtual bool Analyse();
		DWORD GetPacketID();
		BYTE GetErrorID();
		BYTE GetUnpackStatus();
		DWORD GetWriteBytesCount();
	};
}