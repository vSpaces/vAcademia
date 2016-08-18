#include "ObjectsDescIn.h"

namespace MapManagerProtocol{
	class CObjectsDescUpdateIn : public CObjectsDescIn
	{
	protected:

	public:
		CObjectsDescUpdateIn(BYTE *aData, int aDataSize) : CObjectsDescIn(aData, aDataSize){};
	};
};