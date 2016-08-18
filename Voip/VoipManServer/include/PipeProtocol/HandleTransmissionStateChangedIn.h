#pragma once
#include "CommonPipePacketIn.h"

class CHandleTransmissionStateChangedIn : public CCommonPipePacketIn
{
	bool bMicrophoneEnabled;
	bool bVoiceActivate;
public:
	CHandleTransmissionStateChangedIn(BYTE *aData, int aDataSize);
	bool GetMicrophoneEnabled();
	bool GetVoiceActivate();
	virtual bool Analyse();
};