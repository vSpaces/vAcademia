#pragma once
#include "..\RecordServer\Protocol\CommonVoipPacketOut.h"

class CStartPlayingQueryOut : public CCommonVoipPacketOut
{
public:
	CStartPlayingQueryOut(unsigned int aRecordID, unsigned int aRealityID);
};

class CServerAudioCommandQueryOut : public CCommonVoipPacketOut
{
public:
	typedef enum { SAS_PAUSE = 0, SAS_RESUME, SAS_REWIND} SERVER_AUDIO_COMMAND;
	CServerAudioCommandQueryOut(unsigned int aRecordID, unsigned int aRealityID, SERVER_AUDIO_COMMAND aCommand)
	{
		GetDataBuffer().addData( &aRecordID, sizeof(aRecordID));
		GetDataBuffer().addData( &aRealityID, sizeof(aRealityID));
		unsigned int mode = (unsigned int)aCommand;
		GetDataBuffer().addData( &aCommand, sizeof(aCommand));
	}
};