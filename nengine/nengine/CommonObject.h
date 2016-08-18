
#pragma once

#include "CommonEngineHeader.h"

#define USER_DATA_ITEM_COUNT	11

#include <string>

class CCommonObject
{
public:
	CCommonObject();
	CCommonObject(const CCommonObject& a);
	~CCommonObject();

	__forceinline wchar_t* GetName()const
	{
		return (wchar_t*)m_name.c_str();
	}

	__forceinline void SetName(const std::wstring& name)
	{
		m_name = name;
	}

	__forceinline void SetID(const int id)
	{
		m_id = id;
	}

	__forceinline int GetID()const
	{
		return m_id;
	}

	__forceinline void MarkDeleted()
	{
		if (!m_isDeleted)
		{
			m_isDeleted = true;
			m_refCount = 0;
			FreeResources();
		}	
	}

	__forceinline void UnmarkDeleted()
	{
		m_isDeleted = false;
	}

	__forceinline bool IsDeleted()const
	{
		return m_isDeleted;
	}

	__forceinline void SetUserData(const int itemID, void* const userData)
	{
		m_userData[itemID] = userData;
	}

	__forceinline void* GetUserData(const int itemID)
	{
		return m_userData[itemID];
	}

	__forceinline void AddRef(int refDelta = 1)
	{
		m_refCount += refDelta;
	}

	__forceinline void ReleaseRef()
	{
		m_refCount--;

		if (0 == m_refCount)
		{
			m_isDeleted = true;
			FreeResources();
		}
	}

	virtual void FreeResources() 
	{
	}

private:
	MP_WSTRING m_name;

	bool m_isDeleted;

	int m_refCount;
	int m_id;

	void* m_userData[USER_DATA_ITEM_COUNT];
};
