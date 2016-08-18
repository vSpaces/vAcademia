
#pragma once

#include "MapManagerStructures2.h"
#include "Camera.h"

#define VISIBILITY_ZONES_COUNT	3
#define LOD_COUNT				4
#define VISIBILITY_COUNT		2
#define RESOURCE_TYPE_COUNT		3

#define	RESOURCE_MESH			2
#define	RESOURCE_TEXTURE		1
#define	RESOURCE_SOUND			0

#define LOD_LEVEL_COUNT			4

#define PRIORITY_EXTREMELY_BIG	4000
#define PRIORITY_VERY_BIG		3000
#define PRIORITY_BIG			2500
#define PRIORITY_MEDIUM			800
#define PRIORITY_LEVEL_LAST		 100
#define PRIORITY_GARBAGE		 1

#define MAX_ZONE_NUM			10

#define PRIORITY_GROUND_ADDITION  1000

class CMapManagerPriority
{
public:
	CMapManagerPriority();
	~CMapManagerPriority();

	int GetObjectPriority(const ZONE_OBJECT_INFO* const objectInfo, void* const resource);
	int GetSoundPriority(const ZONE_OBJECT_INFO* const objectInfo);
	int GetTexturePriority(const ZONE_OBJECT_INFO* const objectInfo, void* const resource);

	int GetMinPriority();

	int GetZoneNum(const ZONE_OBJECT_INFO* const objectInfo, float& distSq);
    	
	void SetCamera(CCamera3D* const camera);

private:
	int GetZoneNumByDist(float distSq);

	int GetPriority(C3DObject* const obj, const int lod, const int resType);

	CVector3D m_cameraPosition;
	CCamera3D* m_camera;
};