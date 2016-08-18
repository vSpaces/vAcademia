#pragma once
#include "CommonPipePacketIn.h"

class CUserTalkingIn : public CCommonPipePacketIn
{
        byte btTalking;
	int userID;
	MP_WSTRING userNick;
public:
	CUserTalkingIn(BYTE *aData, int aDataSize);
	byte GetIsTalking();
	unsigned int GetUserID();
	std::wstring GetUserNick();
	virtual bool Analyse();
};