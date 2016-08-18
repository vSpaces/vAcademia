#pragma once

#include "CommonEngineHeader.h"

#include "IChangesListener.h"
#include "UpdateSkeletonAnimationTask.h"
#include "IDynamicTexture.h"
#include "SceneObject.h"
#include "3DObject.h"
#include "Vector2D.h"
#include "Vector3D.h"
#include <vector>
#include "PhysicsObject.h"

#define MAX_GROW_STEP		20

#define SHADOWS_NONE		0
#define SHADOWS_STENCIL		1
#define SHADOWS_SHADOWMAP	2

struct SLODLevel
{
	int _modelID;
	int renderMode;
	int shadowType;
	float size;

	float lodMinDist;
	float lodMinDistSq;
	float lodMaxDist;
	float lodMaxDistSq;

	bool isLoaded;
	bool isDynamic;

	C3DObject** visibleObjects;
	int visibleObjectCount;
	unsigned int maxObjectCount;
	
	int GetModelID();
	void SetModelID(int modelID);

	SLODLevel();
};

extern bool g_isShadowsNeeded;

class CTexture;

class CLODGroup : public ISceneObject, public CBaseChangesInformer, public IChangesListener 
{
public:
	CLODGroup();	
	~CLODGroup();

	void EnableShadows(bool isEnabled);

	void SetDrawState(bool isNeedToDraw);
	bool GetDrawState()const;

	bool IsDynamic()const;

	void OnFreeze(C3DObject* const obj, const bool isFrozen);

	void GetMaterialBounds(const unsigned int materialID, float& minX, float& minY, float& minZ, 
								   float& maxX, float& maxY, float& maxZ);

	void Clear3DObjLinks();

	void SetVisibleAll(const bool isVisible);

    int GetPhysicsType()const;
	void SetPhysicsType(const int type);

	unsigned long GetCollisionBits()const;
	void SetCollisionBits(const unsigned long bits);
	
	void Add3DObject(C3DObject* const object3d);
	void UpdateObject(C3DObject* const object3d);
	void Draw();
	void RenderObject(C3DObject* const obj);
	void RenderObjectSafely(C3DObject* const obj);
	void Update(const bool isCameraChanged = true, const bool isUpdateAllAnyway = false, const unsigned int shiftTime = 0);
	void UpdateAnimations();
	void Finalize();

	void SetCameraSpaceIfNeed(C3DObject* const obj);
	void UnSetCameraSpaceIfNeed(C3DObject* const obj);

	void SetInvisibleDist(const float dist);
	float GetInvisibleDist()const;

	void AddModelAsLODLevel(const int* modelIDs, const int shadowType, const float scale, const float lodMaxDist);
	void AddModelAsLODLevel(const std::string* modelName, const int shadowType, const float scale, const float lodMaxDist);

	void ClearAll();
	void ClearVisibilityCheckResults();
	void DeleteAllObjects();
	void DeleteAllExcept(C3DObject* const obj);
	void ReloadAll();

	void OnChanged(int eventID);

	bool IsGeometryLoaded()const;
	bool IsFullyLoaded()const;

	void ChangeCollisions(C3DObject* const object3d, const bool isEnabled);

	int GetCurrentModelID(C3DObject* const obj3d)const ;
	SLODLevel* GetLodLevel(const unsigned char id);
	unsigned char GetLodLevelCount()const;
	bool IsLodVisible(const unsigned int lodID)const;

	bool IsModelVisible(CModel* const model)const;
	bool IsTexturePotentialVisible(CTexture* const texture)const;

	void RemoveObject(C3DObject* const obj3d);

	void SetTexture(const int orderID, const int textureID, const bool isTraceable, const int level = -1, const CVector2D aspect = CVector2D(1.0f, 1.0f));
	void SetDynamicTexture(const int orderID, IDynamicTexture* const dynamicTexture);

	void SetShadowStatus(const int lodID, const int shadowType);

	std::vector<C3DObject*>& GetObjects();

	void UpdateBoundingBox(C3DObject* const obj3d);

	void InitTriMesh(CRayTraceTriMesh* const triMesh);

	void NeedToUpdate();
	void OnDeleted(C3DObject* const obj);

	void FreeResources();

	void SetMaterialAsXML(const int orderID, const std::string& xml);

	bool IsOk()const;

	int DetectObjectMaterial(C3DObject* const obj, const int mouseX, const int mouseY);

	void SetCommonColor(const unsigned char r, const unsigned char _g, const unsigned char b);
	void SetCommonScale(const float x, const float y, const float z);
	void SetCommonAlpha(const unsigned char alpha);

	void ToInitialPose();
	void FromInitialPose();
	bool IsSetToInitialPose()const;

	void OnObjectCreated(C3DObject* obj3d);

	void SetTextureTiling(const int orderID, CVector2D aspect);

	void SetUnpickable(bool isUnpickable);
	bool IsUnpickable()const;

private:
	void UpdateRenderModes();

	bool IsFrozen();

	void ShowObjectBoundingBox(C3DObject* obj3d);
	
	void ShowCoordinateAxes(C3DObject* obj3d);

	void DrawDynamic();
	void UpdateDynamic(bool isUpdateAllAnyway, const unsigned int shiftTime = 0);

	float m_invisibleDist;
	float m_invisibleDistSq;
	unsigned long m_collisionBits;
	__int64 m_lastUpdateTime;
	__int64 m_updateAnimationTime;

	MP_VECTOR<SLODLevel *> m_lodLevels;
	MP_VECTOR<C3DObject *> m_objects;

	CUpdateSkeletonAnimationTask* m_updateAnimationTask;

	MP_VECTOR<CPhysicsObject*> m_physicsObjects;
	int m_physicsModelID;
	int	m_physicsType;
	int m_updateCount;
	int m_lodNum;

	bool m_isOk;
	bool m_isDynamic;
	bool m_isFinalized;
	bool m_isNeedToUpdate;
	bool m_isPhysicsLoaded;
	bool m_isSetToInitialPose;
	bool m_isFirstLODLoaded;
	bool m_isNeedToDraw;
	bool m_isUnpickable;
};