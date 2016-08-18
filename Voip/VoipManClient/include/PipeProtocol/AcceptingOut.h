#pragma once
#include "CommonPipePacketOut.h"

class CJoinChannelOut : public CCommonPipePacketOut
{
public:
	CJoinChannelOut(byte aID, const char *apwRoomName);
};