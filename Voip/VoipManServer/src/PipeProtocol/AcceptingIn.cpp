#include "stdafx.h"
#include "../../include/PipeProtocol/AcceptingIn.h"


CJoinChannelIn::CJoinChannelIn(BYTE *aData, int aDataSize) : 
		CCommonPipePacketIn(aData, aDataSize),
		MP_STRING_INIT(sRoomName)
{
}

bool CJoinChannelIn::Analyse()
{
	if ( !CCommonPipePacketIn::Analyse())
		return false;

	if(!data->Read((std::string&)sRoomName))
		return false;

	return EndOfData();
}

std::string CJoinChannelIn::GetRoomName()
{
	return sRoomName;
}