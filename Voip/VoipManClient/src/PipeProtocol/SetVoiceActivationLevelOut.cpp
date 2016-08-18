#include "stdafx.h"
#include "include/PipeProtocol/SetVoiceActivationLevelOut.h"

CSetVoiceActivationLevelOut::CSetVoiceActivationLevelOut(byte aID, float aValue) : CCommonPipePacketOut( aID)
{
	data->Add( aValue);
}