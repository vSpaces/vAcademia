#ifndef DEVICE_H
#define DEVICE_H


#include <queue>
#include "audiere.h"
#include "threads.h"

// Vladimir
#define VLADIMIR_RELEASE_H 1
// end Vladimir

namespace audiere {
  
  /// Basic StopEvent implementation.
  class StopEventImpl : public RefImplementation<StopEvent> {
  public:
    StopEventImpl(OutputStream* os, Reason reason) {
      m_stream = os;
      m_reason = reason;
    }

	~StopEventImpl()
	{
		int ii = 0;
	}

	void destroy() { MP_DELETE_THIS; };

    OutputStream* ADR_CALL getOutputStream() {
      return m_stream.get();
    }

    Reason ADR_CALL getReason() {
      return m_reason;
    }

  private:
    OutputStreamPtr m_stream;
    Reason m_reason;
  };


  /// Contains default implementation of functionality common to all devices.
  class AbstractDevice : public RefImplementation<AudioDevice> {
  protected:
    AbstractDevice();
    ~AbstractDevice();

  public:
    void ADR_CALL registerCallback(Callback* callback);
    void ADR_CALL unregisterCallback(Callback* callback);
    void ADR_CALL clearCallbacks();
#if AUDIERE_RELEASE_P
	virtual bool ADR_CALL GetSoundOutputDevices(std::wstring &soundDevices);
	virtual bool ADR_CALL IsDeviceChangeNeed( std::wstring &sDeviceName){return true;};
	virtual bool ADR_CALL ReplaceDevice(std::wstring &sDeviceName);
	virtual void ADR_CALL Lock();
	virtual void ADR_CALL UnLock();
#endif

  protected:
    void fireStopEvent(OutputStream* stream, StopEvent::Reason reason);
    void fireStopEvent(const StopEventPtr& event);

  private:
    static void eventThread(void* arg);
    void eventThread();
    void processEvent(Event* event);

    volatile bool m_thread_exists;
    volatile bool m_thread_should_die;

    Mutex m_event_mutex;
	Mutex m_callbacks_mutex;
    CondVar m_events_available;
    //typedef std::queue<EventPtr> EventQueue;
	typedef MP_VECTOR<EventPtr> EventQueue;
    MP_VECTOR<StopEventImpl*> m_events;
    
	MP_VECTOR<CallbackPtr> m_callbacks;
  };

}


#endif
