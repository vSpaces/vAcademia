
#pragma once

#include "CommonPhysicsHeader.h"

#include "Manager.h"
#include "PhysicsObject.h"
#include "ControlledObject.h"

#define POTENTIALLY_COLLIDE_OBJECT_COUNT	200
#define MAX_UPDATEABLE_OBJECT_COUNT			16000
#define RAYS_COUNT							5

typedef struct _SContact
{
	int physObjID1;
	int physObjID2;

	_SContact()
	{
		physObjID1 = 0xFFFFFFFF;
		physObjID2 = 0xFFFFFFFF;
	}

	_SContact(const _SContact& contact)
	{
		physObjID1 = contact.physObjID1;
		physObjID2 = contact.physObjID2;
	}
} SContact;

typedef struct _SControlledObject
{
	
} SControlledObject;

class CPhysicsObjectManager : public CManager <CPhysicsObject> 
{
public:
	CPhysicsObjectManager();
	~CPhysicsObjectManager();

	void SetWorkStatus(bool isEnabled);

	int AddObject(std::string name);
	int AddObject(std::wstring name);
	
	void AddContact(SContact& objs);
	void ClearContactList();
	std::vector<SContact>& GetContactList(); 

	void Update();

	dSpaceID* GetSpaceID();
	dWorldID* GetWorldID();
	dJointGroupID* GetContactGroup();

	void DrawDebugInfo();

	void SetControlledObjectID(unsigned int objectID);
	unsigned int GetControlledObjectID()const;

	void SetCollisionStatus(bool isCollision, std::string collisionObjectName);
	bool GetCollisionStatus()const;

	void SetRayZ(float z, CPhysicsObject* rayObject);
	float GetRayZ()const;

	void OnNearCallback(dGeomID o1, dGeomID o2);

	void SetJustCreatingFlag(int flag);
	int GetJustCreatingFlag()const;

	CControlledObject* GetControlledObject()const; 

	bool CheckRayIntersect(CVector3D& rayStart, CVector3D& rayDX);
	bool IsOnlyGroundExistsAtPoint(float x, float y, bool isIgnoreControlledObject = true);
	bool IsGroundExistsAtPoint(float x, float y);
	CVector3D GetProjectionToLand(CVector3D point);
	bool GetProjectionToObjects(CVector3D point, float& resultZ);
	bool TraceRayZ(float x, float y, float* z);

	void AddTrimesh(CPhysicsObject* obj);
	void DeleteTrimesh(CPhysicsObject* obj);
	std::vector<CPhysicsObject *>* GetTrimeshList();

	void AddUpdateableObject(CPhysicsObject* obj, bool isCreatedNow = false);

	std::string GetCollisionObjectName()const;
	CPhysicsObject* GetRayObject()const;

	void SetJustCreatingObject(CPhysicsObject* obj);
	CPhysicsObject* GetJustCreatingObject()const;

	void SetRayOptions(const unsigned int rayID, const CVector3D& start, const CVector3D& delta);

	bool IsCollidedWithOtherObjects(std::vector<CPhysicsObject *>& physicsObjects);

private:
	void InitODE();
	void CloseODE();

	void DoSimulation();

	void SetGravityIfNeeded();

	float m_rayZ;

	__int64 m_lastTime;
	int m_skippedTime;

	bool m_isCollision;
	bool m_isGravityDetected;
	bool m_isWasSimulation;
	bool m_needAddSimulation;
	bool m_isObjectCreated;
	bool m_isEnabled;

	CPhysicsObject* m_updateableObjects[MAX_UPDATEABLE_OBJECT_COUNT];
	CPhysicsObject* m_rayObject;
	int m_updateableObjectCount;

	int m_justCreatedChangeRayZ;

	MP_STRING m_collisionObjectName;

	dGeomID m_ray;
	dGeomID m_rays[RAYS_COUNT];
	CVector3D m_raysPositions[RAYS_COUNT];
	dWorldID m_world;
	dSpaceID m_space;
	dJointGroupID m_contactGroup;
	
	MP_VECTOR<SContact> m_contactList;

	CControlledObject* m_controlledObject;

	MP_VECTOR<CPhysicsObject *> m_triMeshes;

	CPhysicsObject* m_justCreatedObject;

	MP_VECTOR<CPhysicsObject *> m_potentiallyCollideObjects;

	CVector3D m_oldControlledObjectCoords;

	dQuaternion m_d;
};
