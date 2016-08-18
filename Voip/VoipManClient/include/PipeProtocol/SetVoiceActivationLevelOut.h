#pragma once
#include "CommonPipePacketOut.h"

class CSetVoiceActivationLevelOut : public CCommonPipePacketOut
{
public:
	CSetVoiceActivationLevelOut(byte aID, float aValue);
};