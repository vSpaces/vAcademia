#ifndef DEVICE_DS_BUFFER_H
#define DEVICE_DS_BUFFER_H

/*#ifndef _WIN32_WINNT
#   define _WIN32_WINNT 0x400
#endif*/

#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include "audiere.h"


namespace audiere {

  class DSAudioDevice;

  class DSOutputBuffer : public RefImplementation<OutputStream> {
  public:
    DSOutputBuffer(
      DSAudioDevice* device,
      IDirectSoundBuffer* buffer,
      int length,
      int frame_size);

	void destroy() { MP_DELETE_THIS; };

    ~DSOutputBuffer();

    void ADR_CALL play();
#if AUDIERE_BUILD_H
	void ADR_CALL resume_play();
#endif
    void ADR_CALL stop();
#if AUDIERE_RELEASE_P
	void ADR_CALL stop_and_wait();
	void ADR_CALL clear( bool bInternal){};
	void ADR_CALL createBuffer(){};	
#endif

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
	void ADR_CALL setSoundPtrItem(ISoundPtrBase *pSoundPtrBase){};
#endif

  private:
    void update(); ///< Solely for processing events.

    RefPtr<DSAudioDevice> m_device;
    IDirectSoundBuffer* m_buffer;
    int m_length;
    int m_frame_size;

    int m_base_frequency;

    bool  m_repeating;
    float m_volume;
    float m_pan;

    HANDLE m_stop_event;

    friend class DSAudioDevice;
  };

}


#endif
