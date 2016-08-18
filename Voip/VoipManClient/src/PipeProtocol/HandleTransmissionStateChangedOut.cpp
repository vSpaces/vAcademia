#include "stdafx.h"
#include "include/PipeProtocol/HandleTransmissionStateChangedOut.h"

CHandleTransmissionStateChangedOut::CHandleTransmissionStateChangedOut(byte aID, bool aMicrophoneEnabled, bool aVoiceActivate) : CCommonPipePacketOut( aID)
{
	data->Add( aMicrophoneEnabled);
	data->Add( aVoiceActivate);
}