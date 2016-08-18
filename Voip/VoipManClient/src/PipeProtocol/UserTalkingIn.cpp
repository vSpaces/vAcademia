#include "stdafx.h"
#include "../../include/PipeProtocol/UserTalkingIn.h"


CUserTalkingIn::CUserTalkingIn(BYTE *aData, int aDataSize) : 
		CCommonPipePacketIn(aData, aDataSize)
		, MP_WSTRING_INIT(userNick)
{
}

bool CUserTalkingIn::Analyse()
{
	if ( !CCommonPipePacketIn::Analyse())
		return false;

	if(!data->Read( btTalking))
		return false;

	if(!data->Read( userID))
		return false;

	if(!data->Read( userNick))
		return false;


	return EndOfData();
}

byte CUserTalkingIn::GetIsTalking()
{
	return btTalking;
}

unsigned int CUserTalkingIn::GetUserID()
{
	return userID;
}

std::wstring CUserTalkingIn::GetUserNick()
{
	return userNick;
}