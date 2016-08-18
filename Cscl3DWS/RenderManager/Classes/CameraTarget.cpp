
#include "StdAfx.h"
#include "CameraTarget.h"

// "дефолтное" расстояние от стоп ног до лица (аватар стоит)
#define ADD_Z								160.0f
#define ADD_Z_HALF							70.0f
// стандартный рост аватара
#define STANDARD_HEIGHT						180.0f
// 
#define HEAD_KOEF							(ADD_Z / STANDARD_HEIGHT)
// минимальное изменение Z, на которое реагируем
#define MIN_DELTA_Z							25.0f

CCameraTarget::CCameraTarget():
	m_type(CAMERA_TARGET_POINT),
	m_viewPoint(0, 0, 0),
	m_viewTarget(NULL),
	m_lastAddZ(ADD_Z)	
{
}

unsigned char CCameraTarget::GetType()const
{
	return m_type;
}

void CCameraTarget::SetViewTarget(C3DObject* const viewTarget)
{
	m_viewTarget = viewTarget;
	if (m_viewTarget)
	{
		m_type = CAMERA_TARGET_OBJECT;
	}
	else
	{
		m_type = CAMERA_TARGET_POINT;
	}
}

void CCameraTarget::SetViewPoint(const CVector3D& viewPoint)
{
	m_viewPoint = viewPoint;
	m_viewTarget = NULL;
	m_type = CAMERA_TARGET_POINT;
}

CVector3D CCameraTarget::GetPoint()const
{
	return m_viewPoint;
}

C3DObject* CCameraTarget::GetObject()const
{
	return m_viewTarget;
}

float CCameraTarget::GetAddZ()
{
	if ((m_type != CAMERA_TARGET_OBJECT) || (!GetAttachObject()))
	{
		if ((m_viewTarget) && (m_viewTarget->GetAnimation()))
		{
			CSkeletonAnimationObject* sao = (CSkeletonAnimationObject *)m_viewTarget->GetAnimation();
			if (!sao->IsUsualOrder())
			{
				return ADD_Z_HALF;
			}
		}

		return ADD_Z;
	}

	float x1, y1, z1, x2, y2, z2;
	GetObject()->GetBoundingBox(x1, y1, z1, x2, y2, z2);

	float height = z2;
	height = height * HEAD_KOEF;

	if (fabsf(height + z1 - m_lastAddZ) > MIN_DELTA_Z)
	{
		m_lastAddZ = height + z1;
	}

	return m_lastAddZ;
}

CVector3D CCameraTarget::GetViewPosition()
{
	switch (m_type)
	{
	case CAMERA_TARGET_POINT:
		return m_viewPoint + m_offset;

	case CAMERA_TARGET_OBJECT:
		return m_viewTarget->GetCoords() + m_offset + CVector3D(0, 0, GetAddZ());
	}

	return CVector3D(0, 0, 0);
}

void CCameraTarget::SetViewOffset(const CVector3D& offset)
{
	m_offset = offset;
}

CVector3D CCameraTarget::GetViewOffset()const
{
	return m_offset;
}

bool CCameraTarget::IsEmptyViewOffset()const
{
	return ((m_offset.x == 0.0f) && (m_offset.y == 0.0f) && (m_offset.z == 0.0f));
}

C3DObject* CCameraTarget::GetAttachObject()const
{
	switch (m_type)
	{
	case CAMERA_TARGET_POINT:
		return NULL;

	case CAMERA_TARGET_OBJECT:
		return m_viewTarget->GetParent();
	}

	return NULL;
}

CCameraTarget::~CCameraTarget()
{
}