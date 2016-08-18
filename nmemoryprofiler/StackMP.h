
#pragma once

#include <stack>
#include <Assert.h>
#include "ISTLObject.h"
#include "IMemoryProfiler.h"

template <typename T>
class CStackMP : public std::stack<T>, public ISTLObject
{
public:
	CStackMP(IMemoryProfiler* mp, unsigned char dllID, char* fileName, int line, char* funcName):
		m_dllID(dllID),	
		m_mp(mp)	
	{
		m_elementSize = sizeof(T);
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

	CStackMP(std::stack<T>& vec, IMemoryProfiler* mp, unsigned char dllID, char* fileName, int line, char* funcName):
		m_dllID(dllID),	
		m_mp(mp)	
	{
		std::stack<T>::iterator it = vec.begin();
		std::stack<T>::iterator itEnd = vec.end();

		for ( ; it != itEnd; it++)
		{
			push_back(*it);
		}

		m_elementSize = sizeof(T);
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

	CStackMP& operator =(const std::stack<T>& vec)
	{
		clear();
		std::stack<T>::const_iterator it = vec.begin();
		std::stack<T>::const_iterator itEnd = vec.end();

		for ( ; it != itEnd; it++)
		{
			push_back(*it);
		}

		return *this;
	}

	CStackMP& operator =(const CStackMP& vec)
	{
		clear();
		std::stack<T>::const_iterator it = vec.begin();
		std::stack<T>::const_iterator itEnd = vec.end();

		for ( ; it != itEnd; it++)
		{
			push_back(*it);
		}

		return *this;
	}

	CStackMP(const CStackMP& vec)
	{
		clear();
		std::stack<T>::const_iterator it = vec.begin();
		std::stack<T>::const_iterator itEnd = vec.end();

		for ( ; it != itEnd; it++)
		{
			push_back(*it);
		}

		m_mp = vec.m_mp;
		m_dllID = vec.m_dllID;
		m_elementSize = sizeof(T);
		if (m_mp)
		{
			m_mp->ReRegisterSTLObject((ISTLObject *)&vec, m_dllID, this);
		}
	}

	~CStackMP()
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
		m_elementSize = sizeof(T);

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

// Класс стека с default-конструктором для случаев, 
// когда невозможно вызвать конструктор вектора CStackMP
template <typename T>
class CStackMP_DC : public CStackMP<T>
{
public:
	CStackMP_DC():
		CStackMP<T>(NULL, 0, "non-init", -1, "non-init")
	{
	}

	void Init(IMemoryProfiler* mp, unsigned char dllID, char* fileName, int line, char* funcName)
	{
		CStackMP<T>::Init(mp, dllID, fileName, line, funcName);
	}
};
