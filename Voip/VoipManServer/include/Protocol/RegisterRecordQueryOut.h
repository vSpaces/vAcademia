#pragma once
#include "..\RecordServer\Protocol\CommonVoipPacketOut.h"

class CRegisterRecordQueryOut : public CCommonVoipPacketOut
{
public:
	CRegisterRecordQueryOut(LPCSTR alpcVoipServerAddress, unsigned int aRecordID);
};