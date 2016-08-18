#pragma once
#include "CommonVoipPacketOut.h"

class CRegisterRecordQueryOut : public CCommonVoipPacketOut
{
public:
	CRegisterRecordQueryOut(LPCSTR alpcVoipServerAddress, unsigned int aRecordID);
};