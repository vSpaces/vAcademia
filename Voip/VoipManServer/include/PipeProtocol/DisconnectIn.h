#pragma once
#include "CommonPipePacketIn.h"

class CDisconnectIn : public CCommonPipePacketIn
{
public:
	CDisconnectIn(BYTE *aData, int aDataSize);
	virtual bool Analyse();
};