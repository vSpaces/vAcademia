#include "stdafx.h"
#include "../../include/PipeProtocol/DoLoginIn.h"


CDoLoginIn::CDoLoginIn(BYTE *aData, int aDataSize) : 
		CCommonPipePacketIn(aData, aDataSize),
		MP_WSTRING_INIT(sUserName),
		MP_WSTRING_INIT(sPassword),
		MP_WSTRING_INIT(sRoomName)
{
}

bool CDoLoginIn::Analyse()
{
	if ( !CCommonPipePacketIn::Analyse())
		return false;

	if(!data->Read( (std::wstring&)sUserName))
		return false;
	
	if(!data->Read( (std::wstring&)sPassword))
		return false;

	if(!data->Read( (std::wstring&)sRoomName))
		return false;

	return EndOfData();
}

std::wstring CDoLoginIn::GetUserName()
{
	return sUserName;
}

std::wstring CDoLoginIn::GetPassword()
{
	return sPassword;
}

std::wstring CDoLoginIn::GetRoomName()
{
	return sRoomName;
}