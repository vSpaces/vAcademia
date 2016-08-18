
#include "StdAfx.h"
#include "UserData.h"
#include "PhysicsParams.h"
#include "PhysicsObjectManager.h"
#include "GlobalSingletonStorage.h"
#include "miniglut.h"

#define NEW_COLLISION_METHOD	1

#define SQUARE(a)					(a * a)
#define MAX_COLLISION_BOX_SIZE		2000
#define MAX_COLLISION_BOX_SIZE_SQ	SQUARE(MAX_COLLISION_BOX_SIZE)
#define MAX_DIST_FOR_LIST_UPDATE_SQ	SQUARE(100)

static void nearCallback (void* data, dGeomID o1, dGeomID o2)
{
	g->phom.OnNearCallback(o1, o2);
}

CPhysicsObjectManager::CPhysicsObjectManager():
	m_ray(NULL),
	m_rayZ(0.0f),
	m_world(NULL),
	m_space(NULL),
	m_lastTime(0),	
	m_skippedTime(0),
	m_isEnabled(true),
	m_isCollision(false),
	m_contactGroup(NULL),	
	m_isObjectCreated(false),
	m_controlledObject(NULL),
	m_justCreatedObject(NULL),
	m_needAddSimulation(false),
	m_isGravityDetected(false), 
	m_justCreatedChangeRayZ(0),
	m_updateableObjectCount(0),
	MP_VECTOR_INIT(m_contactList),
	MP_VECTOR_INIT(m_triMeshes),
	MP_STRING_INIT(m_collisionObjectName),
	MP_VECTOR_INIT(m_potentiallyCollideObjects)
{
	m_contactList.clear();
	ReserveMemory(5000);
	InitODE();

	m_triMeshes.reserve(200);

	for (int k = 0; k < RAYS_COUNT; k++)
	{
		m_rays[k] = NULL;
	}

	m_oldControlledObjectCoords.Set(0.0f, 0.0f, 0.0f);
}

bool CPhysicsObjectManager::IsCollidedWithOtherObjects(std::vector<CPhysicsObject *>& physicsObjects)
{
	if (physicsObjects.size() == 0)
	{
		return false;
	}

	dContact contact[MAX_CONTACT_COUNT];

	std::vector<CPhysicsObject *>::iterator it = physicsObjects.begin();
	std::vector<CPhysicsObject *>::iterator itEnd = physicsObjects.end();

	for ( ; it != itEnd; it++)
	{
		(*it)->SetCheckedNowStatus(true);
	}

	for (it = physicsObjects.begin(); it != itEnd; it++)
	{
		dGeomID o1 = (*it)->GetGeomID();

		std::vector<CPhysicsObject *>::iterator it2 = GetLiveObjectsBegin();
		std::vector<CPhysicsObject *>::iterator it2End = GetLiveObjectsEnd();

		for ( ; it2 != it2End; it2++)
		{
			if ((*it2)->IsCheckedNow())
			{
				continue;
			}

			if ((*it)->GetType() == PHYSICS_OBJECT_TYPE_STATIC_TRIMESH)
			{
				continue;
			}

			dGeomID o2 = (*it2)->GetGeomID();
			if (dCollide(o1, o2, MAX_CONTACT_COUNT, &contact[0].geom, sizeof(dContact)) > 0)
			{
                for (it = physicsObjects.begin(); it != itEnd; it++)
				{
					(*it)->SetCheckedNowStatus(false);
				}

				return true;
			}
		}
	}

	for (it = physicsObjects.begin(); it != itEnd; it++)
	{
		(*it)->SetCheckedNowStatus(false);
	}

	return false;
}

void CPhysicsObjectManager::OnNearCallback(dGeomID o1, dGeomID o2)
{
	if (o1 == NULL)
	{
		return;
	}

	if (o2 == NULL) 
	{
		return;
	}

	dContact contact[MAX_CONTACT_COUNT];
	
	dBodyID b1 = dGeomGetBody(o1);
	dBodyID b2 = dGeomGetBody(o2);

	int numc = dCollide(o1, o2, MAX_CONTACT_COUNT, &contact[0].geom, sizeof(dContact));

    if (numc)
    {
		int physObj1 = (int)dGeomGetData(o1);
		int physObj2 = (int)dGeomGetData(o2);

		for (int i = 0; i < numc; i++)
		{
			contact[i].geom.normal[0] *= dGeomGetPosition(o2)[0] ;
			contact[i].geom.normal[1] *= dGeomGetPosition(o2)[1] ;
			
			contact[i].surface.mode =  dContactBounce | dContactSoftCFM | dContactSoftERP ;
			contact[i].surface.mu = dInfinity; 
			contact[i].surface.soft_cfm = 0.0;		
			contact[i].surface.soft_erp = 0.0;
			contact[i].surface.bounce = 1.0;
			contact[i].surface.bounce_vel = 0.0;
			
			dJointID c = dJointCreateContact(*g->phom.GetWorldID(), *g->phom.GetContactGroup(), &contact[i]);
			dJointAttach(c, b1, b2);
		}
		
		if ((m_rays[0] == o1) || (m_rays[0] == o2) || (m_rays[1] == o1) || (m_rays[1] == o2) || (m_rays[2] == o1) || (m_rays[2] == o2) ||
			(m_rays[3] == o1) || (m_rays[3] == o2) || (m_rays[4] == o1) || (m_rays[4] == o2))
		{
			if ((physObj1 != 0) && (physObj2 != 0))
			{
				for (int i = 0; i < numc; i++)
				{
					if (m_rayZ < RAY_MAX_Z - contact[i].geom.depth)
					{
						int collideCount = 0;
												
						if (physObj1 > 0)
						{
							CPhysicsObject* obj = g->phom.GetObjectPointer(physObj1);
							if (obj->GetObject3D()->GetAnimation())
							{
								m_rayObject = obj;							
								collideCount++;
							}
							SetJustCreatingFlag(obj->IsJustCreated());
							if (obj->IsJustCreated())
							{
								SetJustCreatingObject(obj);
							}
						}

						if (physObj2 > 0)
						{
							CPhysicsObject* obj = g->phom.GetObjectPointer(physObj2);
							if (obj->GetObject3D()->GetAnimation())
							{
								m_rayObject = obj;							
								collideCount++;
							}
							SetJustCreatingFlag(obj->IsJustCreated());
							if (obj->IsJustCreated())
							{
								SetJustCreatingObject(obj);
						}

						if (collideCount != 1)
						{
							m_rayZ = RAY_MAX_Z - contact[i].geom.depth;														
						}
					}
				}
			}
		}
		}
		else
		{
			/*CPhysicsObject* obj1 = g->phom.GetObjectPointer(physObj1 >= 0 ? physObj1 : 0);
			CPhysicsObject* obj2 = g->phom.GetObjectPointer(physObj2 >= 0 ? physObj2 : 0);
			g->lw.WriteLn("Collision: ", physObj1, " ", physObj2, " ", obj1->GetName(), " ", obj2->GetName());*/

			for (int i = 0; i < numc; i++)
			{
				contact[i].geom.normal[0] = -contact[i].geom.pos[0] + dGeomGetPosition(o1)[0] ;
				contact[i].geom.normal[1] = -contact[i].geom.pos[1] + dGeomGetPosition(o1)[1] ;
		
				contact[i].surface.mode =  dContactBounce | dContactSoftCFM | dContactSoftERP ;
				contact[i].surface.mu = dInfinity; 
				contact[i].surface.soft_cfm = 0.0;		
				contact[i].surface.soft_erp = 0.0;
				contact[i].surface.bounce = 1.0;
				contact[i].surface.bounce_vel = 0.0;
		
				dJointID c = dJointCreateContact(*g->phom.GetWorldID(), *g->phom.GetContactGroup(), &contact[i]);
				dJointAttach(c, b1, b2);
			}

			int controlledObjectID = GetControlledObject()->GetObjectID();

			if ((controlledObjectID == physObj1) || (controlledObjectID == physObj2))
			{
				SContact tmp;
				tmp.physObjID1 = physObj1;
				tmp.physObjID2 = physObj2;
#ifdef USE_PHYSICS
				//g->phom.AddContact(tmp);
#endif			
				
				//m_isCollision = true;
				
				unsigned int objectID = physObj1;
				if (GetControlledObjectID() == objectID)
				{
					objectID = physObj2;
				}
				
				CPhysicsObject* obj = GetObjectPointer(objectID);
				if (obj->GetObject3D())
				{
					USES_CONVERSION;
					m_collisionObjectName = W2A( obj->GetObject3D()->GetName());
				}
			}
		}
	}
}

void CPhysicsObjectManager::SetJustCreatingObject(CPhysicsObject* obj)
{
	m_justCreatedObject = obj;
}

CPhysicsObject* CPhysicsObjectManager::GetJustCreatingObject()const
{
	return m_justCreatedObject;
}

std::string CPhysicsObjectManager::GetCollisionObjectName()const
{
	return m_collisionObjectName;
}

void CPhysicsObjectManager::SetCollisionStatus(bool isCollision, std::string collisionObjectName)
{
	m_isCollision = isCollision;
	if ((m_isCollision) && (!collisionObjectName.empty()))
	{
		m_collisionObjectName = collisionObjectName;
	}
}

bool CPhysicsObjectManager::GetCollisionStatus()const
{
	return m_isCollision;
}

void CPhysicsObjectManager::SetJustCreatingFlag(int flag)
{
	m_justCreatedChangeRayZ = flag;
}

int CPhysicsObjectManager::GetJustCreatingFlag()const
{
	return m_justCreatedChangeRayZ;
}

void CPhysicsObjectManager::SetRayZ(float z, CPhysicsObject* rayObject)
{
	m_rayZ = z;
	m_rayObject = rayObject;
}

float CPhysicsObjectManager::GetRayZ()const
{
	return m_rayZ;
}

CPhysicsObject* CPhysicsObjectManager::GetRayObject()const
{
	return m_rayObject;
}

void CPhysicsObjectManager::DoSimulation()
{
	if (!m_isEnabled)
	{
		return;
	}

	dReal stepTime = (float)STEP_TIME;
	int stepCount = 1;
	
	if (m_controlledObject)
	{
		stepCount = m_controlledObject->GetStepCount();
		m_controlledObject->CalculateStepVelocity();
	}

	bool isCollision = false;	

#ifdef NEW_COLLISION_METHOD
	int cnt = 0;
	if (m_controlledObject)
	if (m_controlledObject->GetObject3D())
	{
		CVector3D pos = m_controlledObject->GetObject3D()->GetCoords();
		if (((pos - m_oldControlledObjectCoords).GetLengthSq() > MAX_DIST_FOR_LIST_UPDATE_SQ) ||
			(m_controlledObject->IsGravitationChanged()) || (m_isObjectCreated))
		{
			m_oldControlledObjectCoords = pos;
			m_potentiallyCollideObjects.clear();
			m_potentiallyCollideObjects.reserve(POTENTIALLY_COLLIDE_OBJECT_COUNT);			

			std::vector<CPhysicsObject *>::iterator it = GetLiveObjectsBegin();
			std::vector<CPhysicsObject *>::iterator itEnd = GetLiveObjectsEnd();

			for ( ; it != itEnd; it++)
			if ((*it)->GetObject3D())
			{
				CVector3D coords = (*it)->GetCoords();
				float size = (*it)->GetMaxLinearSize();
				if ((coords - pos).GetLength() - size > MAX_COLLISION_BOX_SIZE)
				{
					(*it)->Disable();
				}
				else if ((*it)->GetType() != PHYSICS_OBJECT_TYPE_STATIC_TRIMESH)
				{
					(*it)->Enable();
					m_potentiallyCollideObjects.push_back(*it);
					cnt++;
				}
			}
		}
	}
#endif

	int maxSkippedTime = MAX_SKIPPED_TIME / 10; 

	if (g->phom.GetRayZ() < 0)
	{
		maxSkippedTime = MAX_SKIPPED_TIME / 20;
	}
	
	for (int i = 0; ((i < stepCount) && (!isCollision)); i++)
	if (((m_controlledObject) && ((m_needAddSimulation) || (m_controlledObject->IsMoved()) || (!m_controlledObject->IsOnGround()))) || (m_skippedTime > maxSkippedTime) || (m_isCollision))
	{
		m_isCollision = false;

		if ((m_controlledObject) && (m_controlledObject->IsMoved()))
		{
			m_needAddSimulation = true;
		}
		else
		{
			m_needAddSimulation = false;
		}

		if (m_controlledObject)
		{
			m_controlledObject->OnBeforeSimulation();
			m_controlledObject->SetStepVelocity();
		}

		if (stepTime != 0)
		{	
			if (m_controlledObject)
			{
				int objectID = m_controlledObject->GetObjectID();
				CPhysicsObject* physObj = GetObjectPointer(objectID);
				dSpaceCollide2(physObj->GetGeomID(), (dGeomID)m_space,0,&nearCallback);
				//int t1 = g->tp.GetMicroTickCount();

/*#ifdef NEW_COLLISION_METHOD
				std::vector<CPhysicsObject *>::iterator it = m_potentiallyCollideObjects.begin();
				std::vector<CPhysicsObject *>::iterator itEnd = m_potentiallyCollideObjects.end();
				
				for ( ; it != itEnd; it++)
				{
					OnNearCallback(physObj->GetGeomID(), (*it)->GetGeomID()); 
				}
				
#else
				dSpaceCollide2(physObj->GetGeomID(), (dGeomID)m_space, 0, &nearCallback);			
#endif

				//int t2 = g->tp.GetMicroTickCount();
				//g->lw.WriteLn("dSpaceCollide2: ", t2 - t1, " ", cnt);

				//int _t1 = g->tp.GetMicroTickCount();

#ifdef NEW_COLLISION_METHOD
				{
					std::vector<CPhysicsObject *>::iterator it = GetLiveObjectsBegin();
					std::vector<CPhysicsObject *>::iterator itEnd = GetLiveObjectsEnd();
					
					for ( ; it != itEnd; it++)
					{
						for (int rayID = 0; rayID < RAYS_COUNT; rayID++)
						if ((*it)->IsPointInXY(m_raysPositions[rayID].x, m_raysPositions[rayID].y))
						{							
							//OnNearCallback(m_rays[rayID], (*it)->GetGeomID());
						}
					}
				}
				
#else
				for (int k = 0; k < RAYS_COUNT; k++)
				{
					m_ray = m_rays[k];
					if (m_ray)
					{
						dSpaceCollide2(m_ray, (dGeomID)m_space, 0, &nearCallback);
					}
				}
#endif*/

				//int _t2 = g->tp.GetMicroTickCount();
				//g->lw.WriteLn("dSpaceCollide3: ", _t2 - _t1);				
			}
			
			//dSpaceCollide(m_space,0,&nearCallback);
			dWorldQuickStep(m_world, stepTime);
			dJointGroupEmpty(m_contactGroup);
			
			m_isWasSimulation = true;
			
		}

		if (m_controlledObject)
		{
			m_controlledObject->OnAfterSimulation();
		}

		isCollision = m_isCollision;

		m_skippedTime = 0;
	}
	else
	{
		m_skippedTime += (int)(g->ne.time - m_lastTime);
	}

	m_lastTime = g->ne.time;
}

void CPhysicsObjectManager::InitODE()
{
	dInitODE();

	m_world = dWorldCreate();
	m_space = dHashSpaceCreate(0);
	dHashSpaceSetLevels(m_space, 1, 16);
	m_contactGroup = dJointGroupCreate(0);
	

	dWorldSetContactMaxCorrectingVel(m_world, 0.0);
	dWorldSetContactSurfaceLayer(m_world, 0.000001);
}

int CPhysicsObjectManager::AddObject(std::string name)
{
	int objID = CManager<CPhysicsObject>::AddObject(name);
	CPhysicsObject* obj = GetObjectPointer(objID);
	obj->SetObjectID(objID);

	return objID;
}

int CPhysicsObjectManager::AddObject(std::wstring name)
{
	int objID = CManager<CPhysicsObject>::AddObject(name);
	CPhysicsObject* obj = GetObjectPointer(objID);
	obj->SetObjectID(objID);

	return objID;
}

void CPhysicsObjectManager::CloseODE()
{
	dJointGroupDestroy(m_contactGroup); 
	dSpaceDestroy(m_space);
	dWorldDestroy(m_world); 
}

dWorldID* CPhysicsObjectManager::GetWorldID()
{
	return &m_world;
}

dSpaceID* CPhysicsObjectManager::GetSpaceID()
{
	return &m_space;
}

dJointGroupID* CPhysicsObjectManager::GetContactGroup()
{
	return &m_contactGroup;
}

void CPhysicsObjectManager::SetGravityIfNeeded()
{
	if (!m_isGravityDetected)
	{
		if (!g->rs.GetBool(VERTICAL_AXE_Z))
		{
			dWorldSetGravity(m_world, 0, PHYSICS_GRAVITY, 0);	
		}
		else
		{
			dWorldSetGravity(m_world, 0, 0, PHYSICS_GRAVITY);
		}

		for (int k = 0; k < RAYS_COUNT; k++)
		{
			m_rays[k] = dCreateRay(m_space, RAY_LENGTH);
			dGeomSetData(m_rays[k], (void*)-4);
			dGeomEnable(m_rays[k]);
		}

		m_ray = m_rays[0];

		if (m_controlledObject)
		{
			m_controlledObject->InitRays();
		}

		m_isGravityDetected = true;
	}
}

void CPhysicsObjectManager::AddUpdateableObject(CPhysicsObject* obj, bool isCreatedNow)
{
	if (m_updateableObjectCount < MAX_UPDATEABLE_OBJECT_COUNT)
	{
		m_updateableObjects[m_updateableObjectCount] = obj;
		m_updateableObjectCount++;
	}
	else
	{
		assert(false);
	}

	if (isCreatedNow)
	{
		m_isObjectCreated = true;
	}
}

void CPhysicsObjectManager::Update()
{
	g->pm.CustomBlockStart(CUSTOM_BLOCK_PHYSICS_UPDATE);

	m_isWasSimulation = false;

	SetGravityIfNeeded();

	stat_physicsObjectCount = dSpaceGetNumGeoms(m_space) + RAYS_COUNT; // + 1 ray

	DoSimulation();

	m_isObjectCreated = false;

	for (int k = 0; k < m_updateableObjectCount; k++)
	{
		m_updateableObjects[k]->Update();
		if (m_isWasSimulation)
		{
			m_updateableObjects[k]->OnUse();
		}
	}

	if (m_isWasSimulation)
	{
		m_updateableObjectCount = 0;
	}

	g->pm.CustomBlockEnd(CUSTOM_BLOCK_PHYSICS_UPDATE);
}

void CPhysicsObjectManager::ClearContactList()
{
	m_contactList.clear();
}

void CPhysicsObjectManager::SetWorkStatus(bool isEnabled)
{
	m_isEnabled = isEnabled;
}

void CPhysicsObjectManager::AddContact(SContact& objs)
{
	bool isExists = false;

	std::vector<SContact>::iterator it = m_contactList.begin();
	std::vector<SContact>::iterator itEnd = m_contactList.end();
	for ( ; it != itEnd; it++)
	{
		if ((objs.physObjID1 == (*it).physObjID1) && (objs.physObjID2 == (*it).physObjID2) ||
			(objs.physObjID2 == (*it).physObjID1) && (objs.physObjID1 == (*it).physObjID2))
		{
			isExists = true;
		}
	}

	if (!isExists)
	{
		m_contactList.push_back(objs);
	}	
}

std::vector<SContact>& CPhysicsObjectManager::GetContactList()
{
	return m_contactList;
}

void CPhysicsObjectManager::DrawDebugInfo()
{
	GLFUNC(glDisable)(GL_TEXTURE_2D);
	g->stp.SetColor(255, 255, 255);
	g->sm.UnbindShader();

	std::vector<CPhysicsObject *>::iterator it = GetLiveObjectsBegin();
	std::vector<CPhysicsObject *>::iterator itEnd = GetLiveObjectsEnd();

	for ( ; it != itEnd; it++)
	{
		CPhysicsObject* physicObject = *it;		

		g->stp.PushMatrix();

		CVector3D physTranslation;
		CVector3D physDemension;
		float tmp;
		((CBasePhysicsPrimitive *)(physicObject->GetPrimitive()))->GetInitialDimensions(physDemension.x, physDemension.y, physDemension.z, tmp);
		
		physicObject->GetCoords(&physTranslation.x, &physTranslation.y, &physTranslation.z);
		GLFUNC(glTranslatef)(physTranslation.x, physTranslation.y, physTranslation.z);
								
		if ((physicObject->GetType() == PHYSICS_OBJECT_TYPE_STATIC_BOX) || (physicObject->GetType() == PHYSICS_OBJECT_TYPE_BOX))
		{
			assert(physicObject->GetRotationPack());
			physicObject->GetRotationPack()->ApplyRotation();

			float scaleDelta = 1.0f / physDemension.x;
			GLFUNC(glScalef)(physDemension.x * scaleDelta, physDemension.y * scaleDelta, physDemension.z * scaleDelta);

			g->stp.PrepareForRender();
			miniglut::glutWireCube(physDemension.x);
		}
		else if ((physicObject->GetType() == PHYSICS_OBJECT_TYPE_STATIC_SPHERE) || (physicObject->GetType() == PHYSICS_OBJECT_TYPE_SPHERE))
		{
			assert(physicObject->GetRotationPack());
			physicObject->GetRotationPack()->ApplyRotation();			

			g->stp.PrepareForRender();
			miniglut::glutWireSphere(physDemension.x * 0.5f, 8, 8);			
		}

		g->stp.PopMatrix();
	}

	GLFUNC(glEnable)(GL_TEXTURE_2D);
}

void CPhysicsObjectManager::SetControlledObjectID(unsigned int objectID)
{
	if (!m_controlledObject)
	{
		m_controlledObject = MP_NEW(CControlledObject);
		m_controlledObject->InitRays();		
	}

	m_controlledObject->SetObjectID(objectID);

	CPhysicsObject* obj = GetObjectPointer(objectID);
	obj->GetObject3D()->SetControlledObjectStatus(true);
}

unsigned int CPhysicsObjectManager::GetControlledObjectID()const
{
	if (!m_controlledObject)
	{
		return 0xFFFFFFFF;
	}

	return m_controlledObject->GetObjectID();
}

CControlledObject* CPhysicsObjectManager::GetControlledObject()const
{
	return m_controlledObject;
}

bool CPhysicsObjectManager::CheckRayIntersect(CVector3D& rayStart, CVector3D& rayDX)
{
	if (!m_controlledObject)
	{
		return false;
	}

	if (!m_controlledObject->GetObject3D())
	{
		return false;
	}

	float x, y, z;
	m_controlledObject->GetObject3D()->GetCoords(&x, &y, &z);

	std::vector<CPhysicsObject *>::iterator it = m_triMeshes.begin();
	std::vector<CPhysicsObject *>::iterator itEnd = m_triMeshes.end();

	for ( ; it != itEnd; it++)
	{
		(*it)->GetCoords(&x, &y, &z);

		if ((int)((*it)->GetObject3D()->GetUserData(USER_DATA_LEVEL)) == LEVEL_GROUND)
		{
			continue;
		}

		if ((*it)->CheckRayIntersect(rayStart, rayDX))
		{
			return true;
		}
	}

	return false;
}

void CPhysicsObjectManager::AddTrimesh(CPhysicsObject* obj)
{
	m_triMeshes.push_back(obj);
}

std::vector<CPhysicsObject *>* CPhysicsObjectManager::GetTrimeshList()
{
	return &m_triMeshes;
}

void CPhysicsObjectManager::DeleteTrimesh(CPhysicsObject* obj)
{
	std::vector<CPhysicsObject *>::iterator it = m_triMeshes.begin();
	std::vector<CPhysicsObject *>::iterator itEnd = m_triMeshes.end();

	for ( ; it != itEnd; it++)
	if (*it == obj)
	{
		m_triMeshes.erase(it);
		break;
	}
}

bool CPhysicsObjectManager::TraceRayZ(float x, float y, float* z)
{
	bool traceResult = false;

    *z = -1000.0f;

	std::vector<CPhysicsObject *>::iterator it = m_triMeshes.begin();
	std::vector<CPhysicsObject *>::iterator itEnd = m_triMeshes.end();

	float _z;
	for ( ; it != itEnd; it++)
	if (((*it)->GetObject3D()) && ((int)(*it)->GetObject3D()->GetUserData(USER_DATA_LEVEL) != LEVEL_CAMERA_COLLISION_ONLY))
	{
		bool res = (*it)->TraceRayZ(x, y, &_z);
		if ((res) && (_z > *z))
		{
			*z = _z;
			traceResult = true;
		}
	}

	return traceResult;
}

bool CPhysicsObjectManager::IsOnlyGroundExistsAtPoint(float x, float y, bool isIgnoreControlledObject)
{
	float saveRayZ = m_rayZ;
	m_rayZ = MIN_RAYTRACE_Z;
	CPhysicsObject* saveRayObject = m_rayObject;
	m_rayObject = NULL;

	if (m_rays[0])
	{
        dGeomRaySet(m_rays[0], x, y, RAY_MAX_Z, 0, 0, -RAY_LENGTH);
		m_ray = m_rays[0];

		std::vector<CPhysicsObject *>::iterator it = GetLiveObjectsBegin();
		std::vector<CPhysicsObject *>::iterator itEnd = GetLiveObjectsEnd();

		for ( ; it != itEnd; it++)		
		{					
			OnNearCallback(m_rays[0], (*it)->GetGeomID());
		}	
	}

	bool result = (m_rayZ == MIN_RAYTRACE_Z);

	if ((m_rayObject != NULL) && (m_controlledObject != NULL))
	{
		if (!((isIgnoreControlledObject) && (m_rayObject->GetID() == (int)m_controlledObject->GetObjectID())))
		{
			result = false;
		}
	}

	m_rayZ = saveRayZ;
	m_rayObject = saveRayObject;

	return result;
}

bool CPhysicsObjectManager::IsGroundExistsAtPoint(float x, float y)
{
	CVector3D res = GetProjectionToLand(CVector3D(x, y, 0));
	return (res.z > MIN_RAYTRACE_Z);
}

CVector3D CPhysicsObjectManager::GetProjectionToLand(CVector3D point)
{
	float saveRayZ = m_rayZ;
	m_rayZ = MIN_RAYTRACE_Z;
	TraceRayZ(point.x, point.y, &m_rayZ);
	float resRayZ = m_rayZ;
	m_rayZ = saveRayZ;

	/*if (m_rays[0])
	{
		dGeomRaySet(m_rays[0], point.x, point.y, RAY_MAX_Z, 0, 0, -RAY_LENGTH);
		m_ray = m_rays[0];

		dSpaceCollide2(m_ray, (dGeomID)m_space, 0, &nearCallback);
	}*/

	return CVector3D(point.x, point.y, resRayZ);
}

bool CPhysicsObjectManager::GetProjectionToObjects(CVector3D point, float& resultZ)
{
	float saveRayZ = m_rayZ;
	m_rayZ = MIN_RAYTRACE_Z;
	CPhysicsObject* saveRayObject = m_rayObject;
	m_rayObject = NULL;

	if (m_rays[0])
	{
		dGeomRaySet(m_rays[0], point.x, point.y, RAY_MAX_Z, 0, 0, -RAY_LENGTH);
		m_ray = m_rays[0];

		dSpaceCollide2(m_ray, (dGeomID)m_space, 0, &nearCallback);		
	}

	bool result = (m_rayZ != MIN_RAYTRACE_Z);

	resultZ = m_rayZ;
	m_rayZ = saveRayZ;
	m_rayObject = saveRayObject;
	
	return result;
}

void CPhysicsObjectManager::SetRayOptions(const unsigned int rayID, const CVector3D& start, const CVector3D& delta)
{
	if (rayID >= RAYS_COUNT)
	{
		return;
	}

	if (!m_rays[rayID])
	{
		return;
	}

	dGeomRaySet(m_rays[rayID], start.x, start.y, start.z, delta.x, delta.y, delta.z);
	m_raysPositions[rayID].Set(start.x, start.y, start.z);
}

CPhysicsObjectManager::~CPhysicsObjectManager()
{
	if (m_controlledObject)
	{
		MP_DELETE(m_controlledObject);
	}
}