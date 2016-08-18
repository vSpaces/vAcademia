#include "stdafx.h"
#include "include/PipeProtocol/ChannelJoinedOut.h"

CChannelJoinedOut::CChannelJoinedOut(byte aID, wchar_t *aChannelPath) : CCommonPipePacketOut( aID)
{
	data->Add( aChannelPath);
}