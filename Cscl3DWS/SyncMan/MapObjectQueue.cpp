
#include "StdAfx.h"
#include <Math.h>
#include "MapObjectQueue.h"
#include "ILogWriter.h"
#include "rmml.h"

#define MAX_RESERVED_OBJECTS_IN_PART	1000
#define MIN_DIST_CHANGE_FOR_REPROCESS	2000.0f
#define DISTANCE_RANGE_LENGTH			2000
#define QUEUE_SIZE_TO_LOG				1000

CMapObjectQueue::CMapObjectQueue():
	m_oldAvatarX(0.0f), 
	m_oldAvatarY(0.0f),
	m_avatarX(0.0f),
	m_avatarY(0.0f),
	MP_MAP_INIT(m_objectInfoHeap),
	MP_MAP_INIT(m_mapObjectID),
	MP_MAP_INIT(m_highPriorityObjectsMap),
	MP_STRING_INIT(m_avatarLocation)
{
	for (int i = 0; i < MAX_DISTANCE_PARTS; ++i)
	{
		MP_VECTOR_DC_INIT(m_queueObjectInfos[i]);
	}
	InitializeCriticalSection(&m_locker);
}

bool CMapObjectQueue::GetInternalIndex(unsigned int index, unsigned int& partID, unsigned int& partIndex)
{
	unsigned int startIndex = 0;

	for (unsigned int currentPartID = 0; currentPartID < MAX_DISTANCE_PARTS; currentPartID++)
	{
		unsigned int objectCountInPart = (unsigned int)m_queueObjectInfos[currentPartID].size();

		if ((index >= startIndex) && (index < objectCountInPart))
		{
			partID = currentPartID;
			partIndex = index - startIndex;
			return true;
		}

		startIndex += objectCountInPart;
	}

	return false;
}

bool CMapObjectQueue::GetObjectInfoByIndex(unsigned int index, ZONE_OBJECT_INFO*& objectInfo)
{
	CCriticalSectionGuard guard(&m_locker);
	unsigned int partID, partIndex;
	if (GetInternalIndex(index, partID, partIndex))
	{
		objectInfo = m_queueObjectInfos[partID][partIndex];
		if (0 == partID)
		{
			objectInfo->isHighPriority = true;
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool CMapObjectQueue::RemoveObjectInfoByIndex(unsigned int index)
{
	CCriticalSectionGuard guard(&m_locker);
	unsigned int partID, partIndex;
	if (GetInternalIndex(index, partID, partIndex))
	{
		m_mapObjectID.erase((*(m_queueObjectInfos[partID].begin() + partIndex))->complexObjectID);
		m_queueObjectInfos[partID].erase(m_queueObjectInfos[partID].begin() + partIndex);		
		return true;
	}
	else
	{
		return false;
	}
}

void CMapObjectQueue::DeleteObject( unsigned int auObjectID, unsigned int auRealityID)
{
	for (int partID = 0; partID < MAX_DISTANCE_PARTS; partID++)
	{
		CQueueObjectInfos& vecObjectsInfo = m_queueObjectInfos[partID];
		CQueueObjectInfos::iterator vit = vecObjectsInfo.begin();
		for ( ; vit != vecObjectsInfo.end();)
		{
			ZONE_OBJECT_INFO* objectInfo = (*vit);
			if( objectInfo->complexObjectID.objectID == auObjectID && objectInfo->complexObjectID.realityID == auRealityID)
			{
				CObjectIDMap::iterator it = m_mapObjectID.find( objectInfo->complexObjectID);
				if( it != m_mapObjectID.end())
				{
					if( m_context != NULL && m_context->mpLogWriter)
						m_context->mpLogWriter->WriteLn("[OBJECT-WARNING] CMapObjectQueue::DeleteObject for ", objectInfo->objectName);
					m_mapObjectID.erase( it);
					vit = vecObjectsInfo.erase( vit);
					DeleteObjectInfo( objectInfo);
					return;
				}
				else
				{
					vit++;
				}
				
			}
			else
			{
				vit++;
			}
		}
	}
}

void CMapObjectQueue::DeleteObjectsExceptReality( unsigned int auRealityID)
{
	for (int partID = 0; partID < MAX_DISTANCE_PARTS; partID++)
	{
		CQueueObjectInfos& vecObjectsInfo = m_queueObjectInfos[partID];
		CQueueObjectInfos::iterator vit = vecObjectsInfo.begin();
		for ( ; vit != vecObjectsInfo.end();)
		{
			ZONE_OBJECT_INFO* objectInfo = (*vit);
			if( objectInfo->complexObjectID.realityID != auRealityID)
			{
				vit = vecObjectsInfo.erase( vit);

				CObjectIDMap::iterator it = m_mapObjectID.find( objectInfo->complexObjectID);
				if( it != m_mapObjectID.end())
				{
					m_mapObjectID.erase( it);
				}
				if( m_context != NULL && m_context->mpLogWriter)
						m_context->mpLogWriter->WriteLn("[OBJECT-WARNING] CMapObjectQueue::DeleteObjectsExceptReality ", objectInfo->objectName);
				DeleteObjectInfo( objectInfo);
			}
			else
			{
				vit++;
			}
		}
	}
}

bool CMapObjectQueue::Push(ZONE_OBJECT_INFO* const objectInfo)
{
	CCriticalSectionGuard guard(&m_locker);
	if (m_mapObjectID.find(objectInfo->complexObjectID) != m_mapObjectID.end())
	{
		return false;
	}

	AddObjectAccordingToDistance(objectInfo);
	m_mapObjectID[objectInfo->complexObjectID] = 1;

	return true;
}

bool CMapObjectQueue::CacheSyncState( syncObjectState* aSyncObjectState)
{
	ZONE_OBJECT_ID complexObjectID;
	complexObjectID.objectID = aSyncObjectState->uiID;
	complexObjectID.bornRealityID = aSyncObjectState->uiBornRealityID;
	complexObjectID.realityID = aSyncObjectState->uiRealityID;
	if (m_mapObjectID.find(complexObjectID) == m_mapObjectID.end())
	{
		return false;
	}
	m_objectsStateQueue.AttachSyncObjectState( aSyncObjectState);

	unsigned int queueSize = m_objectsStateQueue.Push( aSyncObjectState);
	if( queueSize != 0 && (queueSize % QUEUE_SIZE_TO_LOG) == 0)
	{
		if( m_context != NULL && m_context->mpLogWriter)
			m_context->mpLogWriter->WriteLn("[CMapObjectQueue::CacheSyncState] queueSize = ", queueSize);
	}
	return true;
}

bool CMapObjectQueue::NextObjectState(ZONE_OBJECT_INFO* aZoneObjectInfo, syncObjectState* &object_props)
{
	return m_objectsStateQueue.NextObjectState( aZoneObjectInfo->complexObjectID.objectID
												, aZoneObjectInfo->complexObjectID.bornRealityID
												, aZoneObjectInfo->complexObjectID.realityID
												, object_props);
}

void CMapObjectQueue::FreeCachedObjectState(syncObjectState* &object_props)
{
	m_objectsStateQueue.DeleteObjectState( object_props);
}

void CMapObjectQueue::AddObjectAccordingToDistance(ZONE_OBJECT_INFO* const objectInfo)
{
	if (!objectInfo)
	{
		return;
	}
	//вставляем объекты-неаватары, локация которых совпадает с локацией своего аватара в m_queueObjectInfos[0]
	//и выходим из функции, создаём их в первую очередь

	if (objectInfo->type != vespace::VE_AVATAR)
	{
		if (m_avatarLocation != "" && m_avatarLocation == objectInfo->locationName)
		{
			m_queueObjectInfos[0].push_back(objectInfo);			
			return;
		}
	}

	float objectX = objectInfo->translation.x;
	float objectY = objectInfo->translation.y;
	if (objectInfo->objectFullState != NULL)
	{
		if ((objectInfo->objectFullState->sys_props.miSet & MLSYNCH_COORDINATE_XY_SET_MASK) != 0)
		{
			objectX = objectInfo->objectFullState->sys_props.mfCoordinateX;
			objectY = objectInfo->objectFullState->sys_props.mfCoordinateY;
		}
	}
	unsigned int dist = (unsigned int)sqrtf(
		(objectX - m_avatarX) * (objectX - m_avatarX) +
		(objectY - m_avatarY) * (objectY - m_avatarY));

	int partID = (int)(dist / DISTANCE_RANGE_LENGTH);

	// because first part is high priority objects
	partID++;

	if (partID >= MAX_DISTANCE_PARTS - 1)
	{
		partID = MAX_DISTANCE_PARTS - 1;
	}

	if (!objectInfo->isMyAvatar)
	{
		m_queueObjectInfos[partID].push_back(objectInfo);
	}
	else
	{
		m_queueObjectInfos[0].insert(m_queueObjectInfos[0].begin(), objectInfo);
	}
}

bool CMapObjectQueue::Pop(ZONE_OBJECT_INFO*& objectInfo)
{
	CCriticalSectionGuard guard(&m_locker);
	if (m_queueObjectInfos[0].empty())
	{
		return false;
	}

	objectInfo = *(m_queueObjectInfos[0].end() - 1);
	m_queueObjectInfos[0].erase(m_queueObjectInfos[0].end() - 1);
	m_mapObjectID.erase(objectInfo->complexObjectID);

	return true;
}

bool CMapObjectQueue::IsEmpty()const
{
	return m_queueObjectInfos[0].empty();
}

int CMapObjectQueue::GetNearestMapObjectCount() const
{
	unsigned int size = 0;
	size += (unsigned int)m_queueObjectInfos[0].size();
	size += (unsigned int)m_queueObjectInfos[1].size();
	size += (unsigned int)m_queueObjectInfos[2].size();

	return size;
}

ZONE_OBJECT_INFO* CMapObjectQueue::NewObjectInfo()
{
	CCriticalSectionGuard guard(&m_locker);
	ZONE_OBJECT_INFO* info = MP_NEW( ZONE_OBJECT_INFO);
	m_objectInfoHeap[info] = info;
	return info;
}

void CMapObjectQueue::DeleteObjectInfo(ZONE_OBJECT_INFO*& objectInfo)
{
	if (objectInfo == NULL)
	{
		return;
	}

	m_objectsStateQueue.DeleteAllObjectStates( objectInfo->complexObjectID.objectID
												, objectInfo->complexObjectID.bornRealityID
												, objectInfo->complexObjectID.realityID);

	MP_DELETE( objectInfo);

	CCriticalSectionGuard guard(&m_locker);
	CMapObjectInfos::iterator it = m_objectInfoHeap.find(objectInfo);
	if (it == m_objectInfoHeap.end())
	{
		return;
	}

	m_objectInfoHeap.erase(it);
	objectInfo = NULL;
}

unsigned int CMapObjectQueue::GetObjectCount()const
{
	unsigned int size = 0;

	for (int partID = 0; partID < MAX_DISTANCE_PARTS; partID++)
	{
		size += (unsigned int)m_queueObjectInfos[partID].size();
	}

	return size;
}

bool CMapObjectQueue::IsLeaked()const
{
	bool isLeaked = (m_objectInfoHeap.size() != m_queueObjectInfos[0].size());
	return isLeaked;
}

void CMapObjectQueue::SetContext(omsContext* context)
{
	m_context = context;
}

void CMapObjectQueue::SetAvatarPosition(const float x, const float y, char* locationName)
{
	m_avatarX = x;
	m_avatarY = y;

	std::string oldAvatarLocation = m_avatarLocation;

	if (locationName == NULL)
		m_avatarLocation = "";
	else
		m_avatarLocation = locationName;

	bool isNeedToReprocessQueue = ((fabsf(m_oldAvatarX - m_avatarX) > MIN_DIST_CHANGE_FOR_REPROCESS) ||
		(fabsf(m_oldAvatarY - m_avatarY) > MIN_DIST_CHANGE_FOR_REPROCESS)) ||
		(oldAvatarLocation != m_avatarLocation);

	if (isNeedToReprocessQueue)
	{
		m_oldAvatarX = m_avatarX;
		m_oldAvatarY = m_avatarY;

		CCriticalSectionGuard guard(&m_locker);

		//надо убрать из m_queueObjectInfos[0] объекты-неаватары, 
		//локация которых перестала совпадать с текущей локацией своего аватара
		//и заново добавлять их через CMapObjectQueue::AddObjectAccordingToDistance

		if (m_queueObjectInfos[0].size() !=0)
		{
			CQueueObjectInfos::iterator it = m_queueObjectInfos[0].begin();

			for (;it!=m_queueObjectInfos[0].end();it++)
			{
				ZONE_OBJECT_INFO*  objInfo = (*it);

				if (objInfo->type != vespace::VE_AVATAR)
				{
					if (objInfo->locationName != m_avatarLocation)
					{
						it = m_queueObjectInfos[0].erase(it);
						AddObjectAccordingToDistance(objInfo);
						it--;
					}
				}
			}		
		}
		
		int objectCount = GetObjectCount();
		if (objectCount != 0)
		{
			std::vector<ZONE_OBJECT_INFO *> savedObjects;
			savedObjects.reserve(objectCount);
			
			for (int partID = 1; partID < MAX_DISTANCE_PARTS; partID++)
			{
				std::vector<ZONE_OBJECT_INFO *>::iterator it = m_queueObjectInfos[partID].begin();
				std::vector<ZONE_OBJECT_INFO *>::iterator itEnd = m_queueObjectInfos[partID].end();
				
				for ( ; it != itEnd; it++)
				{
					savedObjects.push_back(*it);
				}

				m_queueObjectInfos[partID].clear();
			}

			std::vector<ZONE_OBJECT_INFO *>::iterator it = savedObjects.begin();
			std::vector<ZONE_OBJECT_INFO *>::iterator itEnd = savedObjects.end();

			for ( ; it != itEnd; it++)
			{
				AddObjectAccordingToDistance(*it);
			}
		}	
	}
}

void CMapObjectQueue::OnLocationsListChanged()
{
	for (int partID = 0; partID < MAX_DISTANCE_PARTS; partID++)
	{
		std::vector<ZONE_OBJECT_INFO *>::iterator it = m_queueObjectInfos[partID].begin();
		std::vector<ZONE_OBJECT_INFO *>::iterator itEnd = m_queueObjectInfos[partID].end();
				
		for ( ; it != itEnd; it++)
		{
			char* str = m_context->mpMapMan->GetLocationNameByXYZ((*it)->translation.x, (*it)->translation.y, (*it)->translation.z);
			if (str)
			{
				(*it)->locationName = str;			
			}
		}
	}
}

CMapObjectQueue::~CMapObjectQueue()
{
	ZONE_OBJECT_INFO* info = NULL;
	while (Pop(info))
	{
		DeleteObjectInfo( info);
	}

	DeleteCriticalSection(&m_locker);
}