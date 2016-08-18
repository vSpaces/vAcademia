
#pragma once

#include "Vector3D.h"
#include "3DObject.h"

#define CAMERA_TARGET_OBJECT		0
#define CAMERA_TARGET_POINT			1

class CCameraTarget
{
public:
	CCameraTarget();
	~CCameraTarget();

	unsigned char GetType()const;

	void SetViewTarget(C3DObject* const viewTarget);
	void SetViewPoint(const CVector3D& viewPoint);

	CVector3D GetPoint()const;
	C3DObject* GetObject()const;
	C3DObject* GetAttachObject()const;

	CVector3D GetViewPosition();

	void SetViewOffset(const CVector3D& offset);
	CVector3D GetViewOffset()const;

	bool IsEmptyViewOffset()const;
	
	float GetAddZ();

private:
	float m_lastAddZ;

	CVector3D m_offset;
	CVector3D m_viewPoint;
	C3DObject* m_viewTarget;

	unsigned char m_type;
};