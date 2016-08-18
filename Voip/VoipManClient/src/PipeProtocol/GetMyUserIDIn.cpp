#include "stdafx.h"
#include "../../include/PipeProtocol/GetMyUserIDIn.h"


CGetMyUserIDIn::CGetMyUserIDIn(BYTE *aData, int aDataSize) : 
		CCommonPipePacketIn(aData, aDataSize)
{
}

bool CGetMyUserIDIn::Analyse()
{
	if ( !CCommonPipePacketIn::Analyse())
		return false;

	if(!data->Read( userID))
		return false;
	
	byte btVoipSystemInitialized = 0;
	if(!data->Read(  btVoipSystemInitialized))
		return false;
	bVoipSystemInitialized = btVoipSystemInitialized!=0;

	return EndOfData();
}

int CGetMyUserIDIn::GetUserID()
{
	return userID;
}

bool CGetMyUserIDIn::IsVoipSystemInitialized()
{
	return bVoipSystemInitialized;
}