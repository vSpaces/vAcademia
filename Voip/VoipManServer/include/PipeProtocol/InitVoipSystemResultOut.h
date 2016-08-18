#pragma once
#include "CommonPipePacketOut.h"

class CInitVoipSystemResultOut : public CCommonPipePacketOut
{
public:
	CInitVoipSystemResultOut(byte aID, unsigned int aErrorCode, bool abInitVoipSystem, int aiVasDevice, int aiToIncludeSoundDevice, const wchar_t *alpcDeviceGuid, const wchar_t *alpcOutDeviceGuid);
};