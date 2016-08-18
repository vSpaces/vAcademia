#pragma once
#include "CommonPipePacketIn.h"

class CGetUserIn : public CCommonPipePacketIn
{
	int msgID;
	int userID;
public:
	CGetUserIn(BYTE *aData, int aDataSize);
	virtual bool Analyse();
	int GetMsgID();
	int GetUserID();
};