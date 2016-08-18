#ifndef DEVICE_NULL_H
#define DEVICE_NULL_H


#include <list>
#include "audiere.h"
#include "device.h"
#include "internal.h"
#include "threads.h"
#include "types.h"
#include "utility.h"


namespace audiere {

  class NullOutputStream;

  class NullAudioDevice : public AbstractDevice, public Mutex {
  public:
    static NullAudioDevice* create(const ParameterList& parameters);

    NullAudioDevice();

	void destroy() { MP_DELETE_THIS; };

    ~NullAudioDevice();

  public:
    void ADR_CALL update();
    OutputStream* ADR_CALL openStream(SampleSource* source);
    OutputStream* ADR_CALL openBuffer(
      void* samples, int frame_count,
      int channel_count, int sample_rate, SampleFormat sample_format);
    const char* ADR_CALL getName();

  private:
    void removeStream(NullOutputStream* stream, bool bLocking);
    
	typedef MP_LIST<NullOutputStream*> StreamList;
    StreamList m_streams;

    friend class NullOutputStream;
  };

  class NullOutputStream : public RefImplementation<OutputStream> {
  private:
    NullOutputStream(NullAudioDevice* device, SampleSource* source);

	void destroy() { MP_DELETE_THIS; };

    ~NullOutputStream();

  public:
    void ADR_CALL play();

#if AUDIERE_BUILD_H
	void ADR_CALL resume_play();
#endif
    
	void ADR_CALL stop();

#if AUDIERE_RELEASE_P
	void ADR_CALL stop_and_wait(){stop();};
	void ADR_CALL clear( bool bInternal){};
	void ADR_CALL createBuffer(){};
#endif

    void ADR_CALL reset();
    bool ADR_CALL isPlaying();

    void ADR_CALL setRepeat(bool repeat);
    bool ADR_CALL getRepeat();

    void  ADR_CALL setVolume(float volume);
    float ADR_CALL getVolume();
    void  ADR_CALL setPan(float pan);
    float ADR_CALL getPan();
    void  ADR_CALL setPitchShift(float shift);
    float ADR_CALL getPitchShift();

    bool ADR_CALL isSeekable();
    int  ADR_CALL getLength();
    void ADR_CALL setPosition(int position);
    int  ADR_CALL getPosition();
#if AUDIERE_BUILD_H
	int  ADR_CALL getFrequency(){return 0;};
	int  ADR_CALL getSize(){return m_source->getSize();};
	int  ADR_CALL updateFileInfo(int loaded_bytes, bool bLoaded){return m_source->updateFileInfo( loaded_bytes, bLoaded);};
#endif
#if AUDIERE_RELEASE_H
	/**
	* @return  size of header of file
	*/
	int	 ADR_CALL getHeaderSize(){ return m_source->getHeaderSize();};	
#endif

#if AUDIERE_RELEASE_O
	void ADR_CALL setSoundPtrItem(ISoundPtrBase *pSoundPtrBase){};
#endif

  private:
    void doStop(bool internal);
    void resetTimer();
    void update();
    int dummyRead(int samples_to_read);

    RefPtr<NullAudioDevice> m_device;
                          
    SampleSourcePtr m_source;
    int m_channel_count;           //
    int m_sample_rate;             // cached stream format
    SampleFormat m_sample_format;  //

    bool m_is_playing;
    float m_volume;
    float m_pan;
    float m_shift;

    u64 m_last_update;

#if AUDIERE_BUILD_H
	int m_last_source_position;
#endif

    friend class NullAudioDevice;
  };

}


#endif
