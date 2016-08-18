#include "stdafx.h"
#include "../../include/PipeProtocol/GetUserIn.h"


CGetUserIn::CGetUserIn(BYTE *aData, int aDataSize) : 
		CCommonPipePacketIn(aData, aDataSize)
{
}

bool CGetUserIn::Analyse()
{
	if ( !CCommonPipePacketIn::Analyse())
		return false;

	if(!data->Read( msgID))
		return false;

	if(!data->Read( userID))
		return false;

	return EndOfData();
}

int CGetUserIn::GetUserID()
{
	return userID;
}

int CGetUserIn::GetMsgID()
{
	return msgID;
}