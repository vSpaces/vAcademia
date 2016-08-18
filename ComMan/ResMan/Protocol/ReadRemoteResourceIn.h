#pragma once

#include "CommonPacketIn.h"

using namespace Protocol;

namespace ResManProtocol{
	class CReadRemoteResourceIn : public CCommonPacketIn
	{
	protected:
		BYTE btExist;
		DWORD iFileSize;
		BYTE* pResourceData;
		DWORD iResourceDataSize;

	public:
		CReadRemoteResourceIn(BYTE *aData, int aDataSize);
		virtual bool Analyse();
		BYTE IsExist();
		DWORD GetResourceSize();
		BYTE* GetResourceData();
		DWORD GetResourceDataSize();
	};
};