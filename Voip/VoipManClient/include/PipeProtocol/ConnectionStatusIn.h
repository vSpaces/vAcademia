#pragma once
#include "CommonPipePacketIn.h"

class CConnectionStatusIn : public CCommonPipePacketIn
{
	unsigned int code;
public:
	CConnectionStatusIn(BYTE *aData, int aDataSize);
	unsigned int GetCode();
	virtual bool Analyse();
};