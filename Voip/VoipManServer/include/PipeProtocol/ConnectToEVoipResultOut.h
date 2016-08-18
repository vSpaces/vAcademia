#pragma once
#include "CommonPipePacketOut.h"

class CConnectToEVoipResultOut : public CCommonPipePacketOut
{
public:
	CConnectToEVoipResultOut(byte aID, unsigned int aErrorCode);
};