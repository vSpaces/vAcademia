
#pragma once

#pragma warning(push)
#pragma warning(disable : 4100)
#include <ode.h>
#pragma warning(pop)

#include "PhysicsPrimitiveTypes.h"
#include "BasePhysicsPrimitive.h"
#include "RayTraceTriMesh.h"
#include "CommonObject.h"
#include "RotationPack.h"
#include "3DObject.h"

#define STEP_TIME			0.005

class CPhysicsObject : public CCommonObject  
{
public:
	CPhysicsObject();
	~CPhysicsObject();

	void Enable();
	void Disable();

	void SetVelocity(float vx, float vy, float vz);
	void SetInitialVelocity(float vx, float vy, float vz);
	void SetVelocityToMove(float deltaX, float deltaY, float deltaZ);
	float GetScalarVelocity();
	void GetVelocity(float* x, float* y, float* z);
	void ClearVelocity();

	void SetCollisionColBits(unsigned long bits);
	void SetCollisionCatBits(unsigned long bits);

	void SetObject3D(C3DObject* obj);
	C3DObject* GetObject3D()const;
	
	void Destroy();
	
	void SetForce(dReal ffx, dReal ffy, dReal ffz);
	void AddForce(dReal ffx, dReal ffy, dReal ffz);

	void SetObjectID(int objectID);

	dGeomID GetGeomID()const;

	void SetCoords(float x, float y, float z);
	void GetCoords(float* x, float* y, float* z)const;
	CVector3D GetCoords()const;
	void GetRealCoords(float *x, float *y, float *z)const;
	
	void SetQuaternion(CQuaternion quat);
	void SetRotation(float axisX, float axisY, float axisZ, float angle);
	CRotationPack* GetRotationPack();

	void Update();
	
	void SetType(int type);
	int GetType()const;

	IPhysicsPrimitive* GetPrimitive()const;

	void OnUse();
	bool IsJustCreated()const;

	bool Init();

	void MarkCollision();
	void UnmarkCollision();

	bool TraceRayZ(float _x, float _y, float* _z);
	bool CheckRayIntersect(CVector3D& rayStart, CVector3D& rayDX);

	__forceinline bool IsPointInXY(float x, float y)const
	{
		return ((x >= m_minX) && (x <= m_maxX) && (y >= m_minY) && (y <= m_maxY));
	}

	float GetMaxLinearSize();

	__forceinline bool IsCheckedNow()
	{
		return m_isCheckedNow;
	}

	void SetCheckedNowStatus(const bool isCheckedNow);

private:
	void UpdateXYBounds();

	dBodyID m_body;
    dGeomID m_geometry;
	dQuaternion m_lastRotation;
	CBasePhysicsPrimitive* m_primitive;

	int m_objectID;
	C3DObject* m_obj3d;
	
	bool m_isVelocitySet;
	bool m_isJustCreated;
	bool m_isStatic;
	bool m_isNeedToUpdate;
	bool m_isCheckedNow;

	float m_minX, m_minY;
	float m_maxX, m_maxY;

	CVector3D m_position;
	CRotationPack m_rotationPack;

	CVector3D m_velocity;
};

