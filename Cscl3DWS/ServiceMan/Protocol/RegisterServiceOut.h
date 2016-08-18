#include "CommonPacketOut.h"

using namespace Protocol;

namespace ServiceManProtocol
{
	class CRegisterServiceOut : public CCommonPacketOut
	{
	public:
		CRegisterServiceOut(int pendingID, unsigned int rmmlID);
	};
};