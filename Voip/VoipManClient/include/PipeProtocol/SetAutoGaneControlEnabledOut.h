#pragma once
#include "CommonPipePacketOut.h"

class CSetAutoGaneControlEnabledOut : public CCommonPipePacketOut
{
public:
	CSetAutoGaneControlEnabledOut(byte aID, bool aEnabled);
};