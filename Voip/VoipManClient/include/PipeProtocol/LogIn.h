#pragma once
#include "CommonPipePacketIn.h"

class CLogIn : public CCommonPipePacketIn
{
	std::string sLog;
public:
	CLogIn(BYTE *aData, int aDataSize);	
	virtual bool Analyse();
	std::string GetLog();
};