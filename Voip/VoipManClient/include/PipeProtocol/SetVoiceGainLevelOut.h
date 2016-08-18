#pragma once
#include "CommonPipePacketOut.h"

class CSetVoiceGainLevelOut : public CCommonPipePacketOut
{
public:
	CSetVoiceGainLevelOut(byte aID, float aValue);
};