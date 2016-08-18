#pragma once
#include "CommonPipePacketIn.h"

class CInitVoipSystemResultIn : public CCommonPipePacketIn
{
	unsigned int errorCode;
	byte btInitVoipSystem;
	int iVasDevice;
	int iToIncludeSoundDevice;
	MP_WSTRING sDeviceName;
	MP_WSTRING sOutDeviceName;
public:
	CInitVoipSystemResultIn(BYTE *aData, int aDataSize);
	unsigned int GetErrorCode();
	virtual bool Analyse();
	bool IsInitVoipSystem(){ return btInitVoipSystem!=0;};
	int GetToIncludeSoundDevice(){ return iToIncludeSoundDevice;};
	int GetVasDevice(){ return iVasDevice;};
	std::wstring GetDeviceName();
	std::wstring GetOutDeviceName();
};