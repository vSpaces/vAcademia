#pragma once
#include "CommonPipePacketOut.h"

class CSetUserVolumeOut : public CCommonPipePacketOut
{
public:
	CSetUserVolumeOut(byte aID, const wchar_t *aUserName, int aVolume);
};