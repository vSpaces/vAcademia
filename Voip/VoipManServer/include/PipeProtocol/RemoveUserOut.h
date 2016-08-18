#pragma once
#include "CommonPipePacketOut.h"

class CRemoveUserOut : public CCommonPipePacketOut
{
public:
	CRemoveUserOut(byte aID, unsigned int aUserID);
};