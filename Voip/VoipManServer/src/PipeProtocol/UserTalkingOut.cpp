#include "stdafx.h"
#include "include/PipeProtocol/UserTalkingOut.h"

CUserTalkingOut::CUserTalkingOut(byte aID, byte abTalk, unsigned int aUserID, wchar_t *apNickName) : CCommonPipePacketOut( aID)
{
	data->Add( abTalk);
	data->Add( aUserID);
	data->Add( apNickName);
}