
#pragma once

#include "CommonRenderManagerHeader.h"

#include <map>

class CLoadingQueueInfo
{
public:
	CLoadingQueueInfo();
	~CLoadingQueueInfo();

	void RegisterObject(unsigned int objectID, char* className);
	void DeleteObject(unsigned int objectID);

	bool IsObjectInQueue(unsigned int objectID);

private:
	typedef MP_MAP<unsigned int, int> CLoadingObjectMap;
	CLoadingObjectMap m_objects;
};