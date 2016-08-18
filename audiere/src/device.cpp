// *sigh*, looking forward to VS.NET...
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include "stdafx.h"
#include <string>
#include "audiere.h"
#include "debug.h"
#include "device_null.h"
#include "internal.h"
#include "threads.h"

#ifdef _MSC_VER

  #include <windows.h>
  #include <mmsystem.h>
  #include "device_ds.h"
  #include "device_mm.h"

#endif

#ifdef HAVE_OSS
  #include "device_oss.h"
#endif

#ifdef HAVE_AL
  #include "device_al.h"
#endif

#ifdef HAVE_DSOUND
  #include "device_ds.h"
#endif

#ifdef HAVE_WINMM
  #include "device_mm.h"
#endif


namespace audiere {

  AbstractDevice::AbstractDevice():MP_VECTOR_INIT(m_callbacks), MP_VECTOR_INIT(m_events) {
    m_thread_exists = false;
    m_thread_should_die = false;

    bool result = AI_CreateThread(eventThread, this, 2);
    if (!result) {
      ADR_LOG("THREAD CREATION FAILED");
    }
  }

  AbstractDevice::~AbstractDevice() {
    m_thread_should_die = true;

    // Trick the thread into no longer waiting.
    m_events_available.notify();

    while (m_thread_exists) {
      AI_Sleep(50);
    }
  }

  void AbstractDevice::registerCallback(Callback* callback) {
#if VLADIMIR_RELEASE_H
    m_callbacks_mutex.lock();
    m_callbacks.push_back(callback);
    m_callbacks_mutex.unlock();
#else
    m_callbacks.push_back(callback);
#endif
  }

  void AbstractDevice::unregisterCallback(Callback* callback) {
    for (size_t i = 0; i < m_callbacks.size(); ++i) {
      if (m_callbacks[i] == callback) {
#if VLADIMIR_RELEASE_H
		m_callbacks_mutex.lock();
		m_callbacks.erase(m_callbacks.begin() + i);
		m_callbacks_mutex.unlock();
#else
		m_callbacks.erase(m_callbacks.begin() + i);
#endif
        return;
      }
    }
  }

  void AbstractDevice::clearCallbacks() {
#if VLADIMIR_RELEASE_H
    m_callbacks_mutex.lock();
    m_callbacks.clear();
	m_callbacks_mutex.unlock();
#else
    m_callbacks.clear();
#endif
  }

 #if AUDIERE_RELEASE_P
  bool AbstractDevice::GetSoundOutputDevices(std::wstring &soundDevices) {
	return true;
  }

  bool AbstractDevice::ReplaceDevice(std::wstring &sDeviceName) {
	  return true;
  }

  void AbstractDevice::Lock() {
	  
  }
  void AbstractDevice::UnLock() {

  }
#endif

  void AbstractDevice::fireStopEvent(OutputStream* stream, StopEvent::Reason reason) 
  {    
    MP_NEW_P2( event, StopEventImpl, stream, reason);
	m_event_mutex.lock();
    m_events.push_back(event);
    m_event_mutex.unlock();
    m_events_available.notify();
  }

  void AbstractDevice::fireStopEvent(const StopEventPtr& event) {
    
  }

  void AbstractDevice::eventThread(void* arg) {
    ADR_GUARD("AbstractDevice::eventThread[static]");
    ADR_LOG(arg ? "arg is valid" : "arg is not valid");
    
    AbstractDevice* This = static_cast<AbstractDevice*>(arg);
    This->eventThread();
  }

  void AbstractDevice::eventThread() {
    ADR_GUARD("AbstractDevice::eventThread");
    m_thread_exists = true;

	while (!m_thread_should_die) {
		m_event_mutex.lock();
		while (m_events.empty()) {
			m_events_available.wait(m_event_mutex, 0.5f);
			if (m_thread_should_die) {
			break;
			}
		}
		if (m_thread_should_die) {
			m_event_mutex.unlock();
			break;
		}

		// Process the events.
		while (!m_events.empty()) 
		{
			StopEventImpl* event = *m_events.begin();

			processEvent(event);
				
			event->destroy();	

			m_events.erase(m_events.begin());
		}

		m_event_mutex.unlock();
	}

    m_thread_exists = false;
  }

  void AbstractDevice::processEvent(Event* event) {
#if VLADIMIR_RELEASE_H
	if ( event == NULL)
	{
		return;
	}
    m_callbacks_mutex.lock();
#endif
    for (size_t i = 0; i < m_callbacks.size(); ++i) {
      if (event->getType() == m_callbacks[i]->getType()) {
        m_callbacks[i]->call(event);
      }
    }
#if VLADIMIR_RELEASE_H
	m_callbacks_mutex.unlock();
#endif
  }


  ADR_EXPORT(const char*) AdrGetSupportedAudioDevices() {
    return
#ifdef _MSC_VER
      "directsound:DirectSound (high-performance)"  ";"
      "winmm:Windows Multimedia (compatible)"  ";"
#else
#ifdef HAVE_OSS
      "oss:Open Sound System"  ";"
#endif
#ifdef HAVE_DSOUND
      "directsound:DirectSound (high-performance)"  ";"
#endif
#ifdef HAVE_WINMM
      "winmm:Windows Multimedia (compatible)"  ";"
#endif
#ifdef HAVE_AL
      "al:SGI AL"  ";"
#endif
#endif
      "null:Null output (no sound)"  ;
  }


  #define NEED_SEMICOLON do ; while (false)

  #define TRY_GROUP(group_name) {                               \
    AudioDevice* device = DoOpenDevice(group_name, parameters); \
    if (device) {                                               \
      return device;                                            \
    }                                                           \
  } NEED_SEMICOLON

  #define TRY_DEVICE(DeviceType) {                         \
    DeviceType* device = DeviceType::create(parameters);   \
    if (device) {                                          \
      return device;                                       \
    }                                                      \
  } NEED_SEMICOLON


  AudioDevice* DoOpenDevice(
    const std::string& name,
    const ParameterList& parameters)
  {
    ADR_GUARD("DoOpenDevice");

    #ifdef _MSC_VER

      if (name == "" || name == "autodetect") {
        TRY_GROUP("directsound");
        TRY_GROUP("winmm");
        return 0;
      }

      if (name == "directsound") {
        TRY_DEVICE(DSAudioDevice);
        return 0;
      }

      if (name == "winmm") {
        TRY_DEVICE(MMAudioDevice);
        return 0;
      }

      if (name == "null") {
        TRY_DEVICE(NullAudioDevice);
        return 0;
      }

    #else  // not Win32 - assume autoconf UNIX

      if (name == "" || name == "autodetect") {
        // in decreasing order of sound API quality
        TRY_GROUP("al");
        TRY_GROUP("directsound");
        TRY_GROUP("winmm");
        TRY_GROUP("oss");
        return 0;
      }

      #ifdef HAVE_OSS
        if (name == "oss") {
          TRY_DEVICE(OSSAudioDevice);
          return 0;
        }
      #endif

      #ifdef HAVE_DSOUND
        if (name == "directsound") {
          TRY_DEVICE(DSAudioDevice);
          return 0;
        }
      #endif

      #ifdef HAVE_WINMM
        if (name == "winmm") {
          TRY_DEVICE(MMAudioDevice);
          return 0;
        }
      #endif

      #ifdef HAVE_AL
        if (name == "al") {
          TRY_DEVICE(ALAudioDevice);
          return 0;
        }
      #endif

      if (name == "null") {
        TRY_DEVICE(NullAudioDevice);
        return 0;
      }

    #endif

    // no devices
    return 0;
  }


  class ThreadedDevice : public RefImplementation<AudioDevice> {
  public:
    ThreadedDevice(AudioDevice* device) {
      ADR_GUARD("ThreadedDevice::ThreadedDevice");
      if (device) {
        ADR_LOG("Device is valid");
      } else {
        ADR_LOG("Device is not valid");
      }

      m_device = device;
      m_thread_exists = false;
      m_thread_should_die = false;

      /// @todo  what if thread creation fails?
      bool result = AI_CreateThread(threadRoutine, this, 2);
      if (!result) {
        ADR_LOG("THREAD CREATION FAILED");
      }
    }

	void destroy() { MP_DELETE_THIS; };

    ~ThreadedDevice() {
      m_thread_should_die = true;
      while (m_thread_exists) {
        AI_Sleep(50);
      }
    }

    // don't need to update the device...  the thread does it for us
    void ADR_CALL update() {
    }

    OutputStream* ADR_CALL openStream(SampleSource* source) {
      return m_device->openStream(source);
    }

    OutputStream* ADR_CALL openBuffer(
      void* samples, int frame_count,
      int channel_count, int sample_rate, SampleFormat sample_format)
    {
      return m_device->openBuffer(
        samples, frame_count,
        channel_count, sample_rate, sample_format);
    }

    const char* ADR_CALL getName() {
      return m_device->getName();
    }

    void ADR_CALL registerCallback(Callback* callback) {
      m_device->registerCallback(callback);
    }

    void ADR_CALL unregisterCallback(Callback* callback) {
      m_device->unregisterCallback(callback);
    }

    void ADR_CALL clearCallbacks() {
      m_device->clearCallbacks();
    }

#if AUDIERE_RELEASE_P
	bool ADR_CALL GetSoundOutputDevices(std::wstring &soundDevices){ return m_device->GetSoundOutputDevices(soundDevices);};
	bool ADR_CALL IsDeviceChangeNeed( std::wstring &sDeviceName){ return m_device->IsDeviceChangeNeed(sDeviceName);};
	bool ADR_CALL ReplaceDevice( std::wstring &sDeviceName){ return m_device->ReplaceDevice(sDeviceName);};
	void ADR_CALL Lock(){ m_device->Lock();};
	void ADR_CALL UnLock(){ m_device->UnLock();};
#endif

  private:
    void run() {
      ADR_GUARD("ThreadedDevice::run");
      m_thread_exists = true;
      while (!m_thread_should_die) {
        m_device->update();
      }
      m_thread_exists = false;
    }

    static void threadRoutine(void* arg) {
      ADR_GUARD("ThreadedDevice::threadRoutine");
      if (arg) {
        ADR_LOG("arg is valid");
      } else {
        ADR_LOG("arg is not valid");
      }

      ThreadedDevice* This = (ThreadedDevice*)arg;
      This->run();
    }

  private:
    RefPtr<AudioDevice> m_device;
    volatile bool m_thread_should_die;
    volatile bool m_thread_exists;
  };


  ADR_EXPORT(AudioDevice*) AdrOpenDevice(
    const char* name,
    const char* parameters)
  {
    ADR_GUARD("AdrOpenDevice");

    if (!name) {
      name = "";
    }
    if (!parameters) {
      parameters = "";
    }

    // first, we need an unthreaded audio device
    AudioDevice* device = DoOpenDevice(
      std::string(name),
      ParameterList(parameters));
    if (!device) {
      ADR_LOG("Could not open device");
      return 0;
    }

    ADR_LOG("creating threaded device");    
    MP_NEW_P(threadDev, ThreadedDevice, device);
	return threadDev;
  }

  ADR_EXPORT(bool) AdrReplaceDevice(
	  const AudioDevicePtr& device,
	  const std::wstring sDeviceName)
  {
	return true;//device->ReplaceDevice( sDeviceName);
  }

}
