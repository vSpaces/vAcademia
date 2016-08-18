#pragma once
#include "CommonPipePacketOut.h"

class CConfirmationOut : public CCommonPipePacketOut
{
public:
	CConfirmationOut(byte aID);
};