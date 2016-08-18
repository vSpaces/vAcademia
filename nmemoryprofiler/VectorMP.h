
#pragma once

#include <vector>
#include <Assert.h>
#include "ISTLObject.h"
#include "IMemoryProfiler.h"

template <typename T>
class CVectorMP : public std::vector<T>, public ISTLObject
{
public:
	CVectorMP(IMemoryProfiler* mp, unsigned char dllID, char* fileName, int line, char* funcName):
		m_dllID(dllID),	
		m_mp(mp)	
	{
		m_elementSize = sizeof(T);

		assert(m_mp || (line == -1));
		if (m_mp)
		{
			m_mp->RegisterSTLObject(this, dllID, fileName, line, funcName);
		}	
	}

	CVectorMP(std::vector<T>& vec, IMemoryProfiler* mp, unsigned char dllID, char* fileName, int line, char* funcName):
		m_dllID(dllID),	
		m_mp(mp)	
	{
		std::vector<T>::iterator it = vec.begin();
		std::vector<T>::iterator itEnd = vec.end();

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

	CVectorMP& operator =(const std::vector<T>& vec)
	{
		clear();
		std::vector<T>::const_iterator it = vec.begin();
		std::vector<T>::const_iterator itEnd = vec.end();

		for ( ; it != itEnd; it++)
		{
			push_back(*it);
		}

		return *this;
	}

	CVectorMP& operator =(const CVectorMP& vec)
	{
		clear();
		std::vector<T>::const_iterator it = vec.begin();
		std::vector<T>::const_iterator itEnd = vec.end();

		for ( ; it != itEnd; it++)
		{
			push_back(*it);
		}

		return *this;
	}

	CVectorMP(const CVectorMP& vec)
	{
		clear();
		std::vector<T>::const_iterator it = vec.begin();
		std::vector<T>::const_iterator itEnd = vec.end();

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

	~CVectorMP()
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
		return capacity() * m_elementSize;
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

//  ласс вектора с default-конструктором дл€ случаев, 
// когда невозможно вызвать конструктор вектора CVectorMP
template <typename T>
class CVectorMP_DC : public CVectorMP<T>
{
public:
	CVectorMP_DC():
		CVectorMP<T>(NULL, 0, "non-init", -1, "non-init")
	{
	}

	void Init(IMemoryProfiler* mp, unsigned char dllID, char* fileName, int line, char* funcName)
	{
		CVectorMP<T>::Init(mp, dllID, fileName, line, funcName);
	}
};
