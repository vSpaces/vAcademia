#pragma once
#include "CommonPipePacketOut.h"

class CLogOut : public CCommonPipePacketOut
{
public:
	CLogOut(byte aID, const char *alpcLog);
};