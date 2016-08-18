#pragma once

#include "CommonGetObjectsDescQueryIn.h"

namespace MapManagerProtocol{
	class CObjectsDescIn : public CCommonGetObjectsDescQueryIn
	{
	public:
		CObjectsDescIn(BYTE *aData, int aDataSize, bool aOld_server);
		unsigned short GetObjectCountByPos();
	};
};