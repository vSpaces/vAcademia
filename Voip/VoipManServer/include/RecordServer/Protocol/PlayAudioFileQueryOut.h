#pragma once
#include "..\RecordServer\Protocol\CommonVoipPacketOut.h"

class CPlayAudioFileQueryOut : public CCommonVoipPacketOut
{
public:
	CPlayAudioFileQueryOut(LPCSTR alpcUserName, LPCSTR alpcAudioFile
							, unsigned int aiStartPosition
							, unsigned int aiSyncVersion
							, unsigned int aRecordID, unsigned int aRealityID);
};