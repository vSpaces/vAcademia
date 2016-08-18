#pragma once

#include "ObjectsDescIn.h"

namespace MapManagerProtocol{
	class CGetObjectsDescQueryIn : public CObjectsDescIn
	{
	protected:

	public:
		CGetObjectsDescQueryIn(BYTE *aData, int aDataSize) : CObjectsDescIn(aData, aDataSize){};
	};
};