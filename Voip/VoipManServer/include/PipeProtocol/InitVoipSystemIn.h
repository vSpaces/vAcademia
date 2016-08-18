#pragma once
#include "CommonPipePacketIn.h"

class CInitVoipSystemIn : public CCommonPipePacketIn
{
	int iVasDevice;
	int iToIncludeSoundDevice;
	MP_WSTRING sDeviceGuid;
	MP_WSTRING sOutDeviceGuid;
public:
	CInitVoipSystemIn(BYTE *aData, int aDataSize);
	virtual bool Analyse();
	int GetVasDevice(){ return iVasDevice;};
	int GetToIncludeSoundDevice(){ return iToIncludeSoundDevice;};	
	const wchar_t *GetDeviceGuid();
	const wchar_t *GetOutDeviceGuid();
};