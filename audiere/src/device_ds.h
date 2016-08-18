#ifndef DEVICE_DS_H
#define DEVICE_DS_H

/*#ifndef _WIN32_WINNT
#   define _WIN32_WINNT 0x400
#endif*/

// disable 'identifier too long' warning
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <list>
#include "audiere.h"
#include "device.h"
#include "internal.h"
#include "threads.h"
#include "utility.h"


namespace audiere {

#if AUDIERE_RELEASE_P
	/*
	Поиск GUID входного устройства
	*/
	struct DEVICE_ENUMERATION_PARAMS
	{
		DEVICE_ENUMERATION_PARAMS():MP_WSTRING_INIT(outDeviceName){}
		//std::wstring	outDeviceName;
		MP_WSTRING		outDeviceName;
		GUID			outDeviceGuid;
	};
	BOOL CALLBACK DirectSoundEnumerationRoutine(LPGUID guid, LPCWSTR desc, LPCWSTR driver, LPVOID user);


	typedef std::vector<DEVICE_ENUMERATION_PARAMS> VecOutDevicesParams;
	typedef VecOutDevicesParams::iterator VecOutDevicesParamsIt;
	/*
	Поиск всех выходных устройства
	*/
	BOOL CALLBACK DirectSoundEnumerateAllRoutine(LPGUID guid, LPCWSTR desc, LPCWSTR driver, LPVOID user);
#endif

  class DSOutputBuffer;
  class DSOutputStream;

  class DSAudioDevice : public AbstractDevice, public Mutex {
  public:
    static DSAudioDevice* create(const ParameterList& parameters);

  private:
    DSAudioDevice(
      bool global_focus,
      int stream_buffer_length,
      int min_buffer_length,
      HWND anonymous_window,
      IDirectSound* direct_sound);

	void destroy() { MP_DELETE_THIS; };

    ~DSAudioDevice();

public:

#if AUDIERE_RELEASE_P
	bool ADR_CALL GetSoundOutputDevices(std::wstring &soundDevices);
	bool ADR_CALL IsDeviceChangeNeed( std::wstring &sDeviceGuid);
	bool ADR_CALL ReplaceDevice( std::wstring &sDeviceGuid);
	void ADR_CALL Lock();
	void ADR_CALL UnLock();
#endif

    void ADR_CALL update();
	IDirectSoundBuffer *ADR_CALL createStreamBuffer(DSOutputStream *snd);
    OutputStream* ADR_CALL openStream(SampleSource* source);
    OutputStream* ADR_CALL openBuffer(
      void* samples, int frame_count,
      int channel_count, int sample_rate, SampleFormat sample_format);
    const char* ADR_CALL getName();

    /**
     * DirectSound treats volumes and pan levels as decibels (exponential
     * growth like the Richter scale).  We want a linear ramp.  Do
     * the conversion!
     */
    static int Volume_AudiereToDirectSound(float volume);
    static int Pan_AudiereToDirectSound(float pan);

  private:    
    typedef MP_LIST<DSOutputStream*> StreamList;    
	typedef MP_LIST<DSOutputBuffer*> BufferList;

    void removeStream(DSOutputStream* stream, bool bLocking);
    void removeBuffer(DSOutputBuffer* buffer);

    IDirectSound* m_direct_sound;
    StreamList    m_open_streams;
    BufferList    m_open_buffers;

    bool m_global_focus;
    int m_buffer_length;     ///< length of streaming buffer in milliseconds
    int m_min_buffer_length; ///< minimum buffer size in frames

    HWND m_anonymous_window;

#if AUDIERE_RELEASE_P
	//std::wstring m_currDeviceName;
	//MP_WSTRING m_currDeviceName;
	MP_WSTRING m_currDeviceGuid;
	//std::wstring m_currDeviceLineName;
	MP_WSTRING m_currDeviceLineName;
	Mutex m_mutex;
	bool m_bAllLocking;
#endif

    friend class DSOutputBuffer;
    friend class DSOutputStream;

	CRITICAL_SECTION m_cs;
  };


}


#endif
