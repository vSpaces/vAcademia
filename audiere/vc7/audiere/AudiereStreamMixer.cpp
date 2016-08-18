#include "AudiereStreamMixer.h"


AudiereStreamMixer* AudiereStreamMixer::instance = NULL; 

typedef  *short my_short;

AudiereStreamMixer::AudiereStreamMixer(void)
{
	std::wstring sharedMemoryName = L"vacademiaAudiere";
	MP_NEW_V(m_sharedMemoryStreamSound, SharedMemoryStreamSound, sharedMemoryName);
	m_sharedMemoryStreamSound->CreateCaptureBuffer(SAMLING_RATE / 100 * sizeof(short) * 300);
}

AudiereStreamMixer::~AudiereStreamMixer(void)
{
	MP_DELETE(m_sharedMemoryStreamSound);
}

AudiereStreamMixer* AudiereStreamMixer::getInstance( )
{
	if (!instance)
		instance = new AudiereStreamMixer();

	return instance;
}

void AudiereStreamMixer::onUpdate()
{
	m_streamsLock.lock();

	std::map<IStreamListener*, SamplesBuffer*>::iterator it = m_streams.begin();
	std::map<IStreamListener*, SamplesBuffer*>::iterator end = m_streams.end();

	if (it == end)
	{
		m_streamsLock.unlock();
		return;
	}

	int minSize = 0;
	while (it != end)
	{
		if (it->second != NULL && it->second->data != NULL)
		{
			minSize = it->second->size;
			break;
		}
		it++;
	}
	for ( ; it != end; ++it)
	{
		if (it->second != NULL)
		{
			if (it->second->size < minSize && it->second->data != NULL)
				minSize = it->second->size;
		}
	}

	if (minSize == 0)
	{
		m_streamsLock.unlock();
		return;
	}

	short* output = MP_NEW_ARR(short, minSize);

	// mix
	short** buffers = MP_NEW_ARR(short*, m_streams.size());
	int i = 0;
	for ( it = m_streams.begin(); it != end; it++, i++)
	{
		if (it->second != NULL)
		{
			buffers[i] = (short*)(it->second->data);
			int s = it->second->size;
			s++;
		}
		else
			buffers[i] = NULL;
	}
	CAudioMixer::MixBuffers((const short**)buffers, m_streams.size(), minSize/2, output);

	m_sharedMemoryStreamSound->AddData(output, minSize);

	MP_DELETE_ARR(buffers);
	MP_DELETE_ARR(output);

	for ( it = m_streams.begin(); it != end; it++, i++)
	{
		if (it->second == NULL || it->second->data == NULL)
			continue;
		if (it->second->size == minSize)
		{
			it->second->size = 0;
			MP_DELETE_ARR(it->second->data);
			it->second->data = NULL;
		}
		else
		{
			int newSize = it->second->size - minSize;
			BYTE* pNewData = MP_NEW_ARR(BYTE, newSize);
			memcpy(pNewData, it->second->data + minSize, it->second->size - minSize);
			MP_DELETE_ARR(it->second->data);
			it->second->data = pNewData;
			it->second->size = newSize;
		}
	}
	
	m_streamsLock.unlock();
}

void AudiereStreamMixer::onSamplesReceived(IStreamListener* pStreamListener, void* buffer, unsigned int sample_count)
{
	m_streamsLock.lock();
	int maxBufferSize = 1024*1024*6;

	if (sample_count * SAMPLE_SIZE > maxBufferSize)
	{
		int new_sample_count = maxBufferSize/SAMPLE_SIZE;
		buffer = (char*)buffer + (sample_count - new_sample_count)*SAMPLE_SIZE;
		sample_count = new_sample_count;
	}

	std::map<IStreamListener*, SamplesBuffer*>::iterator it = m_streams.find(pStreamListener);
	if (it == m_streams.end())
		return;

	SamplesBuffer* pBuffer = it->second;
	if (pBuffer == NULL)
	{
		it->second = pBuffer = MP_NEW(SamplesBuffer);
		pBuffer->data = MP_NEW_ARR(BYTE, sample_count * SAMPLE_SIZE);
		pBuffer->size = sample_count * SAMPLE_SIZE;
		memcpy(pBuffer->data, buffer, sample_count * SAMPLE_SIZE);
	}
	else
	{
		int newSize = sample_count * SAMPLE_SIZE + pBuffer->size;
		if (newSize > maxBufferSize)
		{
			if (pBuffer->data != NULL)
			{
				int delta = newSize-maxBufferSize;
				if (delta < pBuffer->size)
				{
					memcpy(pBuffer->data, pBuffer->data+delta, pBuffer->size-delta);
					pBuffer->size = pBuffer->size - delta;
				}
				else
					pBuffer->size = 0;
			}
			newSize = maxBufferSize;
		}
		BYTE* data = MP_NEW_ARR(BYTE, newSize);
		if (pBuffer->data != NULL)
		{
			if (pBuffer->size != 0)
			{
				memcpy(data, pBuffer->data, pBuffer->size);
			}
			memcpy(data + pBuffer->size, buffer, sample_count * SAMPLE_SIZE);
			MP_DELETE_ARR(pBuffer->data);
		}
		else 
		{
			memcpy(data, buffer, sample_count * SAMPLE_SIZE);
		}
		pBuffer->data = data;
		pBuffer->size = sample_count * SAMPLE_SIZE + pBuffer->size;

	}

	m_streamsLock.unlock();
}

void AudiereStreamMixer::registerListener(IStreamListener* pStreamListener)
{
	m_streamsLock.lock();
	if (m_streams.find(pStreamListener) == m_streams.end())
		m_streams[pStreamListener] = NULL;
	m_streamsLock.unlock();
}

void AudiereStreamMixer::unregisterListener(IStreamListener* pStreamListener)
{
	m_streamsLock.lock();
	std::map<IStreamListener*, SamplesBuffer*>::iterator it = m_streams.find(pStreamListener);
	if (it != m_streams.end())
	{
		if (it->second != NULL)
		{
			if (it->second->data != NULL)
				MP_DELETE_ARR(it->second->data);
			MP_DELETE(it->second);
		}
		m_streams.erase(it);
	}
	m_streamsLock.unlock();
}
