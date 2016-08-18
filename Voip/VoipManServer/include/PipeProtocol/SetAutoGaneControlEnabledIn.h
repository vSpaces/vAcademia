#pragma once
#include "CommonPipePacketIn.h"

class CSetAutoGaneControlEnabledIn : public CCommonPipePacketIn
{
	bool bEnabled;
public:
	CSetAutoGaneControlEnabledIn(BYTE *aData, int aDataSize);
	bool GetEnabled();
	virtual bool Analyse();
};