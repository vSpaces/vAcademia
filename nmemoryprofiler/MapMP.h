#pragma once

#include <vector>
#include <Assert.h>
#include "ISTLObject.h"
#include "IMemoryProfiler.h"

template <typename T1, typename T2>
class CMapMP : public std::map<T1, T2>, public ISTLObject
{
public:
	CMapMP(IMemoryProfiler* mp, unsigned char dllID, char* fileName, int line, char* funcName):
		m_dllID(dllID),	
		m_mp(mp)	
	{
		m_elementSize = (sizeof(T1) + sizeof(T2)) * 3;
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

	void Init(IMemoryProfiler* mp, unsigned char dllID, char* fileName, int line, char* funcName)
	{
		m_dllID = dllID;
		m_mp = mp;
		m_elementSize = (sizeof(T1) + sizeof(T2)) * 3;
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

	CMapMP(const CMapMP& map)
	{
		clear();
		std::map<T1, T2>::const_iterator it = map.begin();
		std::map<T1, T2>::const_iterator itEnd = map.end();

		for ( ; it != itEnd; it++)
		{
			insert(std::map<T1, T2>::value_type((*it).first, (*it).second));
		}

		m_mp = map.m_mp;
		m_dllID = map.m_dllID;
		m_elementSize = map.m_elementSize;
		if (m_mp)
		{
			m_mp->ReRegisterSTLObject((ISTLObject *)&map, m_dllID, this);
		}
	}

	~CMapMP()
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

private:
	unsigned short m_elementSize;
	unsigned char m_dllID;
	IMemoryProfiler* m_mp;
};

// Класс мапкм с default-конструктором для случаев, 
// когда невозможно вызвать конструктор мапы CMapMP
template <typename T1, typename T2>
class CMapMP_DC : public CMapMP<T1, T2>
{
public:
	CMapMP_DC():
		CMapMP<T1, T2>(NULL, 0, "non-init", -1, "non-init")
	{
	}

	void Init(IMemoryProfiler* mp, unsigned char dllID, char* fileName, int line, char* funcName)
	{
		CMapMP<T1, T2>::Init(mp, dllID, fileName, line, funcName);
	}
};