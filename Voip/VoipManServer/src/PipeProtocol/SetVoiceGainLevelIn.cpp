#include "stdafx.h"
#include "../../include/PipeProtocol/SetVoiceGainLevelIn.h"


CSetVoiceGainLevelIn::CSetVoiceGainLevelIn(BYTE *aData, int aDataSize) : 
		CCommonPipePacketIn(aData, aDataSize)
{
}

bool CSetVoiceGainLevelIn::Analyse()
{
	if ( !CCommonPipePacketIn::Analyse())
		return false;

	if(!data->Read( value))
		return false;

	return EndOfData();
}

float CSetVoiceGainLevelIn::GetValue()
{
	return value;
}