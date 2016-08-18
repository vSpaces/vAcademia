#include "CommonPacketOut.h"

using namespace Protocol;

namespace SyncManProtocol{
	class CJoinToSeanceOut : public CCommonPacketOut
	{
	public:
		CJoinToSeanceOut();
		CJoinToSeanceOut(unsigned int auLogicalID, unsigned int auID,  const wchar_t *aUser);
	};
};