#pragma once
#include "CommonPipePacketOut.h"

class CSetEnableDenoisingOut : public CCommonPipePacketOut
{
public:
	CSetEnableDenoisingOut(byte aID, bool aEnabled);
};