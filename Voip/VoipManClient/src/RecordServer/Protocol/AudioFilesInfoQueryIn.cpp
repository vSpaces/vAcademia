#include "stdafx.h"
#include "../../../include/RecordServer/Protocol/AudioFilesInfoQueryIn.h"


CAudioFilesInfoQueryIn::CAudioFilesInfoQueryIn(BYTE *aData, int aDataSize) : 
		CCommonPacketIn(aData, aDataSize)
			, realityId(-1)
			, recordId(-1)
			, MP_VECTOR_INIT(audioFrames)
{
}

bool CAudioFilesInfoQueryIn::Analyse()
{
	if ( !CCommonPacketIn::Analyse())
		return false;

	int idx = 0;

	// реальность
	if ( !memparse_object( pData, idx, realityId))
		return false;

	// запись
	if ( !memparse_object( pData, idx, recordId))
		return false;

	// кол-во файлов
	unsigned int filesCount = 0;
	if ( !memparse_object( pData, idx, filesCount))
		return false;

	// файлы
	for (size_t i = 0; i<filesCount; i++)
	{
		AudioFrame	audioFrame;
		if ( !memparse_string( pData, idx, audioFrame.audioFileName))
			return false;
		if ( !memparse_object( pData, idx, audioFrame.audioFileSize))
			return false;
		if ( !memparse_object( pData, idx, audioFrame.audioStartTime))
			return false;
		audioFrames.push_back( audioFrame);
	}
	return EndOfData( idx);
}

unsigned int	CAudioFilesInfoQueryIn::GetRealityID()
{
	return realityId;
}

unsigned int	CAudioFilesInfoQueryIn::GetRecordID()
{
	return recordId;
}

CAudioFilesInfoQueryIn::VecAudioFrames&	CAudioFilesInfoQueryIn::GetAudioFrames()
{
	return audioFrames;
}