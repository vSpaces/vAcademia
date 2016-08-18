#pragma once
#include "CommonPipePacketOut.h"

class CInitVoipSystemOut : public CCommonPipePacketOut
{
public:
	CInitVoipSystemOut(byte aID, int aiVasDevice, int aiToIncludeSoundDevice, const wchar_t *alpcDeviceName, const wchar_t *alpcOutDeviceName);
};