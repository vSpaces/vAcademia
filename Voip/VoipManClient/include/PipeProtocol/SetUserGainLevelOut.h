#pragma once
#include "CommonPipePacketOut.h"

class CSetUserGainLevelOut : public CCommonPipePacketOut
{
public:
	CSetUserGainLevelOut(byte aID, const wchar_t* alwpcUserLogin, double aValue);
};