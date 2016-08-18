#include "stdafx.h"
#include "../../include/PipeProtocol/SetVoiceActivationLevelIn.h"


CSetVoiceActivationLevelIn::CSetVoiceActivationLevelIn(BYTE *aData, int aDataSize) : 
		CCommonPipePacketIn(aData, aDataSize)
{
}

bool CSetVoiceActivationLevelIn::Analyse()
{
	if ( !CCommonPipePacketIn::Analyse())
		return false;

	if(!data->Read( value))
		return false;

	return EndOfData();
}

float CSetVoiceActivationLevelIn::GetValue()
{
	return value;
}