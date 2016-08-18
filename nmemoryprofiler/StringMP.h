#pragma once

#include <string>
#include "ISTLObject.h"
#include "IMemoryProfiler.h"

class CStringMP : public std::string, public ISTLObject
{
public:
	CStringMP(IMemoryProfiler* mp, unsigned char dllID, char* fileName, int line, char* funcName):
		m_dllID(dllID),	
		m_mp(mp)
	{		
		assert(m_mp || (line == -1));
		if (m_mp)
		{
			m_mp->RegisterSTLObject(this, dllID, fileName, line, funcName);
		}	
	}

	CStringMP(std::string& str, IMemoryProfiler* mp, unsigned char dllID, char* fileName, int line, char* funcName):
		m_dllID(dllID),	
		m_mp(mp)
	{	
		assert(m_mp || (line == -1));

		if (m_mp)
		{
			m_mp->RegisterSTLObject(this, dllID, fileName, line, funcName);
		}	

		clear();
		insert(0, str);
	}

	CStringMP(const std::string& str, IMemoryProfiler* mp, unsigned char dllID, char* fileName, int line, char* funcName):
		m_dllID(dllID),	
		m_mp(mp)
	{	
		assert(m_mp || (line == -1));
		if (m_mp)
		{
			m_mp->RegisterSTLObject(this, dllID, fileName, line, funcName);
		}	

		clear();
		insert(0, str);
	}

	CStringMP(char* str, IMemoryProfiler* mp, unsigned char dllID, char* fileName, int line, char* funcName):
		m_dllID(dllID),	
		m_mp(mp)
	{		
		assert(m_mp || (line == -1));
		if (m_mp)
		{
			m_mp->RegisterSTLObject(this, dllID, fileName, line, funcName);
		}	

		clear();
		insert(0, str);
	}

	CStringMP(const char* str, IMemoryProfiler* mp, unsigned char dllID, char* fileName, int line, char* funcName):
		m_dllID(dllID),	
		m_mp(mp)
	{		
		assert(m_mp || (line == -1));
		if (m_mp)
		{
			m_mp->RegisterSTLObject(this, dllID, fileName, line, funcName);
		}	

		clear();
		insert(0, str);
	}

	CStringMP(const CStringMP& str)
	{
		clear();
		insert(0, str);	
		m_mp = str.m_mp;
		m_dllID = str.m_dllID;
		if (m_mp)
		{
			m_mp->ReRegisterSTLObject((ISTLObject *)&str, m_dllID, this);
		}
	}

	~CStringMP()
	{
		assert(m_mp);
		if (m_mp)
		{	
			m_mp->UnregisterSTLObject(this, m_dllID);
		}		
	}

	unsigned int GetSize()
	{
		return (unsigned int)capacity() * sizeof(char);
	}

	CStringMP& operator =(const char* str)
	{
		clear();
		insert(0, str);	

		return *this;
	}

	CStringMP& operator =(const std::string& str)
	{
		clear();
		insert(0, str);	

		return *this;
	}

	CStringMP& operator =(const CStringMP& str)
	{
		clear();
		insert(0, str);	
        
		return *this;
	}

	CStringMP& operator +=(const char* str)
	{		
		std::string temp;
		temp.insert(0, str);
		temp.insert(0, *this);
		*this = temp;
		return *this;
	}

	CStringMP& operator +=(const std::string& str)
	{
		std::string temp;
		temp.insert(0, str);
		temp.insert(0, *this);		
		*this = temp;
		return *this;
	}

	CStringMP& operator +=(const CStringMP& str)
	{
		std::string temp;
		temp.insert(0, str);
		temp.insert(0, *this);
		*this = temp;
		return *this;
	}

	void Init(IMemoryProfiler* mp, unsigned char dllID, char* fileName, int line, char* funcName)
	{
		m_dllID = dllID;
		m_mp = mp;
		assert(m_mp);
		if (m_mp)
		{
			m_mp->RegisterSTLObject(this, dllID, fileName, line, funcName);
		}	
	}

private:	
	unsigned char m_dllID;
	IMemoryProfiler* m_mp;
};

// Класс строки с default-конструктором для случаев, 
// когда невозможно вызвать конструктор строки CStringMP
class CStringMP_DC : public CStringMP
{
public:
	CStringMP_DC():
		CStringMP(NULL, 0, "non-init", -1, "non-init")
	{
	}

	void Init(IMemoryProfiler* mp, unsigned char dllID, char* fileName, int line, char* funcName)
	{
		CStringMP::Init(mp, dllID, fileName, line, funcName);
	}

	CStringMP_DC& operator =(const char* str)
	{
		clear();
		insert(0, str);	

		return *this;
	}

	CStringMP_DC& operator =(const std::string& str)
	{
		clear();
		insert(0, str);	

		return *this;
	}

	CStringMP_DC& operator =(const CStringMP& str)
	{
		clear();
		insert(0, str);	

		return *this;
	}

	CStringMP_DC& operator =(const CStringMP_DC& str)
	{
		clear();
		insert(0, str);	

		return *this;
	}

	CStringMP_DC& operator +=(const char* str)
	{
		std::string temp;
		temp.insert(0, str);
		temp.insert(0, *this);
		return *this;
	}

	CStringMP_DC& operator +=(const std::string& str)
	{
		std::string temp;
		temp.insert(0, str);
		temp.insert(0, *this);
		return *this;
	}

	CStringMP_DC& operator +=(const CStringMP& str)
	{
		std::string temp;
		temp.insert(0, str);
		temp.insert(0, *this);
		return *this;
	}

	CStringMP_DC& operator +=(const CStringMP_DC& str)
	{		
		std::string temp;
		temp.insert(0, str);
		temp.insert(0, *this);
		return *this;
	}
};