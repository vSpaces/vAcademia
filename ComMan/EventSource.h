#pragma once

#include <vector>

/*
    Event source class
*/

template<class T>
class CEventSource
{
public:
	CEventSource()
	{
		InitializeCriticalSection(&m_cs);
	}

	~CEventSource()
	{
		DeleteCriticalSection(&m_cs);
	}

    void Subscribe(T* _sink)
    {
		EnterCriticalSection(&m_cs);
		SinkCollection::iterator i;
		for (i = m_eventSinks.begin(); i != m_eventSinks.end(); ++i)
		{
			if (_sink == *i)
			{
				m_eventSinks.erase(i);
				break;
			}
		}
        m_eventSinks.push_back(_sink);
		LeaveCriticalSection(&m_cs);
    }

    void Unsubscribe(T* _sink)
    {
		EnterCriticalSection(&m_cs);
		SinkCollection::iterator i;
		for (i = m_eventSinks.begin(); i != m_eventSinks.end(); ++i)
		{
			if (_sink == *i)
			{
				m_eventSinks.erase(i);
				break;
			}
		}
		LeaveCriticalSection(&m_cs);
    }

    template<void (T::* _Event) ()>
	void Invoke()
    {
		EnterCriticalSection(&m_cs);
        SinkCollection::iterator i;
        for (i = m_eventSinks.begin(); i != m_eventSinks.end(); ++i)
        {
            ((*i)->*_Event) ();
        }
		LeaveCriticalSection(&m_cs);
    }

    template<class _Param, void (T::* _Event) (_Param)>
        void InvokeEx(_Param _par)
    {
		EnterCriticalSection(&m_cs);
        SinkCollection::iterator i;
        for (i = m_eventSinks.begin(); i != m_eventSinks.end(); ++i)
        {
            ((*i)->*_Event) (_par);
        }
		LeaveCriticalSection(&m_cs);
    }

	template<class _Param1, class _Param2, void (T::* _Event) (_Param1, _Param2)>
		void InvokeEx2(_Param1 _par1, _Param2 _par2)
	{
		EnterCriticalSection(&m_cs);
		SinkCollection::iterator i;
		for (i = m_eventSinks.begin(); i != m_eventSinks.end(); ++i)
		{
			((*i)->*_Event) (_par1, _par2);
		}
		LeaveCriticalSection(&m_cs);
	}
private:
    typedef std::vector<T*> SinkCollection;

private:
    SinkCollection m_eventSinks;

	CRITICAL_SECTION m_cs;
};
