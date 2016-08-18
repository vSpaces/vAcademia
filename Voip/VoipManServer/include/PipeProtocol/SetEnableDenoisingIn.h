#pragma once
#include "CommonPipePacketIn.h"

class CSetEnableDenoisingIn : public CCommonPipePacketIn
{
	bool bEnabled;
public:
	CSetEnableDenoisingIn(BYTE *aData, int aDataSize);
	bool GetEnabled();
	virtual bool Analyse();
};