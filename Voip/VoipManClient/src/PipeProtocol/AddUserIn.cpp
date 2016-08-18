#include "stdafx.h"
#include "../../include/PipeProtocol/AddUserIn.h"


CAddUserIn::CAddUserIn(BYTE *aData, int aDataSize) : 
		CCommonPipePacketIn(aData, aDataSize)
		, MP_WSTRING_INIT(userNick)
{
}

bool CAddUserIn::Analyse()
{
	if ( !CCommonPipePacketIn::Analyse())
		return false;

	if(!data->Read( userID))
		return false;

	if(!data->Read( userNick))
		return false;

	return EndOfData();
}


unsigned int CAddUserIn::GetUserID()
{
	return userID;
}

std::wstring CAddUserIn::GetUserNick()
{
	return userNick;
}