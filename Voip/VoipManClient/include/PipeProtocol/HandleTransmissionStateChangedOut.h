#pragma once
#include "CommonPipePacketOut.h"

class CHandleTransmissionStateChangedOut : public CCommonPipePacketOut
{
public:
	CHandleTransmissionStateChangedOut(byte aID, bool aMicrophoneEnabled, bool aVoiceActivate);
};