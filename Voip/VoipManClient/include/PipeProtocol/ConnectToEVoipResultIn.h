#pragma once
#include "CommonPipePacketIn.h"

class CConnectToEVoipResultIn : public CCommonPipePacketIn
{
	unsigned int code;
public:
	CConnectToEVoipResultIn(BYTE *aData, int aDataSize);
	unsigned int GetCode();
	virtual bool Analyse();
};