#include "stdafx.h"
#include "../../include/PipeProtocol/HandleTransmissionStateChangedIn.h"


CHandleTransmissionStateChangedIn::CHandleTransmissionStateChangedIn(BYTE *aData, int aDataSize) : 
		CCommonPipePacketIn(aData, aDataSize)
{
}

bool CHandleTransmissionStateChangedIn::Analyse()
{
	if ( !CCommonPipePacketIn::Analyse())
		return false;

	if(!data->Read( bMicrophoneEnabled))
		return false;

	if(!data->Read( bVoiceActivate))
		return false;

	return EndOfData();
}

bool CHandleTransmissionStateChangedIn::GetMicrophoneEnabled()
{
	return bMicrophoneEnabled;
}

bool CHandleTransmissionStateChangedIn::GetVoiceActivate()
{
	return bVoiceActivate;
}