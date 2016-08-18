#pragma once
#include "CommonPipePacketOut.h"

class CDisconnectOut : public CCommonPipePacketOut
{
public:
	CDisconnectOut(byte aID);
};