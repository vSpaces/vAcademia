#pragma once
#include "..\RecordServer\Protocol\CommonVoipPacketOut.h"

class CCopyAudioFileQueryOut : public CCommonVoipPacketOut
{
public:
	CCopyAudioFileQueryOut( LPCSTR alpcAudioFile, unsigned int aSourceRecordID, unsigned int aDestRecordID);
};