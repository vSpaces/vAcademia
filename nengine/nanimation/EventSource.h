#pragma once

#include <set>

/*
    Event source class
*/

template<class T>
class CEventSource
{

public:
    void Subscribe(T* _sink)
    {
        m_eventSinks.insert(_sink);
    }

    void Unsubscribe(T* _sink)
    {
        m_eventSinks.erase(_sink);
    }

    template<void (T::* _Event) ()>
	void Invoke()
    {
        SinkCollection::iterator i;
        for (i = m_eventSinks.begin(); i != m_eventSinks.end(); ++i)
        {
            ((*i)->*_Event) ();
        }
    }

    template<class _Param, void (T::* _Event) (_Param)>
        void InvokeEx(_Param _par)
    {
        SinkCollection::iterator i;
        for (i = m_eventSinks.begin(); i != m_eventSinks.end(); ++i)
        {
            ((*i)->*_Event) (_par);
        }
    }
private:
    typedef std::set<T*> SinkCollection;

private:
    SinkCollection m_eventSinks;
};
