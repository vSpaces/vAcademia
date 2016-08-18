#include "CommonGetObjectsDescQueryIn.h"

namespace MapManagerProtocol{
	class CGetZonesObjectsDescQueryIn : public CCommonGetObjectsDescQueryIn
	{
	public:
		CGetZonesObjectsDescQueryIn(BYTE *aData, int aDataSize, bool aOld_server);
	};
};