#include "stdafx.h"
#include "include/PipeProtocol/AcceptingOut.h"

CJoinChannelOut::CJoinChannelOut(byte aID, const char *apwRoomName) : CCommonPipePacketOut( aID)
{
	data->Add( apwRoomName);
}