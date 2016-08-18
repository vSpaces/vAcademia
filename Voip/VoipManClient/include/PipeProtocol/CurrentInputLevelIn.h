#pragma once
#include "CommonPipePacketIn.h"

class CCurrentInputLevelIn : public CCommonPipePacketIn
{
	float currentInputLevel;
public:
	CCurrentInputLevelIn(BYTE *aData, int aDataSize);
	float GetCurrentInputLevel();
	virtual bool Analyse();
};