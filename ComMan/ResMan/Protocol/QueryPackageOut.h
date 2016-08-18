#include "CommonPacketOut.h"

using namespace Protocol;

namespace ResManProtocol{

	class CQueryPackageOut : public CCommonPacketOut
	{
	public:
		CQueryPackageOut(unsigned int queryNum, const BYTE *aData, unsigned int aDataSize);
	};
};

