
#include "StdAfx.h"
#include <Assert.h>
#include "userdata.h"
#include "PhysicsParams.h"
#include "ControlledObject.h"
#include "GlobalSingletonStorage.h"
#include "cal3d/memory_leak.h"

#define MAX_STEP_VELOCITY				/*10000.0f*/7500.0f
#define SMALL_STEP_SIZE_X				10.0f
#define SMALL_STEP_SIZE_Y				20.0f

CControlledObject::CControlledObject():
	m_x(0.0f),
	m_y(0.0f),
	m_z(0.0f),
	m_x2(0.0f),
	m_y2(0.0f),
	m_lastDX(0),
	m_lastDY(0),
	m_z2(-200.0f),
	m_physObj(NULL),
	m_lastAnimation(NULL),
	m_objectID(0xFFFFFFFF),
	m_isGravitationEnabled(true),
	m_isForwardMoveDirection(true),
	m_halfHeight(AVATAR_HALF_HEIGHT)	
{
	int k;
	m_stepVelocity.Set(0, 0, 0);

	for (k = 0; k < RIGHT_COORDS_COUNT; k++)
	{
		m_lastRightCoords[k].Set(0, 0, 0);
	}
	for (k = 0; k < POS_QUEUE_SIZE; k++)
	{
		m_posQueue[k].Set(0, 0, 0);
	}	
}

void CControlledObject::InitRays()
{
	m_z2 = -200.0f;
}

void CControlledObject::SetObjectID(unsigned int objectID)
{
	assert(objectID < (unsigned int)g->phom.GetCount());

	m_objectID = objectID;
	m_physObj = g->phom.GetObjectPointer(m_objectID);
	for (int k = 0; k < RIGHT_COORDS_COUNT; k++)
	{
		m_physObj->GetCoords(&m_lastRightCoords[k].x, &m_lastRightCoords[k].y, &m_lastRightCoords[k].z);
	}
}

unsigned int CControlledObject::GetObjectID()const
{
	return m_objectID;
}

void CControlledObject::OnBeforeSimulation()
{
	assert(m_physObj);
	if (!m_physObj)
	{
		return;
	}

	g->phom.SetCollisionStatus(false, "");

	m_physObj->GetRealCoords(&m_x, &m_y, &m_z);

	if ((m_halfHeight == AVATAR_HALF_HEIGHT) || (m_halfHeight == 0.0f) || (m_lastAnimation != m_physObj->GetObject3D()->GetAnimation()))
	{
		float minX, minY, minZ, maxX, maxY, maxZ;
		m_physObj->GetObject3D()->GetBoundingBox(minX, minY, minZ, maxX, maxY, maxZ);
		m_halfHeight = maxZ / 2.0f;
		m_lastAnimation = m_physObj->GetObject3D()->GetAnimation();
	}
	
	CVector3D smallSizeVec(SMALL_STEP_SIZE_X, SMALL_STEP_SIZE_Y, 0);
	smallSizeVec *= m_physObj->GetObject3D()->GetRotation()->GetAsDXQuaternion();
    	
	g->phom.SetRayOptions(0, CVector3D(m_x, m_y, RAY_MAX_Z), CVector3D(0, 0, -RAY_LENGTH)); 		
	g->phom.SetRayOptions(1, CVector3D(m_x + smallSizeVec.x, m_y - smallSizeVec.y, RAY_MAX_Z), CVector3D(0, 0, -RAY_LENGTH)); 		
	g->phom.SetRayOptions(2, CVector3D(m_x + smallSizeVec.x, m_y + smallSizeVec.y, RAY_MAX_Z), CVector3D(0, 0, -RAY_LENGTH)); 		
	g->phom.SetRayOptions(3, CVector3D(m_x - smallSizeVec.x, m_y - smallSizeVec.y, RAY_MAX_Z), CVector3D(0, 0, -RAY_LENGTH)); 		
	g->phom.SetRayOptions(4, CVector3D(m_x - smallSizeVec.x, m_y + smallSizeVec.y, RAY_MAX_Z), CVector3D(0, 0, -RAY_LENGTH)); 				
	
	g->phom.SetRayZ(MIN_RAYTRACE_Z, NULL);
	g->phom.SetJustCreatingFlag(false);
	
	dGeomGetQuaternion(m_physObj->GetGeomID(), m_d);

	CQuaternion q(m_d[1], m_d[2], m_d[0], m_d[3]);
	CVector3D axe; float angle;
	q.Extract(axe, angle);

	// защита от переворотов аватара
	if (m_physObj->GetObject3D())
	if ((axe.x != 0) || (axe.y != 0))
	if (!m_physObj->GetObject3D()->GetParent())
	{
		dQFromAxisAndAngle(m_d, 0, 0, 1, 0);
	}

	//g->lw.WriteLn("Before simulation: ", m_x, " ", m_y, " ", m_z);
}

bool CControlledObject::IsMoved()const
{
	float x, y, z;
	m_physObj->GetRealCoords(&x, &y, &z);

	bool isMoved = ((m_physObj->GetScalarVelocity() != 0.0f) || (m_lastRightCoords[0].x != x) 
		|| (m_lastRightCoords[0].y != y) || (m_lastRightCoords[0].z != z));

	return isMoved;
}

bool CControlledObject::IsRotated()const
{
	dQuaternion q;
	dGeomGetQuaternion(m_physObj->GetGeomID(), q);

	return (q != m_d);	
}

bool CControlledObject::IsMovedOrRotated()const
{
	return ((IsMoved()) || (IsRotated()));
}

bool CControlledObject::IsOnGround()const
{
	return (m_z2 > -MIN_Z);
}

bool CControlledObject::IsNotFly()
{
#define EPS 25.0f

	if (m_physObj->GetObject3D())
	if (m_physObj->GetObject3D()->GetParent())
	{
		return true;
	}

	float z = g->phom.GetRayZ() + m_halfHeight;

	return (m_z2 - z < EPS);
}

int CControlledObject::GetStepCount()
{
	int stepCount = (int)(m_physObj->GetScalarVelocity() / MAX_STEP_VELOCITY);
	if (0 == stepCount)
	{
		stepCount = 1;
	}

	return stepCount;
}

void CControlledObject::CalculateStepVelocity()
{
	m_physObj->GetVelocity(&m_stepVelocity.x, &m_stepVelocity.y, &m_stepVelocity.z);

	m_stepVelocity /= (float)GetStepCount();

	if (m_isGravitationChanged)
	{
		m_physObj->GetRealCoords(&m_x, &m_y, &m_z);	
		m_physObj->GetRealCoords(&m_x2, &m_y2, &m_z2);	
	}
}

void CControlledObject::SetStepVelocity()
{
	m_physObj->ClearVelocity();
	m_physObj->SetVelocity(m_stepVelocity.x, m_stepVelocity.y, m_stepVelocity.z);	
}

void CControlledObject::AvgTraceZ(CPhysicsObject* obj, float* z)
{
	float z1 = MIN_RAYTRACE_Z, z2 = MIN_RAYTRACE_Z, z3 = MIN_RAYTRACE_Z, z4 = MIN_RAYTRACE_Z, z5 = MIN_RAYTRACE_Z, _z;

	if (!obj->GetObject3D()->IsVisible())
	{
		*z = MIN_RAYTRACE_Z;
		return;
	}

	if (obj->TraceRayZ(m_x, m_y, &_z))
	{
		z1 = _z;
	}

	CVector3D smallSizeVec(SMALL_STEP_SIZE_X, SMALL_STEP_SIZE_Y, 0);
	smallSizeVec *= m_physObj->GetObject3D()->GetRotation()->GetAsDXQuaternion();

	if (obj->TraceRayZ(m_x + smallSizeVec.x, m_y + smallSizeVec.y, &_z))
	{
		z2 = _z;
	}

	if (obj->TraceRayZ(m_x + smallSizeVec.x, m_y - smallSizeVec.y, &_z))
	{
		z3 = _z;
	}

	if (obj->TraceRayZ(m_x - smallSizeVec.x, m_y - smallSizeVec.y, &_z))
	{
		z4 = _z;
	}

	if (obj->TraceRayZ(m_x - smallSizeVec.x, m_y + smallSizeVec.y, &_z))
	{
		z5 = _z;
	}

	*z = max(z1, z2);
	*z = max(*z, z3);
	*z = max(*z, z4);
	*z = max(*z, z5);
}

void CControlledObject::SetForwardMoveDirection(bool isForward)
{
	m_isForwardMoveDirection = isForward;
}

bool CControlledObject::IsForwardMoveDirection()const
{
	return m_isForwardMoveDirection;
}

void CControlledObject::DetectCycle()
{
	m_physObj->GetRealCoords(&m_x2, &m_y2, &m_z2);

	for (int i = 0; i < POS_QUEUE_SIZE - 1; i++)
	{
		m_posQueue[i] = m_posQueue[i + 1];
	}

	m_posQueue[POS_QUEUE_SIZE - 1].x = m_x2;
	m_posQueue[POS_QUEUE_SIZE - 1].y = m_y2;
	m_posQueue[POS_QUEUE_SIZE - 1].z = 0;

	if (m_posQueue[POS_QUEUE_SIZE - 1] == m_posQueue[POS_QUEUE_SIZE - 2])
	{
		return;
	}

	for (int i = 0; i < POS_QUEUE_SIZE - 2; i++)
	if (m_posQueue[i] != m_posQueue[i + 2])
	{
		return;
	}

	GetObject3D()->OnChanged(2);
}

void CControlledObject::ReturnLastCorrectCoords(float& dx, float& dy, bool isNotDefinedDX)
{
	bool isPairCycleDetected = true;

	for (int k = 0; k < RIGHT_COORDS_COUNT; k++)
	if ((k + 2 >= RIGHT_COORDS_COUNT) || (m_lastRightCoords[k] != m_lastRightCoords[k + 2]))
	{
		isPairCycleDetected = false;
	}
	
	//bool isFound = false;
	if (!isPairCycleDetected)
	for (int k = 0; k < RIGHT_COORDS_COUNT; k++)
	if ((m_lastRightCoords[k].x != m_x) || (m_lastRightCoords[k].y != m_y))
	{
		m_physObj->SetCoords(m_lastRightCoords[k].x, m_lastRightCoords[k].y, m_lastRightCoords[k].z);
		//isFound = true;
		break;				
	}
	if ((isPairCycleDetected) && (!isNotDefinedDX))//(!isFound)
	{
		m_physObj->SetCoords(m_x + dx * AVATAR_RETURN_PATH, m_y + dy * AVATAR_RETURN_PATH, m_z);
	}
}

void CControlledObject::OnAfterSimulation()
{
	DetectCycle();

	if (g->phom.GetJustCreatingFlag() == 1)
	{
		if (g->phom.GetJustCreatingObject())
		if (g->phom.GetJustCreatingObject()->GetObject3D())
		{
			if (g->phom.GetJustCreatingObject()->GetObject3D()->GetUserData(USER_DATA_NRMOBJECT) != NULL)
			{
				GetObject3D()->OnChanged(2);	
			}
			else if (((int)g->phom.GetJustCreatingObject()->GetObject3D()->GetUserData(USER_DATA_LEVEL))%2 != 1)
			{
				GetObject3D()->OnChanged(2);	
			}
		}
	}
		
	assert(m_physObj);

	std::vector<CPhysicsObject *>* trimeshes = g->phom.GetTrimeshList();
	std::vector<CPhysicsObject *>::iterator it = trimeshes->begin();
	std::vector<CPhysicsObject *>::iterator itEnd = trimeshes->end();

	for ( ; it != itEnd; it++)
	{
		float _z;
		AvgTraceZ((*it), &_z);
		if (_z > g->phom.GetRayZ())
		{
			g->phom.SetRayZ(_z, *it);
			g->phom.SetJustCreatingFlag(2);
		}
	}

	m_physObj->GetRealCoords(&m_x2, &m_y2, &m_z2);	
	
	dGeomSetQuaternion(m_physObj->GetGeomID(), m_d);

	if (!m_isGravitationEnabled)
	{
		m_physObj->SetCoords(m_x2, m_y2, m_z);
		return;
	}

	CQuaternion quat;
	CVector3D vec1;	

	bool isNotDefinedDX = false;
	float dx = (m_x - m_x2);
	float dy = (m_y - m_y2);
	float r = sqrt(dx * dx + dy * dy);
	if (r != 0.0f)	
	{
		dx = dx / r;
		dy = dy / r;
		m_lastDX = dx;
		m_lastDY = dy;		
	}
	else
	{
		isNotDefinedDX = true;

		vec1 = ((CSkeletonAnimationObject *)(m_physObj->GetObject3D()->GetAnimation()))->GetRotatedFaceVector();

		dx = -vec1.x;
		dy = -vec1.y;

		if (!m_isForwardMoveDirection)
		{
			dx *= -1;
			dy *= -1;
		}

		float r = sqrt(dx * dx + dy * dy);
		if (r != 0.0f)
		{
			dx = dx / r;
			dy = dy / r;
		}		
	}	
			
	if (m_halfHeight != 0.0f)
	{
		// аватар ходил-ходил и оказался под землей?
		float rayZ = g->phom.GetRayZ();

		bool if1 = ((rayZ + m_halfHeight < -MIN_Z) && (m_z > -MIN_Z) && (!((m_x - m_x2 == 0.0f) && (m_y - m_y2 == 0.0f) && (g->phom.GetJustCreatingFlag() > 0))));
		// аватар идет и не может вскарабкаться вверх?
		bool if2 = (((rayZ + m_halfHeight > m_z + AVATAR_MAX_DELTA_Z) && (m_z > MIN_Z) 
			&& (!((m_x == m_x2) && (m_y == m_y2) && (g->phom.GetJustCreatingFlag() > 0)))));		
		if (if1)
		{			
			m_physObj->MarkCollision();

			std::string name = "";
			if (g->phom.GetRayObject())
			if (g->phom.GetRayObject()->GetObject3D())
			{
				USES_CONVERSION;
				name = W2A( g->phom.GetRayObject()->GetObject3D()->GetName());
			}

			g->phom.SetCollisionStatus(true, name);			
			ReturnLastCorrectCoords(dx, dy, isNotDefinedDX);
			m_physObj->Disable();
		}
		else if (if2)
		{			
			m_physObj->MarkCollision();

			std::string name = "";
			if (g->phom.GetRayObject())
			if (g->phom.GetRayObject()->GetObject3D())
			{
				USES_CONVERSION;
				name = W2A( g->phom.GetRayObject()->GetObject3D()->GetName());
			}

			g->phom.SetCollisionStatus(true, name);
			m_physObj->SetCoords(m_x + dx * AVATAR_RETURN_PATH, m_y + dy * AVATAR_RETURN_PATH, m_z);
			m_physObj->Disable();
		}
		else if (g->phom.GetCollisionStatus())
		{			
			//m_physObj->MarkCollision();
			//g->phom.SetCollisionStatus(true, "");
			//m_physObj->SetCoords(m_x + dx * AVATAR_RETURN_PATH, m_y + dy * AVATAR_RETURN_PATH, /*g->phom.GetRayZ() + m_halfHeight*/m_lastRightCoords[0].z);
			//m_physObj->Disable();
		}
		else
		{
			float z = g->phom.GetRayZ() + m_halfHeight;
			
			m_physObj->UnmarkCollision();
			m_physObj->SetCoords(m_x2, m_y2, z);
			m_physObj->Enable();
		}
	}

	m_physObj->GetRealCoords(&m_x2, &m_y2, &m_z2);
	
	if ((fabsf(m_x - m_x2) > UNREAL_DELTA) || (fabsf(m_y - m_y2) > UNREAL_DELTA) || (fabsf(m_z - m_z2) > UNREAL_DELTA))
	{
		m_physObj->SetCoords(m_x, m_y,  m_z);
	}

	m_physObj->GetRealCoords(&m_x2, &m_y2, &m_z2);
	if ((m_z2 > MAX_UNREAL_Z) || (m_z2 < MIN_UNREAL_Z) || (_isnan(m_x2) || (_isnan(m_y2))))
	{
		g->lw.WriteLn("Restore last right coords");
		m_physObj->SetCoords(m_lastRightCoords[0].x, m_lastRightCoords[0].y, m_lastRightCoords[0].z);
	}
	else
	{
		if ((m_lastRightCoords[0].x != m_x2) || (m_lastRightCoords[0].y != m_y2))
		{
			for (int k = RIGHT_COORDS_COUNT - 1; k > 0; k--)
			{
				m_lastRightCoords[k] = m_lastRightCoords[k - 1];
			}
			m_lastRightCoords[0].x = m_x2;
			m_lastRightCoords[0].y = m_y2;			
		}

		m_lastRightCoords[0].z = m_z2;
	}

	m_physObj->GetRealCoords(&m_x2, &m_y2, &m_z2);
	//g->lw.WriteLn("After simulation: ", m_x2, " ", m_y2, " ", m_z2);

	m_physObj->ClearVelocity();
}

float CControlledObject::GetHalfHeight()const
{
	return m_halfHeight;
}

void CControlledObject::ChangeGravitation(bool isGravitationEnabled)
{
	if (m_isGravitationEnabled != isGravitationEnabled)
	{
		m_isGravitationEnabled = isGravitationEnabled;
		m_isGravitationChanged = true;
	}
}

C3DObject* CControlledObject::GetObject3D()
{
	if (m_physObj)
	{
		return m_physObj->GetObject3D();
	}
	else
	{
		return NULL;
	}
}

void CControlledObject::OnStartMoving()
{	
	m_lastDX = 0;
	m_lastDY = 0;
}

bool CControlledObject::IsGravitationChanged()
{
	bool isGravitationChanged = m_isGravitationChanged;
	m_isGravitationChanged = false;
	return isGravitationChanged;
}

CControlledObject::~CControlledObject()
{
}