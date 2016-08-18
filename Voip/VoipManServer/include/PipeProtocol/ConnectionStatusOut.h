#pragma once
#include "CommonPipePacketOut.h"

class CConnectionStatusOut : public CCommonPipePacketOut
{
public:
	CConnectionStatusOut(byte aID, unsigned int aErrorCode);
};