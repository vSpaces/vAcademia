#pragma once
#include "CommonPipePacketOut.h"

class CCurrentInputLevelOut : public CCommonPipePacketOut
{
public:
	CCurrentInputLevelOut(byte aID, float aCurrentInputLevel);
};