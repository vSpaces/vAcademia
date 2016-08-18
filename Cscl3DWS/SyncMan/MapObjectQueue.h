#ifndef MAPOBJECTQUEUE_H__
#define MAPOBJECTQUEUE_H__

#include <queue>
#include <map>

#include "MapObjectStructures.h"

#include "ObjectStateQueue.h"

#define MAX_DISTANCE_PARTS	12

class CMapObjectQueue
{
public:
	CMapObjectQueue();
	virtual ~CMapObjectQueue();

	void SetContext(omsContext* context);
	void SetAvatarPosition(const float x, const float y, char* locationName);

	bool Push(ZONE_OBJECT_INFO* const objectInfo);
	bool Pop(ZONE_OBJECT_INFO*& objectInfo);
	bool IsEmpty()const;
	int	 GetNearestMapObjectCount() const;
	bool IsLeaked()const;

	bool CacheSyncState( syncObjectState* aSyncObjectState);
	bool NextObjectState(ZONE_OBJECT_INFO* aZoneObjectInfo, syncObjectState* &object_props);
	void FreeCachedObjectState(syncObjectState* &object_props);

	bool RemoveObjectInfoByIndex(unsigned int index);
	bool GetObjectInfoByIndex(unsigned int index, ZONE_OBJECT_INFO*& objectInfo);

	void DeleteObject( unsigned int auObjectID, unsigned int auRealityID);
	void DeleteObjectsExceptReality( unsigned int auRealityID);

	ZONE_OBJECT_INFO* NewObjectInfo();
	void DeleteObjectInfo(ZONE_OBJECT_INFO*& objectInfo);

	void SetLogWriter( ILogWriter* aILogWriter)
	{
		m_ILogWriter = aILogWriter;
	}

	void OnLocationsListChanged();

private:
	void AddObjectAccordingToDistance(ZONE_OBJECT_INFO* const objectInfo);

	unsigned int GetObjectCount()const;

	bool GetInternalIndex(unsigned int index, unsigned int& partID, unsigned int& partIndex);

	// Критическая секция на потоково безопасное испозование объекта
	CRITICAL_SECTION m_locker;

	float m_avatarX, m_avatarY;
	float m_oldAvatarX, m_oldAvatarY;

	// Карта созданных описаний объектов
	typedef MP_MAP<ZONE_OBJECT_INFO*, ZONE_OBJECT_INFO*> CMapObjectInfos;
	CMapObjectInfos m_objectInfoHeap;

	// Очередь (перевый пришел - первый вышел) описаний объектов
	typedef MP_VECTOR_DC<ZONE_OBJECT_INFO*> CQueueObjectInfos;
	CQueueObjectInfos m_queueObjectInfos[MAX_DISTANCE_PARTS];

	// Карта идентификаторов объектов, находящихся в очереди
	typedef MP_MAP<ZONE_OBJECT_ID, int> CObjectIDMap;
	CObjectIDMap m_mapObjectID;

	MP_MAP<int, int> m_highPriorityObjectsMap;
	ILogWriter*		m_ILogWriter;

	CObjectStateQueue	m_objectsStateQueue;
	MP_STRING			m_avatarLocation;

	omsContext* m_context;
};
#endif // MAPOBJECTQUEUE_H__