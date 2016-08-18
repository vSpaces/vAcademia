#include "stdafx.h"
#include "include/PipeProtocol/InitVoipSystemOut.h"

CInitVoipSystemOut::CInitVoipSystemOut(byte aID, int aiVasDevice, int aiToIncludeSoundDevice, const wchar_t *alpcDeviceName, const wchar_t *alpcOutDeviceName) : CCommonPipePacketOut( aID)
{
	data->Add( aiVasDevice);
	data->Add( aiToIncludeSoundDevice);
	data->Add( alpcDeviceName);
	data->Add( alpcOutDeviceName);
}