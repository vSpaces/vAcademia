#pragma once
#include "CommonPipePacketIn.h"

class CAddUserIn : public CCommonPipePacketIn
{
	int userID;
	MP_WSTRING userNick;
public:
	CAddUserIn(BYTE *aData, int aDataSize);
	unsigned int GetUserID();
	std::wstring GetUserNick();
	virtual bool Analyse();
};