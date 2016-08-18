#pragma once

#include <vector>

/*
    Event source class
*/

template<class T>
class CEventSource
{

public:
    void Subscribe(T* _sink)
    {
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
    }

    void Unsubscribe(T* _sink)
    {
		SinkCollection::iterator i;
		for (i = m_eventSinks.begin(); i != m_eventSinks.end(); ++i)
		{
			if (_sink == *i)
			{
				m_eventSinks.erase(i);
				break;
			}
		}
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
    typedef std::vector<T*> SinkCollection;

private:
    SinkCollection m_eventSinks;
};
