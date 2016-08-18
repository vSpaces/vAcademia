#pragma once
#include "CommonPipePacketOut.h"

class CChannelJoinedOut : public CCommonPipePacketOut
{
public:
	CChannelJoinedOut(byte aID, wchar_t *aChannelPath);
};