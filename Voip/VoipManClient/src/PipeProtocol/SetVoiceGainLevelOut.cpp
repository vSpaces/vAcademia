#include "stdafx.h"
#include "include/PipeProtocol/SetVoiceGainLevelOut.h"

CSetVoiceGainLevelOut::CSetVoiceGainLevelOut(byte aID, float aValue) : CCommonPipePacketOut( aID)
{
	data->Add( aValue);
}