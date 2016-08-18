#include "CommonPacketOut.h"

using namespace Protocol;

namespace ServiceManProtocol
{
	class CGetServiceOut : public CCommonPacketOut
	{
	public:
		CGetServiceOut(unsigned int aRmmlID, const wchar_t *aServiceName );
	};
};