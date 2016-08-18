#pragma once
#include "CommonPipePacketOut.h"

class CUserTalkingOut : public CCommonPipePacketOut
{
public:
	CUserTalkingOut(byte aID, byte abTalk, unsigned int aUserID, wchar_t *apNickName);
};