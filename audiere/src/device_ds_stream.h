#ifndef DEVICE_DS_STREAM_H
#define DEVICE_DS_STREAM_H

/*#ifndef _WIN32_WINNT
#   define _WIN32_WINNT 0x400
#endif*/

#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include "audiere.h"
#include "threads.h"
#include "utility.h"
#include "IStreamListener.h"

struct ISoundPtrBase;

namespace audiere {

  class DSAudioDevice;

  class DSOutputStream
    : public RefImplementation<OutputStream>
    , public Mutex
  {
  public:
    DSOutputStream(
      DSAudioDevice* device,
      IDirectSoundBuffer* buffer,
      int buffer_length, // in frames
      SampleSource* source, 
	  IStreamListener* audioStreamListener);

	void destroy() { MP_DELETE_THIS; };

    ~DSOutputStream();

#if AUDIERE_RELEASE_P
	void ADR_CALL clear( bool bInternal);
	void ADR_CALL createBuffer();
	void ADR_CALL getFormat(int& channel_count, int& sample_rate, SampleFormat& sample_format);
#endif

    void ADR_CALL play();
#if AUDIERE_BUILD_H
	void ADR_CALL resume_play();
#endif
    void ADR_CALL stop();
	void ADR_CALL stop_and_wait();
    bool ADR_CALL isPlaying();
    void ADR_CALL reset();

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

	void  ADR_CALL setStreamListener(IStreamListener* aStreamListener);
	IStreamListener* ADR_CALL getStreamListener();

#if AUDIERE_BUILD_H
	int  ADR_CALL getSize();
#endif

#if AUDIERE_RELEASE_H
	/**
	* @return  size of header of file
	*/
	int	 ADR_CALL getHeaderSize();	
#endif

    void ADR_CALL setPosition(int position);
    int  ADR_CALL getPosition();
	int  ADR_CALL getFrequency();
#if AUDIERE_BUILD_H
	int  ADR_CALL updateFileInfo(int loaded_bytes, bool bLoaded);
#endif

#if AUDIERE_RELEASE_O
	 void ADR_CALL setSoundPtrItem(ISoundPtrBase *pSoundPtrBase);
#endif

  private:
    void doStop(bool internal);   ///< differentiates between internal and external calls
    void doReset();  ///< thread-unsafe version of reset()
    void fillStream();
    void update();
    int streamRead(int samples_to_read, void* buffer);
    void fillSilence(int sample_count, void* buffer);
	void updateFrameSize();

	void notifyStreamListener(void* aBuffer1, int aLengthBuffer1, void* aBuffer2, int aLengthBuffer2);
  
  private:
    RefPtr<DSAudioDevice> m_device;

    IDirectSoundBuffer* m_buffer;
    int m_buffer_length;  // in samples
    int m_next_read;  // offset (in frames) where we will read next
    int m_last_play;  // offset (in frames) where the play cursor was
    int m_base_frequency;  // in Hz

    bool m_is_playing;

    SampleSourcePtr m_source;
    int m_frame_size;  // convenience: bytes per sample * channel count

    int m_total_read;    // total number of frames read from the stream
    int m_total_played;  // total number of frames played 

    float m_volume;
    float m_pan;
    ::BYTE* m_last_frame; // the last frame read (used for clickless silence)

	IStreamListener* m_streamListener;

#if AUDIERE_BUILD_H
	int m_last_source_position;
#endif

#if AUDIERE_RELEASE_O
	ISoundPtrBase *m_pSoundPtrBase;
#endif

#if AUDIERE_RELEASE_O_FIX
	int m_posCoef;
#endif

#if AUDIERE_RELEASE_P_FIX
	Mutex m_bufferMutex;
#endif

    friend class DSAudioDevice;
  };

}


#endif
