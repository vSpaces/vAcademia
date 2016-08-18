
#include "StdAfx.h"
#include "PhysicsPrimitiveCreator.h"
#include "GlobalSingletonStorage.h"
#include "PhysicsObject.h"
#include "UserData.h"
#include <math.h>
#include "cal3d/memory_leak.h"

CPhysicsObject::CPhysicsObject():
	m_isNeedToUpdate(true),
	m_isJustCreated(false),
	m_isCheckedNow(false),
	m_primitive(NULL),
	m_geometry(NULL),
	m_isStatic(true),
	m_objectID(-1),
	m_obj3d(NULL),	
	m_body(NULL),
	m_minX(FLT_MAX),
	m_minY(FLT_MAX),
	m_maxX(FLT_MIN),
	m_maxY(FLT_MIN)
{
	m_position.Set(0.0f, 0.0f, 0.0f);
	m_velocity.Set(0.0f, 0.0f, 0.0f);
}

void CPhysicsObject::SetCheckedNowStatus(const bool isCheckedNow)
{
	m_isCheckedNow = isCheckedNow;
}

IPhysicsPrimitive* CPhysicsObject::GetPrimitive()const
{
	return m_primitive;
}

bool CPhysicsObject::Init()
{
	if ((m_body) && (m_geometry))
	{
		// object has been created yet...
		return true;
	}

	if (!m_primitive)
	{
		return false;
	}

	bool isOk = m_primitive->Create(m_body, m_geometry, g->phom.GetWorldID(), g->phom.GetSpaceID());

	if ((m_primitive->GetType() == PHYSICS_OBJECT_TYPE_STATIC_TRIMESH) ||
		(m_primitive->GetType() == PHYSICS_OBJECT_TYPE_STATIC_COLLISION_MESH))
	{
		float x, y, z;
		m_primitive->GetInitialCoords(x, y, z);
		SetCoords(x, y, z);
	}

	if (isOk)
	{
		m_isJustCreated = true;

		if (m_obj3d)
		if (m_obj3d->GetAnimation())
		{
			m_isJustCreated = false;			
		}
		
		dGeomSetData(m_geometry, (void*)m_objectID);
		m_position.Set(m_primitive->GetInitialX(), m_primitive->GetInitialY(), m_primitive->GetInitialZ());
	}

	if (!isOk)
	{
		float rx, ry, rz, rw;
		m_primitive->GetInitialDimensions(rx, ry, rz, rw);
		g->lw.WriteLn("failed create physics: ", rx, " , ", ry, " , ", rz);
	}
	
	g->phom.AddUpdateableObject(this, true);

	UpdateXYBounds();

	return isOk;
}

bool CPhysicsObject::IsJustCreated()const
{
	return m_isJustCreated;
}

 void CPhysicsObject::OnUse()
 {
	 m_isJustCreated = false;
 }

int CPhysicsObject::GetType()const
{
	if (m_primitive)
	{
		return m_primitive->GetType();
	}
	else
	{
		return PHYSICS_OBJECT_TYPE_NONE;
	}
}

void CPhysicsObject::SetType(int type)
{
	if (m_primitive)
	{
		if (PHYSICS_OBJECT_TYPE_STATIC_TRIMESH == m_primitive->GetType())
		{
			g->phom.DeleteTrimesh(this);
		}

		m_primitive->Destroy();
		m_primitive = NULL;
	}

	m_primitive = CreatePhysicsPrimitive(type);

	if (PHYSICS_OBJECT_TYPE_BOX == type)
	{
		m_isStatic = false;
	}
	else if (PHYSICS_OBJECT_TYPE_STATIC_TRIMESH == type)
	{
		g->phom.AddTrimesh(this);
	}
	else if (PHYSICS_OBJECT_TYPE_STATIC_COLLISION_MESH == type)
	{
		g->phom.AddTrimesh(this);
	}
}

void CPhysicsObject::SetRotation(float axisX, float axisY, float axisZ, float angle)
{
	m_isNeedToUpdate = true;
	g->phom.AddUpdateableObject(this);

	dQuaternion q;
	
	angle = fmod(fmod(angle, 360.0f) + 360.0f, 360.0f); // angle in [0;360]

	float r = sqrt(axisX * axisX + axisY * axisY + axisZ * axisZ);
	if(r > 0.0f)
	{
		axisX /= r;
		axisY /= r;
		axisZ /= r;
	}

	float sinAngle = sinf(angle / 180.0f * 3.141592653589f / 2.0f);
	q[0] = cosf(angle / 180.0f * 3.141592653589f / 2.0f);
	q[1] = (axisX * sinAngle);
	q[2] = (axisY * sinAngle);
	q[3] = (axisZ * sinAngle);
	
	if (m_body)
	{	
		dBodySetQuaternion(m_body, q);
	}
	else
	{
		if (m_geometry)
		{
			dGeomSetQuaternion(m_geometry, q);
		}
	}

	UpdateXYBounds();
}

void CPhysicsObject::SetQuaternion(CQuaternion quat)
{
	m_isNeedToUpdate = true;
	g->phom.AddUpdateableObject(this);

	dQuaternion q;
	q[0] = quat.w;
	q[1] = quat.x;
	q[2] = quat.y;
	q[3] = quat.z;

	if (m_body)
	{
		dBodySetQuaternion(m_body, q);
	}
	else
	{
		if (m_geometry)
		{
			dGeomSetQuaternion(m_geometry, q);
		}
	}
}

void CPhysicsObject::Update()
{
	if ((m_isStatic) && (!m_isNeedToUpdate))
	{
		return;
	}

	m_isNeedToUpdate = false;

	if (m_geometry)
	{	
		const dReal* position = dGeomGetPosition(m_geometry); 
		m_position.Set(position[0], position[1], position[2]);

		dQuaternion rotationQuaternion;
		dGeomGetQuaternion(m_geometry, rotationQuaternion);

		if ((m_lastRotation[0] != rotationQuaternion[0]) || (m_lastRotation[1] != rotationQuaternion[1]) ||
			(m_lastRotation[2] != rotationQuaternion[2]) || (m_lastRotation[3] != rotationQuaternion[3]))
		{
			memcpy(m_lastRotation, rotationQuaternion, sizeof(dReal) * 4);

			float rotationVector[3];
			float rotationAngle = 2 * acosf(rotationQuaternion[0]) * 180.0f / 3.141592653589f;
			rotationVector[0] = rotationQuaternion[1];
			rotationVector[1] = rotationQuaternion[2];
			rotationVector[2] = rotationQuaternion[3];

			if ((0.0f == rotationVector[0]) && (0.0f == rotationVector[1]) && (0.0f == rotationVector[2]))
			{
				rotationVector[2] = 1.0f;
			}
		
			m_rotationPack.SetRotating(rotationAngle, rotationVector[0], rotationVector[1], rotationVector[2]);					
		}		
	}

	UpdateXYBounds();
}

void CPhysicsObject::GetCoords(float *x, float *y, float *z)const
{
	assert(x);
	assert(y);
	assert(z);

	*x = m_position.x;
	*y = m_position.y;
	*z = m_position.z;
}

CVector3D CPhysicsObject::GetCoords()const
{
	return m_position;
}

void CPhysicsObject::GetRealCoords(float *x, float *y, float *z)const
{
	if (m_geometry)
	{
		const dReal* position = dGeomGetPosition(m_geometry);
		*x = position[0];
		*y = position[1];
		*z = position[2];
	}
	else
	{
		*x = 0;
		*y = 0;
		*z = 0;
	}
}

CRotationPack* CPhysicsObject::GetRotationPack()
{
	return &m_rotationPack;
}

void CPhysicsObject::AddForce(dReal ffx, dReal ffy, dReal ffz)
{
	if (m_body)
	{
		dBodyAddForce(m_body, ffx, ffy, ffz);
	}
}

void CPhysicsObject::SetForce(dReal ffx, dReal ffy, dReal ffz)
{
	if (m_body)
	{
		dBodySetForce(m_body, ffx, ffy, ffz);
	}
}

void CPhysicsObject::SetCoords(float x, float y, float z)
{
	if ((_isnan(x)) || (_isnan(y)) || (_isnan(z)))
	{
		return;
	}

	if (m_body != 0)
	{
		dBodySetPosition(m_body, x, y, z); 
	}
	else
	{
		if (m_geometry)
		{
			dGeomSetPosition(m_geometry, x, y, z);
		}
	}

	m_position.Set(x, y, z);
}

void CPhysicsObject::SetObjectID(int objectID)
{	
	m_objectID = objectID;	
}

dGeomID CPhysicsObject::GetGeomID()const
{
	return m_geometry;
}

void CPhysicsObject::SetObject3D(C3DObject* object3d)
{
	m_obj3d = object3d;
}

C3DObject* CPhysicsObject::GetObject3D()const
{
	return m_obj3d;
}

void CPhysicsObject::Destroy()
{
	if (m_primitive)
	{
		if (m_body)
		{
			dBodyDestroy(m_body);
			m_body = NULL;
		}

		if (m_geometry)
		{
			dSpaceRemove(*g->phom.GetSpaceID(), m_geometry);
			dGeomDestroy(m_geometry);
			m_geometry = NULL;
		}

		if ((PHYSICS_OBJECT_TYPE_STATIC_TRIMESH == m_primitive->GetType()) ||
			(PHYSICS_OBJECT_TYPE_STATIC_COLLISION_MESH == m_primitive->GetType()))
		{
			g->phom.DeleteTrimesh(this);
		}

		m_primitive->Destroy();		
		m_primitive = NULL;

		m_isNeedToUpdate = false;
	}
}

void CPhysicsObject::SetCollisionCatBits(unsigned long bits)
{
	if (m_geometry)
	{
		dGeomSetCategoryBits(m_geometry, bits);	
	}
}

void CPhysicsObject::SetCollisionColBits(unsigned long bits)
{
	if (m_geometry)
	{
		dGeomSetCollideBits(m_geometry, bits);
	}
}

float CPhysicsObject::GetScalarVelocity()
{
	return m_velocity.GetLength();
}

void CPhysicsObject::GetVelocity(float* x, float* y, float* z)
{
	*x = m_velocity.x;
	*y = m_velocity.y;
	*z = m_velocity.z;
}

void CPhysicsObject::SetVelocity(float vx, float vy, float vz)
{
	if (m_body)
	{
		m_velocity.x += vx;
		m_velocity.y += vy;
		m_velocity.z += vz;

		dBodySetLinearVel(m_body, m_velocity.x, m_velocity.y, m_velocity.z);
	}	
}

void CPhysicsObject::Enable()
{
	if (m_body)
	if (!dBodyIsEnabled(m_body))
	{
		dBodyEnable(m_body);
	}
}

void CPhysicsObject::Disable()
{
	if (m_body)
	if (dBodyIsEnabled(m_body))
	{
		dBodyDisable(m_body);
	}
}

void CPhysicsObject::SetVelocityToMove(float deltaX, float deltaY, float deltaZ)
{	
	SetVelocity(deltaX / (float)STEP_TIME, deltaY / (float)STEP_TIME, deltaZ / (float)STEP_TIME);
}

void CPhysicsObject::ClearVelocity()
{
	if (!((0.0f == m_velocity.x) && (0.0f == m_velocity.y) && (0.0f == m_velocity.z)))
	{
		m_velocity.Set(0.0f, 0.0f, 0.0f);
		
		SetVelocity(0.0f, 0.0f, 0.0f);
	}

	if (m_body)
	{
		dBodySetAngularVel(m_body, 0, 0, 0);
	}
}

void CPhysicsObject::MarkCollision()
{
	if (m_obj3d)
	{
		m_obj3d->SetUserData(USER_DATA_WAS_COLLISION, (void *)1);
	}
}

void CPhysicsObject::UnmarkCollision()
{
	if (m_obj3d)
	{
		m_obj3d->SetUserData(USER_DATA_WAS_COLLISION, 0);
	}
}

bool CPhysicsObject::TraceRayZ(float _x, float _y, float* _z)
{
	if (!m_primitive)
	{
		return false;
	}

	if ((m_primitive->GetType() != PHYSICS_OBJECT_TYPE_STATIC_TRIMESH) &&
		(m_primitive->GetType() != PHYSICS_OBJECT_TYPE_STATIC_COLLISION_MESH))
	{
		return false;
	}

	CRayTraceTriMesh* triMesh = (CRayTraceTriMesh*)m_primitive->GetUserData();
	if (!triMesh)
	{
		return false;
	}

	if (m_primitive->GetType() == PHYSICS_OBJECT_TYPE_STATIC_COLLISION_MESH)
	{
		triMesh->SetScale(m_obj3d->GetScale());
	}

	float x, y, z;
	m_obj3d->GetCoords(&x, &y, &z);
	
	bool res = triMesh->RayTraceZ(_x - x , _y - y , _z);
	*_z += z;

	return res;
}

bool CPhysicsObject::CheckRayIntersect(CVector3D& rayStart, CVector3D& rayDX)
{
	if (!m_primitive)
	{
		return false;
	}

	if (m_primitive->GetType() != PHYSICS_OBJECT_TYPE_STATIC_TRIMESH)
	{
		return false;
	}

	CRayTraceTriMesh* triMesh = (CRayTraceTriMesh*)m_primitive->GetUserData();
	if (!triMesh)
	{
		return false;
	}

	CVector3D rayOrigin = rayStart - m_obj3d->GetCoords();

	

	//
	/*CVector3D rayEnd = rayOrigin + rayDX;
	CQuaternion q = GetRotationPack()->GetAsDXQuaternion();
	q.Conjugate();
	rayEnd *= q;
	rayOrigin *= q;
	rayDX = rayEnd - rayOrigin; */
	//

	return triMesh->CheckRayIntersect(rayOrigin, rayDX);
}

void CPhysicsObject::UpdateXYBounds()
{
	if (!m_primitive)
	{
		return;
	}

	if (m_primitive->GetType() != PHYSICS_OBJECT_TYPE_STATIC_BOX)
	{
		return;
	}

	m_minX = FLT_MAX;
	m_minY = FLT_MAX;
	m_maxX = FLT_MIN;
	m_maxY = FLT_MIN;

	CQuaternion q = m_rotationPack.GetAsDXQuaternion();
	q.Normalize();

	float xSize, ySize, zSize, wSize;
	m_primitive->GetInitialDimensions(xSize, ySize, zSize, wSize);

	CVector3D tmp[8];
	int index = 0;
	for (int _x = -1; _x <= 1; _x += 2)
	for (int _y = -1; _y <= 1; _y += 2)
	for (int _z = -1; _z <= 1; _z += 2)
	{
		tmp[index].Set(_x * xSize, _y * ySize, _z * zSize);
		tmp[index] *= q;
		index++;
	}

	for (index = 0; index < 8; index++)	
	{
		if (tmp[index].x < m_minX)
		{
			m_minX = tmp[index].x;
		}

		if (tmp[index].y < m_minY)
		{
			m_minY = tmp[index].y;
		}

		if (tmp[index].x > m_maxX)
		{
			m_maxX = tmp[index].x;
		}

		if (tmp[index].y > m_maxY)
		{
			m_maxY = tmp[index].y;
		}
	}
}

float CPhysicsObject::GetMaxLinearSize()
{
	if (!m_primitive)
	{
		return 0;
	}

	if (m_primitive->GetType() != PHYSICS_OBJECT_TYPE_STATIC_BOX)
	{
		return 0;
	}

	if (m_maxX - m_minX > m_maxY - m_minY)
	{
		return m_maxX - m_minX;
	}
	else
	{
		return m_maxY - m_minY;
	}
}

CPhysicsObject::~CPhysicsObject()
{
	Destroy();
}