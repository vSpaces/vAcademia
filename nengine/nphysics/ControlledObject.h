
#pragma once

#include "PhysicsObject.h"

#define RIGHT_COORDS_COUNT	5
#define POS_QUEUE_SIZE		8

class CControlledObject
{
public:
	CControlledObject();
	~CControlledObject();

	void InitRays();

	void SetObjectID(unsigned int objectID);
	unsigned int GetObjectID()const;

	void OnBeforeSimulation();
	void OnAfterSimulation();

	void SetStepVelocity();
	void CalculateStepVelocity();

	int GetStepCount();

	float GetHalfHeight()const;

	bool IsMoved()const;
	bool IsRotated()const;
	bool IsMovedOrRotated()const;
	bool IsOnGround()const;
	bool IsNotFly();

	void ChangeGravitation(bool isGravitationEnabled);
	bool IsGravitationChanged();
	
	C3DObject* GetObject3D();

	void SetForwardMoveDirection(bool isForward);
	bool IsForwardMoveDirection()const;

	void OnStartMoving();

private:
	void AvgTraceZ(CPhysicsObject* obj, float* z);

	void DetectCycle();

	void ReturnLastCorrectCoords(float& dx, float& dy, bool isNotDefinedDX);

	float m_x, m_y, m_z;
	float m_x2, m_y2, m_z2;

	unsigned int m_objectID;
	CPhysicsObject* m_physObj;
	CAnimationObject* m_lastAnimation;

	bool m_isGravitationEnabled;
	bool m_isGravitationChanged;
	bool m_isForwardMoveDirection;

	float m_halfHeight;

	float m_lastDX;
	float m_lastDY;

	CVector3D m_lastRightCoords[RIGHT_COORDS_COUNT];
	CVector3D m_stepVelocity;

	CVector3D m_posQueue[POS_QUEUE_SIZE];

	dQuaternion m_d;	
};