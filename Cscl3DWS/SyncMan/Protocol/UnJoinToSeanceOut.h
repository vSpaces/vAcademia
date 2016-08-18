#include "commonpacketout.h"

using namespace Protocol;

namespace SyncManProtocol{
	class CUnJoinToSeanceOut : public CCommonPacketOut
	{
	public:
		CUnJoinToSeanceOut();
		CUnJoinToSeanceOut(unsigned int auID);
	};
};