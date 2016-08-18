#include "commonpacketout.h"

using namespace Protocol;

namespace SyncManProtocol{
	class CDeleteRecordsOut : public CCommonPacketOut
	{
	public:
		CDeleteRecordsOut();
		CDeleteRecordsOut(  unsigned int *pRecIDs, unsigned int count);
	
	};
};