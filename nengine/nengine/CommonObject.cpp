
#include "StdAfx.h"
#include <Assert.h>
#include "CommonObject.h"
#include "cal3d/memory_leak.h"

CCommonObject::CCommonObject():
	MP_WSTRING_INIT(m_name),
	m_isDeleted(false),
	m_id(0xFFFFFFFF),
	m_refCount(0)
{
	for (int i = 0; i < USER_DATA_ITEM_COUNT; i++)
	{
		m_userData[i] = NULL;
	}
}

CCommonObject::CCommonObject(const CCommonObject& a):
	MP_WSTRING_INIT(m_name)
{
	m_name = a.m_name;
}

CCommonObject::~CCommonObject()
{
}