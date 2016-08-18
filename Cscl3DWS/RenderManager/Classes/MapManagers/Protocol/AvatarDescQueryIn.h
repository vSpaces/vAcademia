#include "ObjectsDescIn.h"

namespace MapManagerProtocol{
	class CAvatarDescQueryIn : public CObjectsDescIn
	{
	public:
		CAvatarDescQueryIn(BYTE *aData, int aDataSize, bool aOld_server);
	};
};