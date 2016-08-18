#include "stdafx.h"
#include "include/PipeProtocol/InitVoipSystemResultOut.h"

CInitVoipSystemResultOut::CInitVoipSystemResultOut(byte aID, unsigned int aErrorCode, bool abInitVoipSystem, int aiVasDevice, int aiToIncludeSoundDevice, const wchar_t *alpcDeviceGuid, const wchar_t *alpcOutDeviceGuid) : CCommonPipePacketOut( aID)
{
	data->Add( aErrorCode);
	byte btInitVoipSystem = abInitVoipSystem;
	data->Add( btInitVoipSystem);	
	data->Add( aiVasDevice);
	data->Add( aiToIncludeSoundDevice);
	data->Add( alpcDeviceGuid);	
	data->Add( alpcOutDeviceGuid);	
}