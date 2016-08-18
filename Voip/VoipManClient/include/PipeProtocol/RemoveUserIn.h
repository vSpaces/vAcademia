#pragma once
#include "CommonPipePacketIn.h"

class CRemoveUserIn : public CCommonPipePacketIn
{
	int userID;
public:
	CRemoveUserIn(BYTE *aData, int aDataSize);
	unsigned int GetUserID();
	virtual bool Analyse();
};