#include "stdafx.h"
#include "../../include/RecordServer/Protocol/CopyAudioFileQueryOut.h"

CCopyAudioFileQueryOut::CCopyAudioFileQueryOut( LPCSTR alpcAudioFile, unsigned int aSourceRecordID, unsigned int aDestRecordID)
{
	ATLASSERT( alpcAudioFile);

	USES_CONVERSION;
	GetDataBuffer().addString( A2W(alpcAudioFile));
	GetDataBuffer().addData( &aSourceRecordID, sizeof(aSourceRecordID));
	GetDataBuffer().addData( &aDestRecordID, sizeof(aDestRecordID));
}