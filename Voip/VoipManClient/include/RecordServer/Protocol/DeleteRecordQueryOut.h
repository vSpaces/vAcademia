#pragma once
#include "..\RecordServer\Protocol\CommonVoipPacketOut.h"

class CDeleteRecordQueryOut : public CCommonVoipPacketOut
{
public:
	CDeleteRecordQueryOut( unsigned int aRecordID);
};
