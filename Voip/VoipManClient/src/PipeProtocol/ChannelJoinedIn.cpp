#include "stdafx.h"
#include "../../include/PipeProtocol/ChannelJoinedIn.h"


CChannelJoinedIn::CChannelJoinedIn(BYTE *aData, int aDataSize) : 
		CCommonPipePacketIn(aData, aDataSize)
		,MP_WSTRING_INIT(channelPath)
{
}

bool CChannelJoinedIn::Analyse()
{
	if ( !CCommonPipePacketIn::Analyse())
		return false;

	if(!data->Read( channelPath))
		return false;

	return EndOfData();
}

std::wstring CChannelJoinedIn::GetChannelPath()
{
	return channelPath;
}