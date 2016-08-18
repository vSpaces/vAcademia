
#pragma once

#include <list>
#include <Assert.h>
#include "ISTLObject.h"
#include "IMemoryProfiler.h"

template <typename T>
class CListMP : public std::list<T>, public ISTLObject
{
public:
	CListMP(IMemoryProfiler* mp, unsigned char dllID, char* fileName, int line, char* funcName):
		m_dllID(dllID),	
		m_mp(mp)	
	{
		m_elementSize = sizeof(T) + 4;
		if ((!m_mp) && (line != -1))
		{
			int ii = 0;
		}
		assert(m_mp || (line == -1));
		if (m_mp)
		{
			m_mp->RegisterSTLObject(this, dllID, fileName, line, funcName);
		}	
	}

	CListMP(std::list<T>& vec, IMemoryProfiler* mp, unsigned char dllID, char* fileName, int line, char* funcName):
		m_dllID(dllID),	
		m_mp(mp)	
	{
		std::list<T>::iterator it = vec.begin();
		std::list<T>::iterator itEnd = vec.end();

		for ( ; it != itEnd; it++)
		{
			push_back(*it);
		}

		m_elementSize = sizeof(T) + 4;
		if ((!m_mp) && (line != -1))
		{
			int ii = 0;
		}
		assert(m_mp || (-1 == line));
		if (m_mp)
		{
			m_mp->RegisterSTLObject(this, dllID, fileName, line, funcName);
		}	
	}

	CListMP& operator =(const std::list<T>& vec)
	{
		clear();
		std::list<T>::const_iterator it = vec.begin();
		std::list<T>::const_iterator itEnd = vec.end();

		for ( ; it != itEnd; it++)
		{
			push_back(*it);
		}

		return *this;
	}

	CListMP& operator =(const CListMP& vec)
	{
		clear();
		std::list<T>::const_iterator it = vec.begin();
		std::list<T>::const_iterator itEnd = vec.end();

		for ( ; it != itEnd; it++)
		{
			push_back(*it);
		}

		return *this;
	}

	CListMP(const CListMP& vec)
	{
		clear();
		std::list<T>::const_iterator it = vec.begin();
		std::list<T>::const_iterator itEnd = vec.end();

		for ( ; it != itEnd; it++)
		{
			push_back(*it);
		}

		m_mp = vec.m_mp;
		m_dllID = vec.m_dllID;
		m_elementSize = sizeof(T) + 4;
		if (m_mp)
		{
			m_mp->ReRegisterSTLObject((ISTLObject *)&vec, m_dllID, this);
		}
	}

	~CListMP()
	{
		if (!m_mp)
		{
			int ii = 0;
		}
		assert(m_mp);
		if (m_mp)
		{	
			m_mp->UnregisterSTLObject(this, m_dllID);
		}
	}

	unsigned int GetSize()
	{
		return size() * m_elementSize;
	}

protected:
	void Init(IMemoryProfiler* mp, unsigned char dllID, char* fileName, int line, char* funcName)
	{
		m_dllID = dllID;
		m_mp = mp;
		m_elementSize = sizeof(T) + 4;

		if (!m_mp)
		{
			int ii = 0;
		}
		assert(m_mp);		
		if (m_mp)
		{
			m_mp->RegisterSTLObject(this, dllID, fileName, line, funcName);
		}	
	}
private:
	unsigned short m_elementSize;
	unsigned char m_dllID;
	IMemoryProfiler* m_mp;
};

// Класс списка с default-конструктором для случаев, 
// когда невозможно вызвать конструктор списка CListMP
template <typename T>
class CListMP_DC : public CListMP<T>
{
public:
	CListMP_DC():
		CListMP<T>(NULL, 0, "non-init", -1, "non-init")
	{
	}

	void Init(IMemoryProfiler* mp, unsigned char dllID, char* fileName, int line, char* funcName)
	{
		CListMP<T>::Init(mp, dllID, fileName, line, funcName);
	}
};