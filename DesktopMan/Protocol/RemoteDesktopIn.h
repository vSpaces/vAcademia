#pragma once
#include "CommonPacketIn.h"

class CRemoteDesktopIn: public Protocol::CCommonPacketIn
{
public:
	CRemoteDesktopIn ( BYTE *aData, int aDataSize);
	virtual bool Analyse();

	unsigned int GetType();
	unsigned int GetFirstParam();
	unsigned int GetSecondParam();
	unsigned int GetCode();

protected:
	unsigned char auType;
	unsigned short auFirstParam;
	unsigned int auSecondParam;
	unsigned char auCode;
};
