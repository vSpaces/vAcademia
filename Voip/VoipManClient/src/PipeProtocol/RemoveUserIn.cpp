#include "stdafx.h"
#include "../../include/PipeProtocol/RemoveUserIn.h"


CRemoveUserIn::CRemoveUserIn(BYTE *aData, int aDataSize) : 
		CCommonPipePacketIn(aData, aDataSize)
{
}

bool CRemoveUserIn::Analyse()
{
	if ( !CCommonPipePacketIn::Analyse())
		return false;

	if(!data->Read( userID))
		return false;

	return EndOfData();
}


unsigned int CRemoveUserIn::GetUserID()
{
	return userID;
}
