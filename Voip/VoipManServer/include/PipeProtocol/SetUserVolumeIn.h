#pragma once
#include "CommonPipePacketIn.h"

class CSetUserVolumeIn : public CCommonPipePacketIn
{
	std::wstring sUserName;
	int volume;
public:
	CSetUserVolumeIn(BYTE *aData, int aDataSize);
	virtual bool Analyse();
	std::wstring GetUserName();
	int GetVolume();
};