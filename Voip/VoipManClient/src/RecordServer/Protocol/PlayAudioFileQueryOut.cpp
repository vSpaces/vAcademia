#include "stdafx.h"
#include "../../include/RecordServer/Protocol/PlayAudioFileQueryOut.h"

CPlayAudioFileQueryOut::CPlayAudioFileQueryOut(LPCSTR alpcUserName, LPCSTR alpcAudioFile
											   , unsigned int aiStartPosition
											   , unsigned int aiSyncVersion
											   , unsigned int aRecordID, unsigned int aRealityID
											   ,  LPCSTR alpcCommunicationAreaName)
{
	ATLASSERT( alpcUserName);
	ATLASSERT( alpcAudioFile);

	USES_CONVERSION;
	GetDataBuffer().addString( A2W(alpcUserName));
	GetDataBuffer().addString( A2W(alpcAudioFile));
	GetDataBuffer().addData( &aiStartPosition, sizeof(aiStartPosition));
	GetDataBuffer().addData( &aiSyncVersion, sizeof(aiSyncVersion));
	GetDataBuffer().addData( &aRecordID, sizeof(aRecordID));
	GetDataBuffer().addData( &aRealityID, sizeof(aRealityID));
	GetDataBuffer().addString( A2W(alpcCommunicationAreaName));
}