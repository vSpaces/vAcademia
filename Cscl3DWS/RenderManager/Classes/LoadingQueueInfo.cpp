
#include "StdAfx.h"
#include "LoadingQueueInfo.h"

CLoadingQueueInfo::CLoadingQueueInfo():
	MP_MAP_INIT(m_objects)
{
}

void CLoadingQueueInfo::RegisterObject(unsigned int objectID, char* className)
{
	// пока не знаю, как отсечь большую часть скриптовых, но несинхронизируемых объектов
	if (strcmp(className, "sitplace"))
	{
		return;
	}

	CLoadingObjectMap::iterator it = m_objects.find(objectID);

	if (it == m_objects.end())
	{
		m_objects.insert(CLoadingObjectMap::value_type(objectID, 1));
	}
}

void CLoadingQueueInfo::DeleteObject(unsigned int objectID)
{
	CLoadingObjectMap::iterator it = m_objects.find(objectID);

	if (it != m_objects.end())
	{
		m_objects.erase(it);
	}
}

bool CLoadingQueueInfo::IsObjectInQueue(unsigned int objectID)
{
	CLoadingObjectMap::iterator it = m_objects.find(objectID);

	return (it != m_objects.end());
}

CLoadingQueueInfo::~CLoadingQueueInfo()
{
}