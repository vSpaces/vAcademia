#pragma once

#include "CommonEngineHeader.h"

#include "LODGroup.h"
#include "Manager.h"
#include "Scene.h"
#include <string>

#define MAX_TRANSPARENT_OBJECTS		4096
#define MAX_ALPHATEST_OBJECTS		4096

typedef struct SAlphaTestObject
{
	C3DObject* obj3d;
	unsigned int dist;
} _SAlphaTestObject;

typedef struct STransparentObject
{
	C3DObject* obj3d;
	unsigned int dist;
} _STransparentObject;

class CLODGroupManager : public CManager<CLODGroup>
{
friend class CGlobalSingletonStorage;

public:
	void Clear3DObjLinks();

	void ClearPrivelegedObjects();

	void LoadAll();
	void DrawAll();
	void UpdateAll();
	void DrawAllWithoutUpdate();
	void DrawAllTransparent();
	void DrawAllAlphaTest();
	void DrawAllUnpickable();

	void AddTransparentObject(C3DObject* const obj3d);
	void AddAlphaTestObject(C3DObject* const obj3d);

	void MakePickable(CLODGroup* const lodGroup);
	void MakeUnpickable(CLODGroup* const lodGroup);
	
	void OnNewFrame();

	__forceinline float GetLODScaleByDefault()const
	{
		return m_lodScale;
	}

	__forceinline float GetLODScale(C3DObject* const obj)const
	{
		return (obj->IsLODDistanceCanBeChanged()) ? m_lodScale : 1.0f;
	}

	__forceinline float GetLODScaleSq(C3DObject* const obj)const
	{
		return (obj->IsLODDistanceCanBeChanged()) ? m_lodScale : 1.0f;
	}

	void SetLODScale(const float lodScale);

	bool IsCameraChanged()const;

private:
	CLODGroupManager(void);
	CLODGroupManager(const CLODGroupManager&);
	CLODGroupManager& operator=(const CLODGroupManager&);
	~CLODGroupManager(void);

	void LoadLodGroup(const std::string& groupFileName);

	float m_lodScale, m_lodScaleSq;
	
	CVector3D m_lastEyePos;
	CVector3D m_lastLookAt;

	MP_VECTOR<C3DObject *> m_privelegedObjects;

	SAlphaTestObject m_alphatestObjects[MAX_ALPHATEST_OBJECTS];
	int m_alphatestObjectIDs[MAX_ALPHATEST_OBJECTS];
	int m_alphatestObjectsCount;

	STransparentObject m_transparentObjects[MAX_TRANSPARENT_OBJECTS];
	int m_transparentObjectIDs[MAX_TRANSPARENT_OBJECTS];
	int m_transparentObjectsCount;

	MP_VECTOR<CLODGroup *> m_unpickables;

	float m_yaw, m_pitch, m_roll;
};