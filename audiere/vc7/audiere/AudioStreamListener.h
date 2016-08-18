#pragma once

#include "stdafx.h"
#include "AudiereStreamMixer.h"
#include "IStreamListener.h"
#include "audiere.h"
#include "speex.h"
#include "speex_resampler.h"

class AudioStreamListener: public IStreamListener
{
public:
	AudioStreamListener(int sample_rate);
	~AudioStreamListener(void);

	virtual void onSamplesReceived(void* buffer, unsigned int sample_count, int frame_size, int channel_count);
	void destroy();

private:
	AudiereStreamMixer* m_streamMixer;
	//SpeexBits m_bits;
	SpeexResamplerState* m_speex_state;
	int m_frame_size;
	int m_input_sample_rate;
	int m_output_sample_rate;

};
