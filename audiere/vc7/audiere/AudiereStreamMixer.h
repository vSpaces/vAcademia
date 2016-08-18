#pragma once

#include "StdAfx.h"
#include "IStreamListener.h"
#include "Mutex.h"
#include "SamplesBuffer.h"
#include "SharedMemoryStreamSound.h"
#include "AudioMixer.h"

#define FRAME_SIZE 4
#define SAMPLE_SIZE 2
#define CHANNEL_COUNT 2

#define SAMLING_RATE 48000

class AudiereStreamMixer
{
public:
	static AudiereStreamMixer* getInstance( );
	~AudiereStreamMixer( );

	void onUpdate();
	void onSamplesReceived(IStreamListener* pStreamListener, void* buffer, unsigned int sample_count);

	void registerListener(IStreamListener* pStreamListener);
	void unregisterListener(IStreamListener* pStreamListener);

private:
	AudiereStreamMixer(void);

	static AudiereStreamMixer* instance;
	std::map<IStreamListener*, SamplesBuffer*>		m_streams;
	CMutex		m_streamsLock;
	SharedMemoryStreamSound* m_sharedMemoryStreamSound;
};
