#include "commonpacketout.h"
#include "oms_context.h"

namespace MapManagerProtocol
{
	class CDeleteRecordsOut : public Protocol::CCommonPacketOut
	{
	public:
		CDeleteRecordsOut();
		CDeleteRecordsOut(  unsigned int *pRecIDs, unsigned int count);
	
	};
};