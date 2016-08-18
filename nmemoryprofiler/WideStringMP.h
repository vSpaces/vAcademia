#pragma once

#include <string>
#include "ISTLObject.h"
#include "IMemoryProfiler.h"

class CWideStringMP : public std::wstring, public ISTLObject
{
public:
	CWideStringMP(IMemoryProfiler* mp, unsigned char dllID, char* fileName, int line, char* funcName):
		m_dllID(dllID),	
		m_mp(mp)	
	{	
		assert(m_mp || (line == -1));
		if (m_mp)
		{
			m_mp->RegisterSTLObject(this, dllID, fileName, line, funcName);
		}	
	}

	CWideStringMP(std::wstring& str, IMemoryProfiler* mp, unsigned char dllID, char* fileName, int line, char* funcName):
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

	CWideStringMP(const std::wstring& str, IMemoryProfiler* mp, unsigned char dllID, char* fileName, int line, char* funcName):
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

	CWideStringMP(wchar_t* str, IMemoryProfiler* mp, unsigned char dllID, char* fileName, int line, char* funcName):
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

	CWideStringMP(const wchar_t* str, IMemoryProfiler* mp, unsigned char dllID, char* fileName, int line, char* funcName):
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

	CWideStringMP(const CWideStringMP& str)
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

	~CWideStringMP()
	{
		assert(m_mp);
		if (m_mp)
		{	
			m_mp->UnregisterSTLObject(this, m_dllID);
		}
	}

	unsigned int GetSize()
	{
		return capacity() * sizeof(wchar_t);
	}

	CWideStringMP& operator =(const std::wstring& str)
	{
		clear();
		insert(0, str);	

		return *this;
	}

	CWideStringMP& operator =(const wchar_t* str)
	{
		clear();
		insert(0, str);	

		return *this;
	}

	CWideStringMP& operator =(const CWideStringMP& str)
	{
		clear();
		insert(0, str);	

		return *this;
	}

	CWideStringMP& operator +=(const wchar_t* str)
	{		
		std::wstring temp;
		temp.insert(0, str);
		temp.insert(0, *this);
		*this = temp;
		return *this;
	}

	CWideStringMP& operator +=(const std::wstring& str)
	{
		std::wstring temp;
		temp.insert(0, str);
		temp.insert(0, *this);		
		*this = temp;
		return *this;
	}

	CWideStringMP& operator +=(const CWideStringMP& str)
	{
		std::wstring temp;
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
// когда невозможно вызвать конструктор строки CWideStringMP
class CWideStringMP_DC : public CWideStringMP
{
public:
	CWideStringMP_DC():
	  CWideStringMP(NULL, 0, "non-init", -1, "non-init")
	  {
	  }

	  void Init(IMemoryProfiler* mp, unsigned char dllID, char* fileName, int line, char* funcName)
	  {
		  CWideStringMP::Init(mp, dllID, fileName, line, funcName);
	  }

	  CWideStringMP_DC& operator =(const wchar_t* str)
	  {
		  clear();
		  insert(0, str);	

		  return *this;
	  }

	  CWideStringMP_DC& operator =(const std::wstring& str)
	  {
		  clear();
		  insert(0, str);	

		  return *this;
	  }

	  CWideStringMP_DC& operator =(const CWideStringMP& str)
	  {
		  clear();
		  insert(0, str);	

		  return *this;
	  }

	  CWideStringMP_DC& operator =(const CWideStringMP_DC& str)
	  {
		  clear();
		  insert(0, str);	

		  return *this;
	  }

	  CWideStringMP_DC& operator +=(const wchar_t* str)
	  {
		  std::wstring temp;
		  temp.insert(0, str);
		  temp.insert(0, *this);
		  return *this;
	  }

	  CWideStringMP_DC& operator +=(const std::wstring& str)
	  {
		  std::wstring temp;
		  temp.insert(0, str);
		  temp.insert(0, *this);
		  return *this;
	  }

	  CWideStringMP_DC& operator +=(const CWideStringMP& str)
	  {
		  std::wstring temp;
		  temp.insert(0, str);
		  temp.insert(0, *this);
		  return *this;
	  }

	  CWideStringMP_DC& operator +=(const CWideStringMP_DC& str)
	  {		
		  std::wstring temp;
		  temp.insert(0, str);
		  temp.insert(0, *this);
		  return *this;
	  }
};