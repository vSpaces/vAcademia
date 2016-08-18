#pragma once
#include "AudioInput.h"
#include "AudioOutput.h"
#include "StreamSound.h"

class AudioInStream : public AudioInput {
private:
	Q_OBJECT
	Q_DISABLE_COPY(AudioInStream)
public:
	AudioInStream(void);
	~AudioInStream(void);
	void run();
};

class AudioOutStream : public AudioOutput {
private:
	Q_OBJECT
	Q_DISABLE_COPY(AudioOutStream)
public:
	AudioOutStream(void);
	~AudioOutStream(void);
	void run();
};
