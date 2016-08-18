#pragma once
#include "CommonVoipPacketOut.h"

class CUnregisterRecordQueryOut : public CCommonVoipPacketOut
{
public:
	CUnregisterRecordQueryOut( unsigned int aRecordID);
};