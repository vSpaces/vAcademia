#pragma once
#include "CommonPipePacketIn.h"

class CSetUserGainLevelIn : public CCommonPipePacketIn
{
	std::wstring sUserName;
	double value;
public:
	CSetUserGainLevelIn(BYTE *aData, int aDataSize);
	double GetValue();
	std::wstring GetUserName();
	virtual bool Analyse();
};