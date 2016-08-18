#ifndef MIXER_H
#define MIXER_H


#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#include <list>
#include "audiere.h"
#include "device.h"
#include "resampler.h"
#include "threads.h"
#include "types.h"
#include "utility.h"


namespace audiere {

  class MixerStream;


  /// Always produce 16-bit, stereo audio at the specified rate.
  class MixerDevice : public AbstractDevice, public Mutex {
  public:
    MixerDevice(int rate);

    // update() must be implementated by the specific device to call read()
    // and write the samples to the output device.

    OutputStream* ADR_CALL openStream(SampleSource* source);

    OutputStream* ADR_CALL openBuffer(
      void* samples,
      int frame_count,
      int channel_count,
      int sample_rate,
      SampleFormat sample_format);

  protected:
    int read(int sample_count, void* samples);

  private:    
	MP_LIST<MixerStream*> m_streams;
    int m_rate;

    friend class MixerStream;
  };


  class MixerStream : public RefImplementation<OutputStream> {
  public:
    MixerStream(MixerDevice* device, SampleSource* source, int rate);

	void destroy() { MP_DELETE_THIS; };

    ~MixerStream();

    void  ADR_CALL play();

#if AUDIERE_BUILD_H
	void ADR_CALL resume_play();
#endif

    void  ADR_CALL stop();

#if AUDIERE_RELEASE_P
	void ADR_CALL stop_and_wait(){stop();};
	void ADR_CALL clear( bool bInternal){};
	void ADR_CALL createBuffer(){};
#endif

    bool  ADR_CALL isPlaying();
    void  ADR_CALL reset();

    void  ADR_CALL setRepeat(bool repeat);
    bool  ADR_CALL getRepeat();
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
	int  ADR_CALL getSize(){ return m_source->getSize();};
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
    void read(int frame_count, s16* buffer);

  private:
    RefPtr<MixerDevice> m_device;

    //RefPtr<Resampler> m_source;
	Resampler *m_source;
    s16 m_last_l;
    s16 m_last_r;
    bool m_is_playing;
    int m_volume;
    int m_pan;

    friend class MixerDevice;
  };

}

#endif
