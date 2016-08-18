#include "AudioStreamListener.h"

AudioStreamListener::AudioStreamListener(int sample_rate)
{
	m_input_sample_rate = sample_rate;
	m_output_sample_rate = 16000;
	m_streamMixer = AudiereStreamMixer::getInstance();
	if (m_streamMixer != NULL)
		m_streamMixer->registerListener(this);

	int error = 0;
	m_speex_state = speex_resampler_init(1, m_input_sample_rate, m_output_sample_rate, 10, &error);
}

AudioStreamListener::~AudioStreamListener(void)
{
	if (m_streamMixer != NULL)
		m_streamMixer->unregisterListener(this);
	m_streamMixer = NULL;

	//speex_bits_destroy(&m_bits);
	//speex_encoder_destroy(&m_state);
}

void AudioStreamListener::onSamplesReceived(void* buffer, unsigned int sample_count, int frame_size, int channel_count)
{
	if (channel_count == 0) 
		return;

	void* pBuffer = NULL;

	int outputSampleSize = sizeof(spx_int16_t);
	int outputChannelCount = 1;

	audiere::SampleFormat format = frame_size / channel_count == 1 ? audiere::SF_U8 : audiere::SF_S16;
	
	unsigned int convertedChannelSampleCnt = sample_count * ((float)outputChannelCount)/((float)channel_count);
	pBuffer = MP_NEW_ARR(spx_int16_t, convertedChannelSampleCnt);

	spx_int16_t* pDest = (spx_int16_t*)pBuffer;
	BYTE* pSourceFrame = (BYTE*)buffer;
	unsigned short sample = 0;

	//memcpy(pBuffer, buffer, convertedChannelSampleCnt*outputSampleSize);
	for (int i = 0; i < convertedChannelSampleCnt/outputChannelCount; ++i)
	{
		BYTE* pSource = pSourceFrame;
		pSourceFrame += frame_size;

		for (int j = 0; j < outputChannelCount; ++j)
		{
			// fill j channel
			if (format == audiere::SF_S16)
			{
				*pDest = *((spx_int16_t*)(pSource));
				if (channel_count > j + 1)
					pSource += 2;
			}
			else
			{
				spx_int16_t sample = *((BYTE*)(pSource));
				*pDest = (spx_int16_t)(sample * 128);
				if (channel_count > j + 1)
					pSource += 1;
			}

			pDest += 1;
		}
	}

	// resample
	spx_int16_t* inBuffer = (spx_int16_t*)pBuffer;
	spx_uint32_t out_sample_count = int(((float)m_output_sample_rate / m_input_sample_rate) * convertedChannelSampleCnt + 0.5f) + 1;
	spx_int16_t* outBuffer = NULL;
	if (m_input_sample_rate == m_output_sample_rate)
	{
		outBuffer = inBuffer;
		out_sample_count = convertedChannelSampleCnt;
	}
	else
	{
		outBuffer = MP_NEW_ARR(spx_int16_t, out_sample_count);

		speex_resampler_process_int(m_speex_state, 0, inBuffer, &convertedChannelSampleCnt, outBuffer, &out_sample_count);
	}

	if ( m_streamMixer != NULL)
		m_streamMixer->onSamplesReceived(this, outBuffer, out_sample_count);

	if (m_input_sample_rate != m_output_sample_rate)
		MP_DELETE_ARR(outBuffer);

	if (pBuffer != NULL)
		MP_DELETE_ARR(pBuffer);
}

void AudioStreamListener::destroy()
{
	MP_DELETE_THIS;
}

