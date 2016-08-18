#pragma once
#include "CommonPipePacketOut.h"

class CGetMyUserIDOut : public CCommonPipePacketOut
{
public:
	CGetMyUserIDOut(byte aID, int aUserID, bool abInitVoipSystem);
};