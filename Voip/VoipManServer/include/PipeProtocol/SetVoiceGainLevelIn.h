#pragma once
#include "CommonPipePacketIn.h"

class CSetVoiceGainLevelIn : public CCommonPipePacketIn
{
	float value;
public:
	CSetVoiceGainLevelIn(BYTE *aData, int aDataSize);
	float GetValue();
	virtual bool Analyse();
};