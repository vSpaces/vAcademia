#pragma once
#include "CommonPacketIn.h"

typedef struct _AudioFrame
{
	std::wstring	audioFileName;
	unsigned int	audioFileSize;
	unsigned int	audioStartTime;

	_AudioFrame()
	{

	}

	_AudioFrame(const _AudioFrame& audioFrame)
	{
		audioFileName = audioFrame.audioFileName;
		audioFileSize = audioFrame.audioFileSize;
		audioStartTime = audioFrame.audioStartTime;
	}
} AudioFrame;

/************************************************************************/
/* Ответ от сервера записи голосовой связи. Данные:
1. PlayMode - редим воспроизведения
/************************************************************************/
class CAudioFilesInfoQueryIn : public Protocol::CCommonPacketIn
{
public:
	CAudioFilesInfoQueryIn( BYTE *aData, int aDataSize);
	virtual bool Analyse();

public:
	typedef std::vector<AudioFrame>	VecAudioFrames;
	unsigned int	GetRealityID();
	unsigned int	GetRecordID();
	VecAudioFrames&	GetAudioFrames();

protected:
	unsigned int	realityId;
	unsigned int	recordId;
	VecAudioFrames	audioFrames;
};
