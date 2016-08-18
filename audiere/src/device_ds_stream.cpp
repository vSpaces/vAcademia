#include "stdafx.h"
#include "device_ds_stream.h"
#include "device_ds.h"
#include "debug.h"
#include "isound.h"

namespace audiere {

  DSOutputStream::DSOutputStream(
    DSAudioDevice* device,
    IDirectSoundBuffer* buffer,
    int buffer_length,
    SampleSource* source, 
	IStreamListener* audioStreamListener)
  {
    ADR_GUARD("DSOutputStream::DSOutputStream");

	setStreamListener((IStreamListener*)audioStreamListener);

    m_device = device;

    m_buffer        = buffer;
    m_buffer_length = buffer_length;
    m_next_read     = 0;
    m_last_play     = 0;

    DWORD frequency;
    m_buffer->GetFrequency(&frequency);
    m_base_frequency = frequency;

    m_is_playing = false;

    m_source = source;

    m_frame_size = GetFrameSize(m_source);

    m_total_read   = 0;
    m_total_played = 0;
    
    m_last_frame = MP_NEW_ARR(BYTE, m_frame_size);

#if AUDIERE_BUILD_H
	m_last_source_position = 0;
#endif

#if AUDIERE_RELEASE_H
	m_pSoundPtrBase = NULL;
#endif

#if AUDIERE_RELEASE_O_FIX
	m_posCoef = 1;
#endif

    setVolume(1);
    setPan(0);

    // fill the buffer with data
    fillStream();
  }


  DSOutputStream::~DSOutputStream() {
    ADR_GUARD("DSOutputStream::~DSOutputStream");

    m_device->removeStream(this, true);
    clear( true);
  }

  void
  DSOutputStream::clear( bool bInternal) {
    lock();
	//if ( !bInternal)
	//{
	//	m_device->removeStream(this, false);
	//}
#if AUDIERE_RELEASE_P_FIX
	m_bufferMutex.lock();
#endif
    // destroy the sound buffer interface
	m_buffer->Release();
	m_buffer = NULL;	
	MP_DELETE_ARR(m_last_frame);
#if AUDIERE_RELEASE_P_FIX
	m_bufferMutex.unlock();
#endif
	unlock();
  }

  void
  DSOutputStream::createBuffer() { 
    lock();	
#if AUDIERE_RELEASE_P_FIX
	m_bufferMutex.lock();
#endif
    m_buffer = m_device->createStreamBuffer( this);
	m_frame_size = GetFrameSize(m_source);		
	m_last_frame = MP_NEW_ARR(BYTE, m_frame_size);
#if AUDIERE_RELEASE_P_FIX
	m_bufferMutex.unlock();
#endif
    unlock();
  }

  void
  DSOutputStream::getFormat(int& channel_count, int& sample_rate, SampleFormat& sample_format) {    
    m_source->getFormat(channel_count, sample_rate, sample_format);
  }

  void
  DSOutputStream::play() {
    ADR_GUARD("DSOutputStream::play");
#if AUDIERE_RELEASE_P_FIX
	m_bufferMutex.lock();
	if ( m_buffer)
	{
#endif
		m_buffer->Play(0, 0, DSBPLAY_LOOPING);
#if AUDIERE_RELEASE_P_FIX
    }
	m_bufferMutex.unlock();
#endif
    m_is_playing = true;
  }

#if AUDIERE_BUILD_H
  void 
  DSOutputStream::resume_play(){
    if ( m_is_playing)
		return;
	ADR_GUARD("DSOutputStream::resume_play");
	setPosition( m_last_source_position);	
#if AUDIERE_RELEASE_P_FIX
	m_bufferMutex.lock();
	if ( m_buffer)
	{
#endif
		m_buffer->Play(0, 0, DSBPLAY_LOOPING);
#if AUDIERE_RELEASE_P_FIX
	}
	m_bufferMutex.unlock();
#endif
	m_is_playing = true;
  }
#endif

  void
  DSOutputStream::stop() {
    ADR_GUARD("DSOutputStream::stop");
    doStop(false);
  }

#if AUDIERE_RELEASE_P
  void
  DSOutputStream::stop_and_wait() {
	  ADR_GUARD("DSOutputStream::stop_and_wait");
	  lock();
	  stop();
	  unlock();
  }
#endif

  bool
  DSOutputStream::isPlaying() {
    return m_is_playing;
  }


  void
  DSOutputStream::reset() {
    ADR_GUARD("DSOutputStream::reset");
    SYNCHRONIZED(this);
    doReset();
  }


  void
  DSOutputStream::setRepeat(bool repeat) {
    SYNCHRONIZED(this);
    m_source->setRepeat(repeat);
    if (!isPlaying()) {
      doReset();
    }
  }


  bool
  DSOutputStream::getRepeat() {
    SYNCHRONIZED(this);
    return m_source->getRepeat();
  }


  void
  DSOutputStream::setVolume(float volume) {
    m_volume = volume;
#if AUDIERE_RELEASE_P_FIX
	m_bufferMutex.lock();
	if ( m_buffer)
	{
#endif
		m_buffer->SetVolume(DSAudioDevice::Volume_AudiereToDirectSound(volume));
#if AUDIERE_RELEASE_P_FIX
	}
	m_bufferMutex.unlock();
#endif
  }


  float
  DSOutputStream::getVolume() {
    return m_volume;
  }


  void
  DSOutputStream::setPan(float pan) {
    m_pan = pan;
#if AUDIERE_RELEASE_P_FIX
	m_bufferMutex.lock();
	if ( m_buffer)
	{
#endif
		m_buffer->SetPan(DSAudioDevice::Pan_AudiereToDirectSound(pan));
#if AUDIERE_RELEASE_P_FIX
	}
	m_bufferMutex.unlock();
#endif
  }


  float
  DSOutputStream::getPan() {
    return m_pan;
  }


  void
  DSOutputStream::setPitchShift(float shift) {
#if AUDIERE_RELEASE_P_FIX
    m_bufferMutex.lock();
	if ( m_buffer)
	{
#endif
		m_buffer->SetFrequency(DWORD(m_base_frequency * shift));
#if AUDIERE_RELEASE_P_FIX
	}
	m_bufferMutex.unlock();
#endif
  }


  float
  DSOutputStream::getPitchShift() {
    DWORD frequency;
#if AUDIERE_RELEASE_P_FIX
	m_bufferMutex.lock();
	if ( m_buffer)
	{
#endif
		m_buffer->GetFrequency(&frequency);
#if AUDIERE_RELEASE_P_FIX
	}
	m_bufferMutex.unlock();
#endif
    return float(frequency) / m_base_frequency;
  }
  

  bool
  DSOutputStream::isSeekable() {
    SYNCHRONIZED(this);
    return m_source->isSeekable();
  }


  int
  DSOutputStream::getLength() {
    SYNCHRONIZED(this);
    return m_source->getLength();
  }

  void  ADR_CALL DSOutputStream::setStreamListener(IStreamListener* aStreamListener){
		m_streamListener = aStreamListener;
	}

	IStreamListener* ADR_CALL DSOutputStream::getStreamListener(){
		return m_streamListener;
	}

#if AUDIERE_BUILD_H
  int
  DSOutputStream::getSize() {
     SYNCHRONIZED(this);
     return m_source->getSize();
  }
#endif

#if AUDIERE_RELEASE_H
  int
  DSOutputStream::getHeaderSize() {
     SYNCHRONIZED(this);
     return m_source->getHeaderSize();
  }
#endif

  void
  DSOutputStream::setPosition(int position) {
    SYNCHRONIZED(this);

    // figure out if we're playing or not
    bool is_playing = isPlaying();

    // if we're playing, stop
    if (is_playing) {
      doStop(true);
    }

    m_last_play = 0;

#if AUDIERE_BUILD_H
	if ( m_last_source_position > position)
		m_last_source_position = position;
#endif

    m_source->setPosition(position / m_posCoef);
    m_total_read   = 0;
    m_total_played = 0;
    m_next_read    = 0;
    fillStream();

    // if we were playing, restart
    if (is_playing) {
      play();
    }
  }


  int
  DSOutputStream::getPosition() {
    SYNCHRONIZED(this);
    int pos = m_posCoef * (m_source->getPosition() - (m_total_read - m_total_played));
    while (pos < 0) {
      pos += m_source->getLength();
    }
    return pos;
  }

  int
  DSOutputStream::getFrequency() { 
    return m_base_frequency;
  }

#if AUDIERE_BUILD_H
  int
  DSOutputStream::updateFileInfo(int loaded_bytes, bool bLoaded) { 
    int status = m_source->updateFileInfo(loaded_bytes, bLoaded);
	if ( status >= 0)
	{
		this->lock();
		updateFrameSize();
		this->unlock();
	}
	return status;
  }
#endif

#if AUDIERE_RELEASE_O
  void
  DSOutputStream::setSoundPtrItem(ISoundPtrBase *pSoundPtrBase){
     m_pSoundPtrBase = pSoundPtrBase;
  }
#endif

  void
  DSOutputStream::doStop(bool internal) {
#if AUDIERE_RELEASE_P_FIX
    m_bufferMutex.lock();
	if ( m_buffer)
	{
#endif
		m_buffer->Stop();
#if AUDIERE_RELEASE_P_FIX
    }
	m_bufferMutex.unlock();
#endif
    if (m_is_playing) {
      m_is_playing = false;

      if (!internal) {
        m_device->fireStopEvent(this, StopEvent::STOP_CALLED);
#if AUDIERE_RELEASE_O
		if ( m_pSoundPtrBase != NULL)
			m_pSoundPtrBase->on_stopped( StopEvent::STOP_CALLED);
#endif
      }
    } else {
      m_is_playing = false;
    }
  }


  void
  DSOutputStream::doReset() {
    // figure out if we're playing or not
    bool is_playing = isPlaying();

    // if we're playing, stop
    if (is_playing) {
      doStop(true);
    }
#if AUDIERE_RELEASE_P_FIX
	m_bufferMutex.lock();
	if ( m_buffer)
	{
#endif
		m_buffer->SetCurrentPosition(0);
#if AUDIERE_RELEASE_P_FIX
	}
	m_bufferMutex.unlock();
#endif

    m_last_play = 0;

    m_source->reset();
    m_total_read   = 0;
    m_total_played = 0;
    m_next_read    = 0;
    fillStream();

    // if we were playing, restart
    if (is_playing) {
      play();
    }
  }

  void
  DSOutputStream::updateFrameSize()
  {
	  int _frame_size = GetFrameSize(m_source);

	  if ( m_frame_size == _frame_size)
		  return;

#if AUDIERE_RELEASE_P_FIX
	  m_bufferMutex.lock();
	  if ( !m_last_frame || !m_buffer)
	  {
		  m_bufferMutex.unlock();
		  return;
	  }
#endif

	  if ( m_last_frame)
	  {		  
		  MP_DELETE_ARR(m_last_frame);
	  }	  

#if AUDIERE_RELEASE_O_FIX
	  if ( m_frame_size != _frame_size)
	  {	
		  m_posCoef = _frame_size / m_frame_size;
		  m_buffer_length = m_buffer_length / m_posCoef;
		  m_total_played = m_total_played / m_posCoef;		  
		  m_total_read = m_total_read / m_posCoef;
		  m_last_play = m_last_play / m_posCoef;
		  m_next_read = m_next_read / m_posCoef;
		//  int pos = m_source->getPosition();		  
		//  m_source->setPosition( pos / m_posCoef);
		  m_frame_size = _frame_size;
	  }	  
#endif

	  m_last_frame = MP_NEW_ARR(BYTE, m_frame_size);

#if AUDIERE_RELEASE_P_FIX
	  m_bufferMutex.unlock();
#endif
  }

  void
  DSOutputStream::fillStream() {
    ADR_GUARD("DSOutputStream::fillStream");

	updateFrameSize();	

    // we know the stream is stopped, so just lock the buffer and fill it

    void* buffer = NULL;
    DWORD buffer_length = 0;


#if AUDIERE_RELEASE_P_FIX
	m_bufferMutex.lock();
	if ( !m_buffer)
	{
		m_bufferMutex.unlock();
		return;
	}
#endif
	// lock
	HRESULT result = m_buffer->Lock(
	0,
	m_buffer_length * m_frame_size,
	&buffer,
	&buffer_length,
	NULL,
	NULL,
	0);
	if (FAILED(result) || !buffer) {
		ADR_LOG("FillStream failed!");
#if AUDIERE_RELEASE_P_FIX	
		m_bufferMutex.unlock();
#endif
		return;
	}

    ADR_IF_DEBUG {
      char str[80];
      sprintf(str, "Buffer Length = %d", buffer_length);
      ADR_LOG(str);
    }

    // fill
    int samples_to_read = buffer_length / m_frame_size;
    int samples_read = streamRead(samples_to_read, buffer);
    if (samples_read != samples_to_read) {
      m_next_read = samples_read;
    } else {
      m_next_read = 0;
    }

	ADR_IF_DEBUG {
      char str[80];
      sprintf(str, "samples_to_read = %d", samples_to_read); ADR_LOG(str);
      sprintf(str, "samples_read    = %d", samples_read);    ADR_LOG(str);
      sprintf(str, "m_next_read     = %d", m_next_read);     ADR_LOG(str);
    }

	//listener
	notifyStreamListener(buffer, buffer_length, NULL, 0);

	// unlock
	m_buffer->Unlock(buffer, buffer_length, NULL, 0);
	m_buffer->SetCurrentPosition(0);
#if AUDIERE_RELEASE_P_FIX
	m_bufferMutex.unlock();
#endif
  }


  void
  DSOutputStream::update() {
#if AUDIERE_RELEASE_O    
    this->lock();
#else
	SYNCHRONIZED(this);
#endif

#if AUDIERE_RELEASE_P
#if AUDIERE_RELEASE_P_FIX
	m_bufferMutex.lock();
#endif
	if ( !m_buffer)
	{
#if AUDIERE_RELEASE_P_FIX
		m_bufferMutex.unlock();
#endif
		this->unlock();
		return;
	}
#endif

    // if it's not playing, don't do anything
    if (!isPlaying()) {
#if AUDIERE_RELEASE_O
#if AUDIERE_RELEASE_P_FIX
		m_bufferMutex.unlock();
#endif
      this->unlock();
#endif
      return;
    }

    ADR_GUARD("DSOutputStream::update");

    /* this method reads more PCM data into the stream if it is required */

    // read the stream's play and write cursors
    DWORD play, write;
    HRESULT result = m_buffer->GetCurrentPosition(&play, &write);
    if (FAILED(result)) {
      ADR_LOG("GetCurrentPosition failed");
#if AUDIERE_RELEASE_O
#if AUDIERE_RELEASE_P_FIX
	  m_bufferMutex.unlock();
#endif
	  this->unlock();
#endif
      return;
    }

    // deal with them in samples, not bytes
    play  /= m_frame_size;
    write /= m_frame_size;

    ADR_IF_DEBUG {
      //char str[160];
      //sprintf(str,
        //"play: %d  write: %d  nextread: %d",
        //play, write, m_next_read);
      //ADR_LOG(str);
    }

    // how many samples have we playen since the last update?
    if (int(play) < m_last_play) {
      m_total_played += play + m_buffer_length - m_last_play;
    } else {
      m_total_played += play - m_last_play;
    }
    m_last_play = play;

    // read from |m_next_read| to |play|
    int read_length = play - m_next_read;
    if (read_length < 0) {
      read_length += m_buffer_length;
    }

    if (read_length == 0) {
#if AUDIERE_RELEASE_O
#if AUDIERE_RELEASE_P_FIX
		m_bufferMutex.unlock();
#endif
		this->unlock();
#endif
      return;
    }

    // lock the buffer
    void* buffer1;
    void* buffer2;
    DWORD buffer1_length;
    DWORD buffer2_length;
    result = m_buffer->Lock(
      m_next_read * m_frame_size,
      read_length * m_frame_size,
      &buffer1, &buffer1_length,
      &buffer2, &buffer2_length,
      0);
    if (FAILED(result)) {
      ADR_LOG("Lock() failed!");
#if AUDIERE_RELEASE_O
#if AUDIERE_RELEASE_P_FIX
	  m_bufferMutex.unlock();
#endif
	  this->unlock();
#endif
      return;
    }

    ADR_IF_DEBUG {
      //char str[160];
      //sprintf(str, "buffer1: %d  buffer2: %d", buffer1_length, buffer2_length);
      //ADR_LOG(str);
    }

    // now actually read samples
	int length1 = buffer1_length / m_frame_size;
    int length2 = buffer2_length / m_frame_size;
    int read = streamRead(length1, buffer1);
    if (buffer2) {
      if (length1 == read) {
        read += streamRead(length2, buffer2);
      } else {
        fillSilence(length2, buffer2);
      }
    }

	//listener
	notifyStreamListener(buffer1, buffer1_length, buffer2, buffer2_length);

    ADR_IF_DEBUG {
      //char str[80];
      //sprintf(str, "read: %d", read);
      //ADR_LOG(str);
    }

    m_next_read = (m_next_read + read) % m_buffer_length;

    // unlock
    m_buffer->Unlock(buffer1, buffer1_length, buffer2, buffer2_length);

  
    // Should we stop?
    if (m_total_played > m_total_read) {
      ADR_LOG("Stopping stream!");

#if AUDIERE_BUILD_H
	  m_last_source_position = m_posCoef * m_source->getPosition();
#endif

      doStop(true);
      m_buffer->SetCurrentPosition(0);
      m_last_play = 0;

      m_source->reset();

      m_total_played = 0;
      m_total_read = 0;
      m_next_read = 0;
      fillStream();

      m_device->fireStopEvent(this, StopEvent::STREAM_ENDED);

#if AUDIERE_RELEASE_O
#if AUDIERE_RELEASE_P_FIX
	  m_bufferMutex.unlock();
#endif
      this->unlock();
	  if ( m_pSoundPtrBase != NULL)
        m_pSoundPtrBase->on_stopped( StopEvent::STREAM_ENDED);
#endif

      return;
    }
    else
	{
#if AUDIERE_RELEASE_O
#if AUDIERE_RELEASE_P_FIX
		m_bufferMutex.unlock();
#endif
      this->unlock();	
	}
#endif
  }

void DSOutputStream::notifyStreamListener(void* aBuffer1, int aLengthBuffer1, void* aBuffer2, int aLengthBuffer2)
{
	HANDLE mutex = OpenMutex(READ_CONTROL, false, "Local\\videoStreamingStarted");
	if (mutex)
	{
		CloseHandle(mutex);
	}
	else
	{
		return;
	}

	if (m_streamListener != NULL)
	{
		int channel_count = 0;
		int sample_rate = 0;
		SampleFormat sample_format;
		m_source->getFormat(channel_count, sample_rate, sample_format);

		BYTE* buff = MP_NEW_ARR(BYTE, aLengthBuffer1 + aLengthBuffer2);
		memcpy(buff, aBuffer1, aLengthBuffer1);
		if (aBuffer2)
			memcpy(buff + aLengthBuffer1, aBuffer2, aLengthBuffer2);

		int length1 = aLengthBuffer1 / m_frame_size;
		int length2 = aLengthBuffer2 / m_frame_size;

		int sampleCnt = 0;
		if (sample_format ==  SF_U8)
			sampleCnt = aLengthBuffer1 + aLengthBuffer2;
		else
			sampleCnt = (aLengthBuffer1 + aLengthBuffer2)/2;
//apply volume
		for (int i=0; i < sampleCnt; i++)
		{
			if (sample_format ==  SF_U8)
				buff[i] = buff[i]*m_volume;
			else
				((short*)buff)[i] = ((short*)buff)[i]*m_volume;
		}

		m_streamListener->onSamplesReceived(buff, length1 + length2, m_frame_size, channel_count);

		MP_DELETE_ARR(buff);
	}
}


  // read as much as possible from the stream source, fill the rest
  // with silence
  int
  DSOutputStream::streamRead(int sample_count, void* samples) {
    ADR_GUARD("streamRead");

    // try to read from the stream
    int samples_read = m_source->read(sample_count, samples);

    ADR_IF_DEBUG {
      //char str[80];
      //sprintf(str, "samples_read = %d\n", samples_read);
      //ADR_LOG(str);
    }

    // remember the last sample
    if (samples_read > 0) {
      memcpy(
        m_last_frame,
        (BYTE*)samples + (samples_read - 1) * m_frame_size,
        m_frame_size);
    }

    // fill the rest with silence
    BYTE* out = (BYTE*)samples + m_frame_size * samples_read;
    int c = sample_count - samples_read;
    fillSilence(c, out);

    m_total_read += samples_read;

    return samples_read;
  }


  void
  DSOutputStream::fillSilence(int sample_count, void* samples) {
    int c = sample_count;
    BYTE* out = (BYTE*)samples;
    while (c--) {
      memcpy(out, m_last_frame, m_frame_size);
      out += m_frame_size;
    }
  }

}
