#pragma once
#include "CommonPipePacketOut.h"

class CChannelLeftOut : public CCommonPipePacketOut
{
public:
	CChannelLeftOut(byte aID);
};