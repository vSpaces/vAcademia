#pragma once
#include "..\RecordServer\Protocol\CommonVoipPacketOut.h"

class CRemoveUserAudioQueryOut  : public CCommonVoipPacketOut
{
public:
	CRemoveUserAudioQueryOut(LPCSTR alpcUserName, unsigned int aRecordID, unsigned int aRealityID);
};