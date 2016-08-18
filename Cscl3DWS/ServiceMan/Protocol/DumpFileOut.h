//#include "DataBuffer.h"
#pragma once

#include "CommonPacketOut.h"
using namespace Protocol;

namespace ServiceManProtocol{
	class CDumpFileOut : public CCommonPacketOut
	{	
	public:
		CDumpFileOut( BYTE *aData, int aDataSize, int aDataTotalSize);
		~CDumpFileOut();
	};
};