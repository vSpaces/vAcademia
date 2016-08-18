#pragma once
#include "CommonVoipPacketOut.h"

class CGetVoipServerForRecordQueryOut : public CCommonVoipPacketOut
{
public:
	CGetVoipServerForRecordQueryOut( unsigned int aiPlayRealityID,
									unsigned int aiPlayingRecordID);
};