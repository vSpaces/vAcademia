
#include "StdAfx.h"
#include "ActiveObjects.h"

CActiveObjects::CActiveObjects():
	m_teleportIndex(0xFFFFFFFF),
	MP_VECTOR_INIT(m_activeObjects)
{
	InitializeCriticalSection(&m_cs);
}

unsigned int CActiveObjects::Count()
{
	EnterCriticalSection(&m_cs);
	unsigned int count = m_activeObjects.size();
	LeaveCriticalSection(&m_cs);

	return count;
}

void CActiveObjects::Erase(int index)
{
	EnterCriticalSection(&m_cs);
	if (index < m_activeObjects.size())
	{
		m_activeObjects.erase(m_activeObjects.begin() + index);
	}
	LeaveCriticalSection(&m_cs);
}

void CActiveObjects::Deactivate()
{
	for (unsigned int objectIndex = 0; objectIndex < Count(); objectIndex ++)
	{
		C3DObject* obj = GetObject(objectIndex);
		obj->SetBoundingBoxVisible(false);
	}
	Clear();

	m_teleportIndex = 0xFFFFFFFF;
}

void CActiveObjects::Deactivate(C3DObject* obj)
{
	for (unsigned int objectIndex = 0; objectIndex < Count(); objectIndex ++)
	{
		C3DObject* activeObj = GetObject(objectIndex);
		if (activeObj == obj)
		{
			activeObj->SetBoundingBoxVisible(false);
			Erase(objectIndex);
			return;
		}		
	}
}

void CActiveObjects::AddObject(C3DObject* obj)
{
	if (m_teleportIndex == 0xFFFFFFFF)
	{
		m_teleportIndex = 0;
	}

	EnterCriticalSection(&m_cs);
	m_activeObjects.push_back(obj);
	LeaveCriticalSection(&m_cs);
}

C3DObject* CActiveObjects::GetObject(int index)
{
	C3DObject* obj = NULL;

	EnterCriticalSection(&m_cs);
	if (index < m_activeObjects.size())
	{
		obj = m_activeObjects[index];
	}
	LeaveCriticalSection(&m_cs);

	return obj;
}

C3DObject* CActiveObjects::GetNextTeleportObject()
{
	C3DObject* object = NULL;

	if (m_teleportIndex != 0xFFFFFFFF)
	{
		object = GetObject(m_teleportIndex);
		m_teleportIndex++;
		if (m_teleportIndex >= Count())
		{
			m_teleportIndex = 0;
		}
	}

	return object;
}

void CActiveObjects::Clear()
{
	EnterCriticalSection(&m_cs);
	m_activeObjects.clear();
	LeaveCriticalSection(&m_cs);
}

std::vector<C3DObject*> CActiveObjects::GetObjects()
{
	EnterCriticalSection(&m_cs);
	std::vector<C3DObject*> objects = m_activeObjects;
	LeaveCriticalSection(&m_cs);	
	return objects;
}

bool CActiveObjects::IsActive(C3DObject* obj)
{
	for (unsigned int objectIndex = 0; objectIndex < Count(); objectIndex ++)
	{
		C3DObject* activeObj = GetObject(objectIndex);
		if (obj == activeObj)
		{
			return true;
		}
	}
	return false;
}

CActiveObjects::~CActiveObjects()
{
	DeleteCriticalSection(&m_cs);
}