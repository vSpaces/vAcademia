#include "..\RecordServer\Protocol\CommonVoipPacketOut.h"

class CDeleteRecordsQueryOut : public CCommonVoipPacketOut
{
public:
	CDeleteRecordsQueryOut();
	CDeleteRecordsQueryOut(  unsigned int *pRecIDs, unsigned int count);
};