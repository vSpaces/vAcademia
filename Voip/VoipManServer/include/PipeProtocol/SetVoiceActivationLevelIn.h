#pragma once
#include "CommonPipePacketIn.h"

class CSetVoiceActivationLevelIn : public CCommonPipePacketIn
{
	float value;
public:
	CSetVoiceActivationLevelIn(BYTE *aData, int aDataSize);
	float GetValue();
	virtual bool Analyse();
};