#include "CommonPacketOut.h"

using namespace Protocol;

namespace ServiceManProtocol
{
	class CExecMethodOut : public CCommonPacketOut
	{
	public:
		CExecMethodOut(int requestID, unsigned int rmmlID, const wchar_t *aMethodDescription );
	};
};