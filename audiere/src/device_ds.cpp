#include "stdafx.h"
#include <algorithm>
#include <sstream>
#include <math.h>
#include "device_ds.h"
#include "device_ds_stream.h"
#include "device_ds_buffer.h"
#include "debug.h"
#include "utility.h"
#include "AudioStreamListener.h"

#if AUDIERE_RELEASE_P
#include <atlconv.h>
#endif

namespace audiere {

  static const int DEFAULT_BUFFER_LENGTH = 1000;  // one second


  DSAudioDevice*
  DSAudioDevice::create(const ParameterList& parameters) {
    ADR_GUARD("DSAudioDevice::create");

    // parse parameters
    int stream_buffer_length = parameters.getInt("buffer", 0);
    if (stream_buffer_length <= 0) {
      stream_buffer_length = DEFAULT_BUFFER_LENGTH;
    }
    int min_buffer_length = parameters.getInt("min_buffer_size", 0);
    min_buffer_length = std::max(1, min_buffer_length);
    bool global_focus = parameters.getBoolean("global", true);

    // initialize COM
    HRESULT rv = CoInitialize(NULL);
    if (FAILED(rv)) {
      return 0;
    }

    ADR_LOG("COM initialized properly");

    // register anonymous window class
    // don't worry about failure, if it fails, the window creation will fail
    WNDCLASS wc;
    wc.style          = 0;
    wc.lpfnWndProc    = DefWindowProc;
    wc.cbClsExtra     = 0;
    wc.cbWndExtra     = 0;
    wc.hInstance      = GetModuleHandle(NULL);
    wc.hIcon          = NULL;
    wc.hCursor        = NULL;
    wc.hbrBackground  = NULL;
    wc.lpszMenuName   = NULL;
    wc.lpszClassName  = "AudiereHiddenWindow";
    RegisterClass(&wc);

    // create anonymous window
    HWND anonymous_window = CreateWindow(
      "AudiereHiddenWindow", "", WS_POPUP,
      0, 0, 0, 0,
      NULL, NULL, GetModuleHandle(NULL), NULL);
    if (!anonymous_window) {
      return false;
    }

    ADR_LOG("Anonymous window created successfully");

    // create the DirectSound object
    IDirectSound* direct_sound;
    rv = CoCreateInstance(
      CLSID_DirectSound,
      NULL,
      CLSCTX_INPROC_SERVER,
      IID_IDirectSound,
      (void**)&direct_sound);
    if (FAILED(rv) || !direct_sound) {
      DestroyWindow(anonymous_window);
      return 0;
    }

    ADR_LOG("Created DS object");

    LPGUID guid = NULL;
    GUID stack_guid;  // so we can point 'guid' to an object that won't be destroyed

    std::string guid_string = parameters.getValue("device_guid", "");
    if (!guid_string.empty()) {
      if (UuidFromString((unsigned char*)guid_string.c_str(), &stack_guid) == RPC_S_OK) {
        guid = &stack_guid;
      }
    }

    // initialize the DirectSound device
    rv = direct_sound->Initialize(guid);
    if (FAILED(rv)) {
      DestroyWindow(anonymous_window);
      direct_sound->Release();
      return 0;
    }

    ADR_LOG("Initialized DS object");

    // set the cooperative level
    rv = direct_sound->SetCooperativeLevel(anonymous_window, DSSCL_NORMAL);
    if (FAILED(rv)) {
      DestroyWindow(anonymous_window);
      direct_sound->Release();
      return 0;
    }

    ADR_LOG("Set cooperative level");
    
	MP_NEW_P5( dsAudioDev, DSAudioDevice, global_focus, stream_buffer_length, min_buffer_length,
				anonymous_window, direct_sound);
	return dsAudioDev;
  }


  DSAudioDevice::DSAudioDevice(
    bool global_focus,
    int stream_buffer_length,
    int min_buffer_length,
    HWND anonymous_window,
	IDirectSound* direct_sound):MP_LIST_INIT(m_open_streams),MP_LIST_INIT(m_open_buffers),
								MP_WSTRING_INIT(m_currDeviceGuid),/*MP_WSTRING_INIT(m_currDeviceName),*/MP_WSTRING_INIT(m_currDeviceLineName)
  {
    //m_currDeviceName    = L"";
	m_currDeviceGuid    = L"";
    m_global_focus      = global_focus;
    m_buffer_length     = stream_buffer_length;
    m_min_buffer_length = min_buffer_length;
    m_anonymous_window  = anonymous_window;
    m_direct_sound      = direct_sound;
	m_bAllLocking		= false;

	InitializeCriticalSection(&m_cs);
  }


  DSAudioDevice::~DSAudioDevice() {
    ADR_ASSERT(m_open_streams.empty(),
      "DirectSound device should not die with open streams");
    ADR_ASSERT(m_open_buffers.empty(),
      "DirectSound device should not die with open buffers");

    // shut down DirectSound
	EnterCriticalSection(&m_cs);
    if (m_direct_sound) {
      m_direct_sound->Release();
      m_direct_sound = NULL;
    }
	LeaveCriticalSection(&m_cs);

	DeleteCriticalSection(&m_cs);

    // if the anonymous window is open, close it
    if (m_anonymous_window) {
      DestroyWindow(m_anonymous_window);
      m_anonymous_window = NULL;
    }
	m_bAllLocking		= false;

    CoUninitialize();
  }

#if AUDIERE_RELEASE_P

  BOOL CALLBACK DirectSoundEnumerationRoutine(LPGUID guid, LPCWSTR desc, LPCWSTR driver, LPVOID user)
  {
	  DEVICE_ENUMERATION_PARAMS* deviceParams = (DEVICE_ENUMERATION_PARAMS*)user;
	  if( deviceParams->outDeviceName.compare(desc) == 0)
	  {
		  deviceParams->outDeviceGuid = *guid;
		  return FALSE;
	  }
	  return TRUE;
  }


  BOOL CALLBACK DirectSoundEnumerateAllRoutine(LPGUID guid, LPCWSTR desc, LPCWSTR driver, LPVOID user)
  {
	  VecOutDevicesParams* vecDevices = (VecOutDevicesParams*)user;
	  if( desc != NULL)
	  {
		  DEVICE_ENUMERATION_PARAMS	params;
		  params.outDeviceName = desc;
		  if( guid)
			  params.outDeviceGuid = *guid;
		  else
			  params.outDeviceGuid = GUID_NULL;
		  vecDevices->push_back( params);
	  }
	  return TRUE;
  }

#define AUDIO_SUBSYSTEM_XP 1 
#define AUDIO_SUBSYSTEM_VISTA 2

  bool
  DSAudioDevice::GetSoundOutputDevices( std::wstring &soundDevices) {
	  VecOutDevicesParams vecDevices;
	  if( FAILED( DirectSoundEnumerateW( &DirectSoundEnumerateAllRoutine, &vecDevices)))
		  return false;

	  int aiBufferLength = 1024;
	  wchar_t alpcDeviceNames[ 1024];
	  int audioSystem = 0;
	  int bufferIndex = 0;
	  int iCurrentDeviceLineIndex = -1;	  
	  int iSimilarDeviceLineIndex = 0;
	  int index = -1;

	  OSVERSIONINFOEXW ovi;
	  memset(&ovi, 0, sizeof(ovi));
	  ovi.dwOSVersionInfoSize=sizeof(ovi);
	  GetVersionExW(reinterpret_cast<OSVERSIONINFOW *>(&ovi));

	  if ((ovi.dwMajorVersion < 6) || (ovi.dwBuildNumber < 6001))
		  audioSystem = AUDIO_SUBSYSTEM_XP;
	  else
		  audioSystem = AUDIO_SUBSYSTEM_VISTA;

	  _itow( audioSystem,  alpcDeviceNames, 10);
	  bufferIndex += wcslen(  alpcDeviceNames);

	  VecOutDevicesParamsIt it = vecDevices.begin(), end = vecDevices.end();
	  it++;	// первое устройство пропускаем
	  for (; it!=end; it++)
	  {
		  if( bufferIndex!=0 && bufferIndex<aiBufferLength)
			  alpcDeviceNames[bufferIndex++] = L';';

		  int iLen = wcslen(it->outDeviceName.c_str());
		  if( bufferIndex + iLen < aiBufferLength)
			  wcscpy( & alpcDeviceNames[bufferIndex], it->outDeviceName.c_str());
		  bufferIndex += iLen;

		  WCHAR apwcGUID[ 512];
		  apwcGUID[ 0] = 0;
		  int pos =  StringFromGUID2( it->outDeviceGuid, apwcGUID, 512);
		   apwcGUID[ pos] = 0;
		  _wcslwr_s(apwcGUID, pos + 1);		 
		  if( audioSystem == AUDIO_SUBSYSTEM_XP)
		  {			  
			  if( m_currDeviceGuid == apwcGUID)// && currentLine == *m_currDeviceLineName)
				  iCurrentDeviceLineIndex = index;			  
		  }
		  else
		  {
			  index = index + 1;

			  //if(it->outDeviceName == m_currDeviceName)
			  if( m_currDeviceGuid == apwcGUID)
	          {
				  iCurrentDeviceLineIndex = index;
			  }
			  
			  /*if( iCurrentDeviceLineIndex == -1 && iSimilarDeviceLineIndex == -1)
			  {
				  if( wcsstr( it->outDeviceName.c_str(), m_currDeviceName.c_str()) != NULL)
				  {
					  iSimilarDeviceLineIndex = index;
				  }
			  }*/		  
		  }
	  }

	  // если не нашли точного совпадения, возьмем частичное
	  if( iCurrentDeviceLineIndex == -1)
		  iCurrentDeviceLineIndex = iSimilarDeviceLineIndex;
	  
	  // номер текущего устройства
	  if( bufferIndex!=0 && bufferIndex<aiBufferLength)
		  alpcDeviceNames[bufferIndex++] = L';';

	  if( bufferIndex<aiBufferLength - 39)	// чтобы на _itow хватило
	  {		
		  _itow( iCurrentDeviceLineIndex, alpcDeviceNames + bufferIndex, 10);
	  }

	  soundDevices = alpcDeviceNames;

	  return bufferIndex == 0 ? false : true;
  }

  /*bool
  DSAudioDevice::IsDeviceChangeNeed( std::wstring &sDeviceName) {	  
	if (m_currDeviceName.compare( sDeviceName) == 0 && m_currDeviceName.compare( L"") != 0
		|| (m_currDeviceName.compare( L"") == 0
			&& (sDeviceName.compare( L"Устройство по умолчанию") == 0
				|| sDeviceName.compare( L"Default device") == 0)
			)
		)
	{
		return false;
	}
	return true;
  }*/

bool
	DSAudioDevice::IsDeviceChangeNeed( std::wstring &sDeviceGuid) {	  
		if (m_currDeviceGuid.compare( sDeviceGuid) == 0 && m_currDeviceGuid.compare( L"") != 0
			|| (m_currDeviceGuid.compare( L"") == 0
			&& (sDeviceGuid.compare( L"Устройство по умолчанию") == 0
			|| sDeviceGuid.compare( L"Default device") == 0)
			)
			)
		{
			return false;
		}
		return true;
  }

/*  bool
DSAudioDevice::ReplaceDevice( std::wstring &sDeviceName) {
	  if ( m_anonymous_window == NULL)
	  {
		  return false;
	  }	  

	  if (m_currDeviceName.compare( sDeviceName) == 0)
	  {
		  return true;
	  }
	  
	  LPGUID guid = NULL;
	  GUID stack_guid;  // so we can point 'guid' to an object that won't be destroyed

	  if ( sDeviceName.compare( L"Устройство по умолчанию") != 0
		  && sDeviceName.compare( L"Default device") != 0
		  )
	  {
		  VecOutDevicesParams vecDevices;
		  if( FAILED( DirectSoundEnumerateW( &DirectSoundEnumerateAllRoutine, &vecDevices)))
			  return false;

		  VecOutDevicesParamsIt it = vecDevices.begin(), end = vecDevices.end();
		  it++;	// первое устройство пропускаем
		  for (; it!=end; it++)
		  {
			  if ( wcsstr( it->outDeviceName.c_str(), sDeviceName.c_str()))
			  {
				  USES_CONVERSION;
				  stack_guid = it->outDeviceGuid;
				  guid = &stack_guid;
				  break;
			  }
		  }
	  }
	  else
	  {
		  sDeviceName = L"";
	  }
	  	 
	  // shut down DirectSound
	  if (m_direct_sound) {
		  m_direct_sound->Release();
		  m_direct_sound = NULL;
	  }
	  // create the DirectSound object
	  IDirectSound* direct_sound;
	  HRESULT rv = CoCreateInstance(
		  CLSID_DirectSound,
		  NULL,
		  CLSCTX_INPROC_SERVER,
		  IID_IDirectSound,
		  (void**)&direct_sound);
	  if (FAILED(rv) || !direct_sound) {
		  DestroyWindow(m_anonymous_window);
		  return false;
	  }

	  // initialize the DirectSound device
	  rv = direct_sound->Initialize(guid);
	  if (FAILED(rv)) {
		  DestroyWindow(m_anonymous_window);
		  direct_sound->Release();
		  return false;
	  }

	  ADR_LOG("Initialized DS object");

	  // set the cooperative level
	  rv = direct_sound->SetCooperativeLevel(m_anonymous_window, DSSCL_NORMAL);
	  if (FAILED(rv)) {
		  DestroyWindow(m_anonymous_window);
		  direct_sound->Release();
		  return false;
	  }

	  m_currDeviceName = sDeviceName;
	  m_direct_sound = direct_sound;

	  return true;
  }*/

 bool
DSAudioDevice::ReplaceDevice( std::wstring &sDeviceGuid) {
	  if ( m_anonymous_window == NULL)
	  {
		  return false;
	  }	  

	  if (m_currDeviceGuid.compare( sDeviceGuid) == 0)
	  {
		  return true;
	  }
	  
	  LPGUID guid = NULL;
	  GUID stack_guid;  // so we can point 'guid' to an object that won't be destroyed

	  if ( sDeviceGuid.compare( L"Устройство по умолчанию") != 0
		  && sDeviceGuid.compare( L"Default device") != 0
		  )
	  {
		  VecOutDevicesParams vecDevices;
		  if( FAILED( DirectSoundEnumerateW( &DirectSoundEnumerateAllRoutine, &vecDevices)))
			  return false;

		  VecOutDevicesParamsIt it = vecDevices.begin(), end = vecDevices.end();
		  it++;	// первое устройство пропускаем
		  for (; it!=end; it++)
		  {
			  wchar_t apwcGUID[512];			  
			  apwcGUID[ StringFromGUID2( it->outDeviceGuid, apwcGUID, 512)] = 0;
			  if ( wcsstr( apwcGUID, sDeviceGuid.c_str()))
			  {
				  USES_CONVERSION;
				  stack_guid = it->outDeviceGuid;				 
				  break;
			  }
		  }
	  }
	  else
	  {
		  sDeviceGuid = L"";
	  }
	  	 
	  // shut down DirectSound
	  EnterCriticalSection(&m_cs);
	  if (m_direct_sound) {
		  m_direct_sound->Release();
		  m_direct_sound = NULL;
	  }
	  LeaveCriticalSection(&m_cs);
	  // create the DirectSound object
	  IDirectSound* direct_sound;
	  HRESULT rv = CoCreateInstance(
		  CLSID_DirectSound,
		  NULL,
		  CLSCTX_INPROC_SERVER,
		  IID_IDirectSound,
		  (void**)&direct_sound);
	  if (FAILED(rv) || !direct_sound) {
		  DestroyWindow(m_anonymous_window);
		  return false;
	  }

	  // initialize the DirectSound device
	  rv = direct_sound->Initialize(guid);
	  if (FAILED(rv)) {
		  DestroyWindow(m_anonymous_window);
		  direct_sound->Release();
		  return false;
	  }

	  ADR_LOG("Initialized DS object");

	  // set the cooperative level
	  rv = direct_sound->SetCooperativeLevel(m_anonymous_window, DSSCL_NORMAL);
	  if (FAILED(rv)) {
		  DestroyWindow(m_anonymous_window);
		  direct_sound->Release();
		  return false;
	  }

	  //m_currDeviceName = sDeviceName;
	  m_currDeviceGuid = sDeviceGuid;

	  EnterCriticalSection(&m_cs);
	  m_direct_sound = direct_sound;
	  LeaveCriticalSection(&m_cs);

	  return true;
  }

#define SIMPLE_DEVICE_CHANGE 1

  void
  DSAudioDevice::Lock( ) {
    m_mutex.lock();
	m_bAllLocking = true;
	lock();
	m_open_streams.clear();
#if SIMPLE_DEVICE_CHANGE
	unlock();
#endif
  }

  void
  DSAudioDevice::UnLock( ) {
#if SIMPLE_DEVICE_CHANGE
#else
    unlock();
#endif;
    m_bAllLocking = false;
    m_mutex.unlock();
  }
#endif


  void
  DSAudioDevice::update() {
#ifdef _DEBUG
	//ADR_GUARD("DSAudioDevice::update");
#endif

    {
      /* Put the critical section in its own scope so we don't hold the lock
         while sleeping. --MattC */
      ScopedLock waitAndSignal( &m_mutex);

      SYNCHRONIZED(this);

      // enumerate all open streams
      StreamList::iterator i = m_open_streams.begin();
      while (i != m_open_streams.end()) {
        DSOutputStream* s = *i++;
        s->update();		
      }

	  AudiereStreamMixer::getInstance()->onUpdate();

      // enumerate all open buffers
      BufferList::iterator j = m_open_buffers.begin();
      while (j != m_open_buffers.end()) {
        DSOutputBuffer* b = *j++;
        b->update();
      }
    }

    Sleep(50);
  }

  IDirectSoundBuffer*
  DSAudioDevice::createStreamBuffer(DSOutputStream *snd) {
	if (!snd) {
		return NULL;
	}
	
	/*bool bFinded = false;
	StreamList::iterator it = m_open_streams.begin(), itend = m_open_streams.end();
	for ( ; it != itend; it++)
	{
		if ( *it == snd)
		{
			bFinded = true;
		}
	}
	if ( bFinded)
	{
		return NULL;
	}*/

	ADR_GUARD("DSAudioDevice::createStreamBuffer");

	int channel_count, sample_rate;
	SampleFormat sample_format;
	snd->getFormat(channel_count, sample_rate, sample_format);

	const int frame_size = channel_count * GetSampleSize(sample_format);

	// calculate an ideal buffer size
	const int buffer_length = sample_rate * m_buffer_length / 1000;

	// define the wave format
	WAVEFORMATEX wfx;
	memset(&wfx, 0, sizeof(wfx));
	wfx.wFormatTag      = WAVE_FORMAT_PCM;
	wfx.nChannels       = channel_count;
	wfx.nSamplesPerSec  = sample_rate;
	wfx.nAvgBytesPerSec = sample_rate * frame_size;
	wfx.nBlockAlign     = frame_size;
	wfx.wBitsPerSample  = GetSampleSize(sample_format) * 8;
	wfx.cbSize          = sizeof(wfx);

	DSBUFFERDESC dsbd;
	memset(&dsbd, 0, sizeof(dsbd));
	dsbd.dwSize        = sizeof(dsbd);
	dsbd.dwFlags       = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLPAN |
						 DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
	if (m_global_focus) {
		dsbd.dwFlags |= DSBCAPS_GLOBALFOCUS;
	}
	dsbd.dwBufferBytes = frame_size * buffer_length;
	dsbd.lpwfxFormat   = &wfx;

	EnterCriticalSection(&m_cs);

	// create the DirectSound buffer
	IDirectSoundBuffer* buffer = NULL;
	HRESULT result = m_direct_sound->CreateSoundBuffer(&dsbd, &buffer, NULL);
	if (FAILED(result) || !buffer) {
		LeaveCriticalSection(&m_cs);
		return NULL;
	}

	ADR_LOG("createStreamBuffer::CreateSoundBuffer succeeded");
	
	m_open_streams.push_back(snd);

	LeaveCriticalSection(&m_cs);

	return buffer;
}

  OutputStream*
  DSAudioDevice::openStream(SampleSource* source) {
    if (!source) {
      return 0;
    }

	//ScopedLock waitAndSignal( &m_mutex);
    ADR_GUARD("DSAudioDevice::openStream");

    int channel_count, sample_rate;
    SampleFormat sample_format;
    source->getFormat(channel_count, sample_rate, sample_format);

    const int frame_size = channel_count * GetSampleSize(sample_format);

    // calculate an ideal buffer size
    const int buffer_length = sample_rate * m_buffer_length / 1000;

    // define the wave format
    WAVEFORMATEX wfx;
    memset(&wfx, 0, sizeof(wfx));
    wfx.wFormatTag      = WAVE_FORMAT_PCM;
    wfx.nChannels       = channel_count;
    wfx.nSamplesPerSec  = sample_rate;
    wfx.nAvgBytesPerSec = sample_rate * frame_size;
    wfx.nBlockAlign     = frame_size;
    wfx.wBitsPerSample  = GetSampleSize(sample_format) * 8;
    wfx.cbSize          = sizeof(wfx);

    DSBUFFERDESC dsbd;
    memset(&dsbd, 0, sizeof(dsbd));
    dsbd.dwSize        = sizeof(dsbd);
    dsbd.dwFlags       = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLPAN |
                         DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
    if (m_global_focus) {
      dsbd.dwFlags |= DSBCAPS_GLOBALFOCUS;
    }
    dsbd.dwBufferBytes = frame_size * buffer_length;
    dsbd.lpwfxFormat   = &wfx;

	EnterCriticalSection(&m_cs);

    // create the DirectSound buffer
    IDirectSoundBuffer* buffer;
    HRESULT result = m_direct_sound->CreateSoundBuffer(&dsbd, &buffer, NULL);
    if (FAILED(result) || !buffer) {
      //UnLock();
	  LeaveCriticalSection(&m_cs);
      return 0;
    }

    ADR_LOG("CreateSoundBuffer succeeded");

	// create streamListener for video streaming
	MP_NEW_P(audioStreamListener, AudioStreamListener, sample_rate);
    
	// now create the output stream    
	MP_NEW_P5( stream, DSOutputStream, this, buffer, buffer_length, source, (IStreamListener*)audioStreamListener);

    // add it the list of streams and return
    SYNCHRONIZED(this);	
    m_open_streams.push_back(stream);	
	LeaveCriticalSection(&m_cs);

    return stream;
  }


  OutputStream*
  DSAudioDevice::openBuffer(
    void* samples, int frame_count,
    int channel_count, int sample_rate, SampleFormat sample_format)
  {
	//ScopedLock waitAndSignal( &m_mutex);
    ADR_GUARD("DSAudioDevice::openBuffer");

    const int frame_size = channel_count * GetSampleSize(sample_format);

    WAVEFORMATEX wfx;
    memset(&wfx, 0, sizeof(wfx));
    wfx.wFormatTag      = WAVE_FORMAT_PCM;
    wfx.nChannels       = channel_count;
    wfx.nSamplesPerSec  = sample_rate;
    wfx.nAvgBytesPerSec = sample_rate * frame_size;
    wfx.nBlockAlign     = frame_size;
    wfx.wBitsPerSample  = GetSampleSize(sample_format) * 8;
    wfx.cbSize          = sizeof(wfx);

    DSBUFFERDESC dsbd;
    memset(&dsbd, 0, sizeof(dsbd));
    dsbd.dwSize  = sizeof(dsbd);
    dsbd.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLPAN |
                   DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY |
                   DSBCAPS_STATIC | DSBCAPS_CTRLPOSITIONNOTIFY;
    if (m_global_focus) {
      dsbd.dwFlags |= DSBCAPS_GLOBALFOCUS;
    }

    const int buffer_frame_count = std::max(m_min_buffer_length, frame_count);
    const int buffer_size = buffer_frame_count * frame_size;
    dsbd.dwBufferBytes = buffer_size;
    dsbd.lpwfxFormat   = &wfx;

	EnterCriticalSection(&m_cs);

    // create the DS buffer
    IDirectSoundBuffer* buffer;
    HRESULT result = m_direct_sound->CreateSoundBuffer(
      &dsbd, &buffer, NULL);
    if (FAILED(result) || !buffer) {
		LeaveCriticalSection(&m_cs);
      //UnLock();
      return 0;
    }

    ADR_IF_DEBUG {
      DSBCAPS caps;
      caps.dwSize = sizeof(caps);
      result = buffer->GetCaps(&caps);
      if (FAILED(result)) {
        buffer->Release();
		LeaveCriticalSection(&m_cs);
		//UnLock();
        return 0;
      } else {
        std::ostringstream ss;
        ss << "actual buffer size: " << caps.dwBufferBytes << std::endl
           << "buffer_size: " << buffer_size;
        ADR_LOG(ss.str().c_str());
      }
    }

    void* data;
    DWORD data_size;
    result = buffer->Lock(0, buffer_size, &data, &data_size, 0, 0, 0);
    if (FAILED(result)) {	  
      buffer->Release();
	  LeaveCriticalSection(&m_cs);
	  //UnLock();
      return 0;
    }

    ADR_IF_DEBUG {
      std::ostringstream ss;
      ss << "buffer size: " << buffer_size << std::endl
         << "data size:   " << data_size << std::endl
         << "frame count: " << frame_count;
      ADR_LOG(ss.str().c_str());
    }

    const int actual_size = frame_count * frame_size;
    memcpy(data, samples, actual_size);
    memset((u8*)data + actual_size, 0, buffer_size - actual_size);

    buffer->Unlock(data, data_size, 0, 0);
    
	MP_NEW_P4( b, DSOutputBuffer, this, buffer, buffer_frame_count, frame_size);
    SYNCHRONIZED(this);
	//UnLock();
	//lock();
    m_open_buffers.push_back(b);
	LeaveCriticalSection(&m_cs);
	//unlock();
    return b;
  }


  const char* ADR_CALL
  DSAudioDevice::getName() {
    return "directsound";
  }


  void
  DSAudioDevice::removeStream(DSOutputStream* stream, bool bLocking) {
    //if ( bLocking)
	//{
		IStreamListener* streamListener = stream->getStreamListener();
		stream->setStreamListener(NULL);
		if (streamListener)
			streamListener->destroy();

		SYNCHRONIZED(this);
		m_open_streams.remove(stream);
	//}
	//else
	//{
		//m_open_streams.remove(stream);
	//}
  }


  void
  DSAudioDevice::removeBuffer(DSOutputBuffer* buffer) {
    SYNCHRONIZED(this);
    m_open_buffers.remove(buffer);
  }


  int
  DSAudioDevice::Volume_AudiereToDirectSound(float volume) {
    if (volume == 0) {
      return -10000;
    } else {
      double attenuate = 1000 * log(1 / volume);
      return int(-attenuate);
    }
  }


  int
  DSAudioDevice::Pan_AudiereToDirectSound(float pan) {
    if (pan < 0) {
      return -Pan_AudiereToDirectSound(-pan);
    } else {
      return -Volume_AudiereToDirectSound(1 - pan);
    }
  }

}
