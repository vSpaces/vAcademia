
#include "StdAfx.h"
#include <Assert.h>
#include "MapManagerPriority.h"
#include "GlobalSingletonStorage.h"
#include "userdata.h"

#define AUDITORY_SIZE		3000.0f
#define AUDITORY_SIZE_SQ	(AUDITORY_SIZE * AUDITORY_SIZE)

#define	TYPE_MUL_KOEF		100
#define ZONE_MUL_KOEF		50
#define VISIBLE_MUL_KOEF	500

CMapManagerPriority::CMapManagerPriority():
	m_camera(NULL)
{
}

int GetPriorityValue(int type, int zoneID, bool isVisible)
{
	return type * TYPE_MUL_KOEF + (MAX_ZONE_NUM - zoneID) * ZONE_MUL_KOEF + VISIBLE_MUL_KOEF * (isVisible ? 1 : 0);
}

int CMapManagerPriority::GetZoneNumByDist(float distSq)
{
	float distD = sqrtf(distSq);
	distD /= ZONE_SIZE;
	int zoneNum = (int)distD;
	if (zoneNum > MAX_ZONE_NUM)
	{
		zoneNum = MAX_ZONE_NUM;
	}

	return zoneNum;
}

int CMapManagerPriority::GetZoneNum(const ZONE_OBJECT_INFO* const objectInfo, float& distSq)
{
	assert(objectInfo);
	assert(objectInfo->obj3d);

	CVector3D dist = objectInfo->obj3d->GetCoords() - m_cameraPosition;
	distSq = dist.GetLengthSq();

	return GetZoneNumByDist(distSq);
}

int CMapManagerPriority::GetPriority(C3DObject* const obj, const int lod, const int resType)
{
	assert(obj);

	if (!m_camera)
	{
		return PRIORITY_LEVEL_LAST;
	}

	CVector3D objPosition = obj->GetCoords();
	CVector3D dist = objPosition - m_cameraPosition;

	int zoneNum = GetZoneNumByDist(dist.GetLengthSq());
	bool isVisible = obj->IsVisibleNow();

	int priority = GetPriorityValue(resType, zoneNum, isVisible);

	// у чужих аватаров снижаем приоритет на загрузку - главное загрузить мир.
	if ((!g->phom.GetControlledObject()) || ((obj->GetAnimation()) && (obj != g->phom.GetControlledObject()->GetObject3D())))
	{
		priority *= 0.7;
	}

	if (obj->GetUserData(USER_DATA_OBJECT_IN_LECTURE_GROUP) != NULL)
	{
		// пока будем упрощенно определять, в аудитории мы или нет
		if ((obj->GetCoords() - m_cameraPosition).GetLengthSq() > AUDITORY_SIZE_SQ)
		{
			priority = priority >> 1;
		}
	}

	return priority;
}

int CMapManagerPriority::GetObjectPriority(const ZONE_OBJECT_INFO* const objectInfo, void* const exactObject)
{
	assert(exactObject);

	CModel* model = (CModel *)exactObject;
	
	if (model->IsDeleted())
	{
		return -1;
	}

	assert(objectInfo);
	assert(objectInfo->obj3d);

	if ((objectInfo->obj3d->GetLODGroup()) && (!objectInfo->obj3d->GetLODGroup()->IsModelVisible(model)))
	{
		return PRIORITY_LEVEL_LAST;
	}

	return GetPriority(objectInfo->obj3d, objectInfo->lod, RESOURCE_MESH) + ((objectInfo->level%2 == 1) ? PRIORITY_GROUND_ADDITION : 0);
}

int CMapManagerPriority::GetSoundPriority(const ZONE_OBJECT_INFO* const objectInfo)
{
	assert(objectInfo);

	if (!m_camera)
	{
		return PRIORITY_LEVEL_LAST;
	}

	CVector3D objPosition = objectInfo->obj3d->GetCoords();
	CVector3D dist = objPosition - m_cameraPosition;

	int zoneNum = GetZoneNumByDist(dist.GetLengthSq());	
	bool isVisible = objectInfo->obj3d->IsVisibleNow();	

	return GetPriorityValue(RESOURCE_SOUND, zoneNum, isVisible);
}

int CMapManagerPriority::GetTexturePriority(const ZONE_OBJECT_INFO* const objectInfo, void* const exactObject)
{
	CTexture* texture = (CTexture *)exactObject;

	if (texture && texture->IsDeleted())
	{
		return -1;
	}

	assert(objectInfo);
	assert(objectInfo->obj3d);
	
	if (objectInfo->obj3d->GetLODGroup())
	if (texture && (!texture->IsBinded()) && (!objectInfo->obj3d->GetLODGroup()->IsTexturePotentialVisible(texture)))
	{
		return PRIORITY_LEVEL_LAST;
	}

	return GetPriority(objectInfo->obj3d, objectInfo->lod, RESOURCE_TEXTURE);
}

int CMapManagerPriority::GetMinPriority()
{
	return PRIORITY_LEVEL_LAST;
}

void CMapManagerPriority::SetCamera(CCamera3D* const camera)
{
	assert(camera);

	m_camera = camera;
	m_cameraPosition = m_camera->GetEyePosition();

	if (m_cameraPosition == CVector3D(0, 0, 0))
	{
		m_cameraPosition = gRM->mapMan->GetMainObjectCoords();
	}
}

CMapManagerPriority::~CMapManagerPriority()
{
}