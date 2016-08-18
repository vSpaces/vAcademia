#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include "stdafx.h"
#include <algorithm>
#include <functional>
#include "device_null.h"
#include "timer.h"
#include "threads.h"
#include "utility.h"

namespace audiere {

  NullAudioDevice*
  NullAudioDevice::create(const ParameterList& /*parameters*/) {    
	return MP_NEW( NullAudioDevice);
  }


  NullAudioDevice::NullAudioDevice():MP_LIST_INIT(m_streams) {
  }


  NullAudioDevice::~NullAudioDevice() {
    ADR_GUARD("~NullAudioDevice");

    ADR_ASSERT(m_streams.size() == 0,
      "Null output context should not die with open streams");
  }


  void
  NullAudioDevice::update() {
    ADR_GUARD("NullAudioDevice::update");
    SYNCHRONIZED(this);

    StreamList::iterator i = m_streams.begin();
    for (; i != m_streams.end(); ++i) {
      (*i)->update();
    }

    AI_Sleep(50);
  }


  OutputStream*
  NullAudioDevice::openStream(SampleSource* source) {
    ADR_GUARD("NullAudioDevice::openStream");
    
    if (!source) {
      return 0;
    }

    SYNCHRONIZED(this);
    
	MP_NEW_P2( stream, NullOutputStream, this, source);
    m_streams.push_back(stream);
    return stream;
  }


  OutputStream*
  NullAudioDevice::openBuffer(
    void* samples, int frame_count,
    int channel_count, int sample_rate, SampleFormat sample_format)
  {
    ADR_GUARD("NullAudioDevice::openBuffer");

    RefPtr<SampleSource> source(OpenBufferStream(
      samples, frame_count,
      channel_count, sample_rate, sample_format));
    return openStream(source.get());
  }


  const char*
  NullAudioDevice::getName() {
    return "null";
  }


  void
  NullAudioDevice::removeStream(NullOutputStream* stream, bool bLocking) {
    SYNCHRONIZED(this);
    m_streams.remove(stream);
  }


  NullOutputStream::NullOutputStream(
    NullAudioDevice* device,
    SampleSource* source)
  : m_device(device)
  , m_source(source)
  , m_is_playing(false)
  , m_volume(1)
  , m_pan(0)
  , m_shift(1)
  , m_last_update(0)
  {
    ADR_GUARD("NullOutputStream::NullOutputStream");
    m_source->getFormat(m_channel_count, m_sample_rate, m_sample_format);
	m_last_source_position = 0;
  }


  NullOutputStream::~NullOutputStream() {
    m_device->removeStream(this, true);
  }


  void
  NullOutputStream::play() {
    ADR_GUARD("NullOutputStream::play");
    m_is_playing = true;
    resetTimer();
  }

#if AUDIERE_BUILD_H
  void 
  NullOutputStream::resume_play(){
	if ( m_is_playing)
		return;
	ADR_GUARD("NullOutputStream::resume_play");
	setPosition( m_last_source_position);		
	m_is_playing = true;
  }
#endif

  void
  NullOutputStream::stop() {
    doStop(false);
  }


  void
  NullOutputStream::reset() {
    SYNCHRONIZED(m_device.get());
    resetTimer();
    m_source->reset();
  }


  bool
  NullOutputStream::isPlaying() {
    return m_is_playing;
  }


  void
  NullOutputStream::setRepeat(bool repeat) {
    SYNCHRONIZED(m_device.get());
    m_source->setRepeat(repeat);
  }


  bool
  NullOutputStream::getRepeat() {
    SYNCHRONIZED(m_device.get());
    return m_source->getRepeat();
  }


  void
  NullOutputStream::setVolume(float volume) {
    m_volume = volume;
  }


  float
  NullOutputStream::getVolume() {
    return m_volume;
  }

  
  void
  NullOutputStream::setPan(float pan) {
    m_pan = pan;
  }

  
  float
  NullOutputStream::getPan() {
    return m_pan;
  }


  void
  NullOutputStream::setPitchShift(float shift) {
    m_shift = shift;
  }


  float
  NullOutputStream::getPitchShift() {
    return m_shift;
  }


  bool
  NullOutputStream::isSeekable() {
    return m_source->isSeekable();
  }


  int
  NullOutputStream::getLength() {
    return m_source->getLength();
  }


  void
  NullOutputStream::setPosition(int position) {
    SYNCHRONIZED(m_device.get());
    m_source->setPosition(position);
    reset();
  }


  int
  NullOutputStream::getPosition() {
    return m_source->getPosition();
  }


  void
  NullOutputStream::doStop(bool internal) {
    if (m_is_playing) {
      m_is_playing = false;
      if (!internal) {
        // let subscribers know that the sound was stopped
        m_device->fireStopEvent(this, StopEvent::STOP_CALLED);
      }
    } else {
#if AUDIERE_BUILD_H
	  m_last_source_position = m_source->getPosition();
#endif
      m_is_playing = false;
    }
  }


  void
  NullOutputStream::resetTimer() {
    m_last_update = GetNow();
  }


  void
  NullOutputStream::update() {
    ADR_GUARD("NullOutputStream::update");

    if (m_is_playing) {
      ADR_LOG("Null output stream is playing");

      // get number of microseconds elapsed since last playing update
      // so we can read that much time worth of samples
      u64 now = GetNow();
      u64 elapsed = now - m_last_update;

      double shifted_time = m_shift * s64(elapsed) / 1000000.0;  // in seconds
      int samples_to_read = int(m_sample_rate * shifted_time);

      ADR_IF_DEBUG {
        //char str[100];
        //sprintf(str, "Samples to read: %d", samples_to_read);
        //ADR_LOG(str);
      }

      int samples_read = dummyRead(samples_to_read);

      if (samples_read != samples_to_read) {
        ADR_LOG("Stopping null output stream");
        m_source->reset();
        doStop(true);
        m_device->fireStopEvent(this, StopEvent::STREAM_ENDED);
      }

      m_last_update = now;
    }
  }


  int
  NullOutputStream::dummyRead(int samples_to_read) {
    int total = 0;  // number of samples read so far

    const int bytes_per_sample = GetSampleSize(m_sample_format);

    // read samples into dummy buffer, counting the number we actually read    
	u8* dummy = MP_NEW_ARR(u8, 1024 * m_channel_count * bytes_per_sample);
    while (samples_to_read > 0) {
      int read = std::min(1024, samples_to_read);
      int actual_read = m_source->read(read, dummy);
      total += actual_read;
      samples_to_read -= actual_read;
      if (actual_read < read) {
        break;
      }
    }
    
	MP_DELETE_ARR_UNSAFE( dummy);
    return total;
  }

}
