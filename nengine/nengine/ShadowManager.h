#pragma once

#include "Manager.h"
#include "3DObject.h"
#include "Vector3D.h"
#include "ShadowObject.h"
#include "RotationPack.h"

#define SHADOWED_OBJECT_MAX_COUNT			2000
#define SHADOW_PROTECTED_OBJECT_MAX_COUNT	1000

struct SShadowedObject
{
	float x, y, z;
	float xAngle, yAngle, zAngle;
	float scaleX, scaleY, scaleZ;
	CRotationPack* rotation;
	int shadowID;
	bool isAvoidSelfShadowing;
	C3DObject* avoidSelfShadowingObject;
	C3DObject* obj3d;

	SShadowedObject()
	{
		rotation = NULL;
		avoidSelfShadowingObject = NULL;
	}
};

class CShadowManager : public CManager<CShadowObject>
{
friend class CGlobalSingletonStorage;

public:	
	CShadowManager();
	~CShadowManager();

	void NeedToDumpShadowQueue();
	void DumpShadowQueueIfNeeded();

	void ReplaceShadowedModel(int shadowID, int modelID);
	void UpdateModelVertexs(int shadowID, int modelID);

	// Load model data to shadow object
	int LoadShadowedModel(int modelID, std::string name);

	// Draw shadow
	void DrawShadow(int modelID);

	// Recount shadow volume
	void CastShadow(int modelID);

	// Start/end of global shadow drawing process
	void StartShadowing();
	void EndShadowing();

	void NewFrame();
	//void AddToShadowQueue(int shadowID, float x, float y, float z, float xAngle, float yAngle, float zAngle, float scale, C3DObject* avoidSelfShadowingObject, C3DObject* obj);
	void AddToShadowQueue(/*int shadowID, float x, float y, float z, CRotationPack* rotation, float scaleX, float scaleY, float scaleZ, C3DObject* avoidSelfShadowingObject, */C3DObject* obj);
	void ClearShadowQueue();
	void RenderAllShadows();
	void RenderShadowCasters(bool isDeleteObjects);

	//void AddToShadowProtectionQueue(C3DObject* obj);

	void SetShadowRect(float x1, float y1, float x2, float y2);

	void CheapSoftShadowSupport(bool isEnabled);
	bool IsCheapSoftShadowingEnabled()const;

	void EnableShadows();
	void DisableShadows();
	
	void SaveShadowQueue();
	void RestoreShadowQueue();

private:
	void DoShadowPass(int nModel);
	bool IsFacesNeighbours(int fs1[3], int fs2[3]);

	void GetCurrentLightPosition(CShadowObject* shadow, float& lx, float& ly, float& lz);

	int m_shadowCount;
	/*SShadowedObject*/C3DObject* m_shadows[SHADOWED_OBJECT_MAX_COUNT];

	int m_savedShadowCount;
	C3DObject* m_savedShadows[SHADOWED_OBJECT_MAX_COUNT];
	CVector3D m_lightPosition;

	int m_sovp[9];

	int m_renderMethod;

	bool m_isCheapSoftShadowingEnabled;
	bool m_isShadowsEnabled;
	bool m_isNeedToDumpShadowQueue;

	float m_shadowX1, m_shadowX2;
	float m_shadowY1, m_shadowY2;

	int m_shadowProtectedObjectsCount;
	C3DObject* m_shadowsProtectedObjects[SHADOW_PROTECTED_OBJECT_MAX_COUNT];
};


