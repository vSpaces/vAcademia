
#include "stdafx.h"
#include "camera.h"
#include "GlobalSingletonStorage.h"
#include "cal3d/memory_leak.h"

#define _USE_MATH_DEFINES
#include "math.h"

CCamera3D::CCamera3D():
	m_yaw(0.0f),
	m_pitch(0.0f), 
	m_roll(0.0f),
	m_additionalPos(0, 0, 0),
	m_isAdditionalPosSet(false),
	m_fov(0.0f),
	m_zOffset(0.0f),
	m_nearPlane(10.0f),
	m_isLeftEye(false),
	m_linkToObject(NULL),
	m_farPlane(125000.0f),
	m_up(0.0f, 0.0f, 1.0f),
	m_eye(0.0f, 0.0f, 0.0f),
	m_look(0.0f, 1.0f, 0.0f),
	m_offset(2.0f),
	m_addOculusLookA(0)
{

}

void CCamera3D::IncOffset()
{
	m_offset += 1.0f;
	SetWindowText(g->ne.ghWnd, IntToStr((int)m_offset).c_str());
}

void CCamera3D::DecOffset()
{
	m_offset -= 1.0f;
	SetWindowText(g->ne.ghWnd, IntToStr((int)m_offset).c_str());
}

float CCamera3D::DegreesToRadians(const float angle) const
{
	return (3.14159265f / 180.0f * angle);
}

float* CCamera3D::GetMatrix()
{
	return &m_matrix[0];
}

void CCamera3D::SetZOffset(const float zOffset)
{
	m_zOffset = zOffset;
}

float CCamera3D::GetZOffset()const
{
	return m_zOffset;
}

void CCamera3D::SetLeftEye(bool isLeftEye)
{
	m_isLeftEye = isLeftEye;
}

void CCamera3D::SetModelViewMatrix()
{
	CVector3D tmpVec(m_eye.x, m_eye.y, m_eye.z + m_zOffset);

	CVector3D oldN = m_n;
	CVector3D oldU = m_u;
	CVector3D oldV = m_v;

	if (!m_isAdditionalPosSet)
	{
		CalcNormalFromLook(m_n, m_look);
		CalcUVFromNormal(m_u, m_v, m_n);
	} else
	{
		CalcNormalFromLookWithEyeOffset(m_n, m_u, m_look, m_isLeftEye, m_offset);
		RotateByVector(m_n, m_up, m_addOculusLookA);
		CalcUVFromNormal(m_u, m_v, m_n);
		CalcAdditionalRot(m_n, m_u, m_v, -m_yaw, -m_roll, m_pitch);
	}
	
	m_matrix[0] = m_u.x;
	m_matrix[1] = m_v.x;
	m_matrix[2] = m_n.x;		
	m_matrix[3] = 0.0f;
	m_matrix[4] = m_u.y;
	m_matrix[5] = m_v.y;
	m_matrix[6] = m_n.y;
	m_matrix[7] = 0.0f;
	m_matrix[8] = m_u.z;
	m_matrix[9] = m_v.z;
	m_matrix[10] = m_n.z;
	m_matrix[11] = 0.0f;
	m_matrix[12] = -tmpVec.Dot(m_u);
	m_matrix[13] = -tmpVec.Dot(m_v);
	m_matrix[14] = -tmpVec.Dot(m_n);
	m_matrix[15] = 1.0f;

	GLFUNC(glMatrixMode)(GL_MODELVIEW);
	GLFUNC(glLoadMatrixf)(m_matrix);

	m_u = oldU;
	m_n = oldN;
	m_v = oldV;
}

void CCamera3D::CalcNormalFromLook(CVector3D & n, CVector3D const& look, bool doNormalize) const
{
	n.Set(m_eye.x - look.x, m_eye.y - look.y, m_eye.z - look.z - m_zOffset * 2.0f);
	if (doNormalize)
		n.Normalize();
}

void CCamera3D::CalcNormalFromLookWithEyeOffset(CVector3D & n, CVector3D  const& u, CVector3D const& look, bool isLeftEye, float offset) const
{
	if (isLeftEye)
	{
		CalcNormalFromLook(n, look + u * offset);			
	}
	else
	{
		CalcNormalFromLook(n, look - u * offset);
	}
}

void CCamera3D::CalcUVFromNormal(CVector3D & u, CVector3D & v, CVector3D const& n) const
{
	u.Set(m_up.Cross(n));
	u.Normalize();
	v.Set(n.Cross(u));
	v.Normalize();
}

void CCamera3D::CalcAdditionalRotPart(CVector3D & v1, CVector3D & v2, float a) const
{
	float cs = cos(a);
	float sn = sin(a);

	CVector3D t = v1;

	v1.Set(cs * t.x - sn * v2.x, cs * t.y - sn * v2.y, cs * t.z - sn * v2.z);
	v2.Set(sn * t.x + cs * v2.x, sn * t.y + cs * v2.y, sn * t.z + cs * v2.z);
}

void CCamera3D::CalcAdditionalRot(CVector3D & n, CVector3D & u, CVector3D & v, float yaw, float roll, float pitch) const
{
	CalcAdditionalRotPart(n, u, yaw);
	CalcAdditionalRotPart(u, v, roll);
	CalcAdditionalRotPart(n, v, pitch);
}

void CCamera3D::RotateByVector(CVector3D & v, CVector3D const& n, float a) const
{
	CQuaternion rot(n, a);
	v *= rot;
}

CVector3D CCamera3D::GetExtractedLookAt() const
{
	CVector3D n;
	CalcNormalFromLook(n, m_look);

	if (m_isAdditionalPosSet)
	{
		RotateByVector(n, m_up, m_addOculusLookA);
		CVector3D u;
		CVector3D v;
		CalcUVFromNormal(u, v, n);
		CalcAdditionalRot(n, u, v, -m_yaw, -m_roll, m_pitch);
	}

	CVector3D look;
	CalcNormalFromLook(look, n, false);
	return look;	 
}

void CCamera3D::SetOculusLookAt(const CVector3D& look) 
{
	CVector3D nNew;
	CalcNormalFromLook(nNew, look);
	CVector3D nCurrent;
	CalcNormalFromLook(nCurrent, GetExtractedLookAt());
	m_addOculusLookA = nNew.GetAngleSigned(nCurrent, m_up);
}

void CCamera3D::ResetOculusLookAt()
{
	m_addOculusLookA = 0;
}

void CCamera3D::SetAdditionalPos(const CVector3D& pos, const float yaw, const float pitch, const float roll)
{
	/*SetEyePosition(m_eye - m_additionalPos);
	CVector3D n = CVector3D(m_n.x, m_n.y, 0);
	float a = n.GetAngle(CVector3D(0, 1, 0));
	m_additionalPos = CMatrix4D::RotateZ(a) * CVector3D(-pos.x, -pos.z, pos.y);
	SetEyePosition(m_eye + m_additionalPos);*/

	m_yaw = yaw;
	m_pitch = pitch;
	m_roll = roll;	
	m_isAdditionalPosSet = true;
}

void CCamera3D::ClearAdditionalPos()
{
	SetEyePosition(m_eye - m_additionalPos);
	m_isAdditionalPosSet = false;
}

void CCamera3D::GetAdditionalRotation(float& yaw, float& pitch, float& roll) const
{
	if (m_isAdditionalPosSet)
	{
		yaw = m_yaw;
		roll = m_roll;
		pitch = m_pitch;
	}
	else
	{
		yaw = 0;
		roll = 0;
		pitch = 0;
	}
}

void CCamera3D::Set(const CVector3D& eye, const CVector3D& look, const CVector3D& up)
{
	m_eye.Set(eye);
	m_look.Set(look);
	m_up.Set(up);

	Bind();
}

void CCamera3D::UpdateModelViewMatrix()
{
	m_n.Set(m_eye.x - m_look.x, m_eye.y - m_look.y, m_eye.z - m_look.z);
	m_u.Set(m_up.Cross(m_n));

	m_n.Normalize();
	m_u.Normalize();

	m_v.Set(m_n.Cross(m_u));
	SetModelViewMatrix();
}

void CCamera3D::Bind(const float aspect)
{
#pragma warning(push)
#pragma warning(disable : 4239)

	// затычки против зеленого экрана
	if ((m_eye.x == m_look.x) && (m_eye.y == m_look.y) && (m_eye.z == m_look.z))
	{
		m_look.x = m_eye.x + 1.0f;
		m_look.y = m_eye.y + 1.0f;
		m_look.z = m_eye.z;
	}

	if ((_isnan(m_eye.x)) || (_isnan(m_eye.y)) || (_isnan(m_eye.z)))
	{		
		m_eye.x = m_look.x - 1.0f;
		m_eye.y = m_look.y - 1.0f;
		m_eye.z = m_look.z;
	}

	if ((_isnan(m_look.x)) || (_isnan(m_look.y)) || (_isnan(m_look.z)))
	{
		m_look.x = m_eye.x - 1.0f;
		m_look.y = m_eye.y - 1.0f;
		m_look.z = m_eye.z;
	}

	UpdateModelViewMatrix();

	g->cm.SetNearPlane(m_nearPlane);
	g->cm.SetFarPlane(m_farPlane);
	if (aspect == 0.0f)
	{
		g->cm.SetFov((float)(g->rs.GetInt(FOV) - 45));
	}
	else
	{
		g->cm.SetFov((float)(g->rs.GetInt(FOV) - 45), aspect);
	}

#pragma warning(pop)
}

void CCamera3D::Slide(const float delU, const float delV, const float delN)
{
	m_eye.x += delU * m_u.x + delV * m_v.x + delN * m_n.x;
	m_eye.y += delU * m_u.y + delV * m_v.y + delN * m_n.y;
	m_eye.z += delU * m_u.z + delV * m_v.z + delN * m_n.z;

	SetModelViewMatrix();
}

void CCamera3D::Roll(const float angle)
{
	float cs = float(cos(DegreesToRadians(angle)));
	float sn = float(sin(DegreesToRadians(angle)));
	CVector3D t = m_u;

	m_u.Set(cs * t.x - sn * m_v.x, cs * t.y - sn * m_v.y, cs * t.z - sn * m_v.z);
	m_v.Set(sn * t.x + cs * m_v.x, sn * t.y + cs * m_v.y, sn * t.z + cs * m_v.z);

	//SetModelViewMatrix();
}

void CCamera3D::Pitch(const float angle)
{
	float cs = float(cos(DegreesToRadians(angle)));
	float sn = float(sin(DegreesToRadians(angle)));

	CVector3D t = m_n;

	m_n.Set(cs * t.x - sn * m_v.x, cs * t.y - sn * m_v.y, cs * t.z - sn * m_v.z);
	m_v.Set(sn * t.x + cs * m_v.x, sn * t.y + cs * m_v.y, sn * t.z + cs * m_v.z);

	//SetModelViewMatrix();
}

void CCamera3D::Yaw(const float angle)
{
	float cs = float(cos(DegreesToRadians(angle)));
	float sn = float(sin(DegreesToRadians(angle)));

	CVector3D t = m_n;

	m_n.Set(cs * t.x - sn * m_u.x, cs * t.y - sn * m_u.y, cs * t.z - sn * m_u.z);
	m_u.Set(sn * t.x + cs * m_u.x, sn * t.y + cs * m_u.y, sn * t.z + cs * m_u.z);

	//SetModelViewMatrix();
}

CVector3D CCamera3D::GetCameraNormal()const
{
	return m_n;
}

CVector3D CCamera3D::GetMainVector1()const
{
	return m_u;
}

CVector3D CCamera3D::GetMainVector2()const
{
	return m_v;
}

void CCamera3D::SetEyePosition(const CVector3D& eye)
{
	m_eye = eye;
}

void CCamera3D::SetEyePosition(const float x, const float y, const float z)
{
	m_eye.x = x;
	m_eye.y = y;
	m_eye.z = z;
}

CVector3D CCamera3D::GetEyePosition()const
{
	return m_eye;
}

void CCamera3D::GetEyePosition(float* const x, float* const y, float* const z)const
{
	*x = m_eye.x;
	*y = m_eye.y;
	*z = m_eye.z;
}

void CCamera3D::SetLookAt(const CVector3D& at)
{
	m_look = at;
}

void CCamera3D::SetLookAt(const float x, const float y, const float z)
{
	m_look.x = x;
	m_look.y = y;
	m_look.z = z;
}

CVector3D CCamera3D::GetLookAt()const
{
	return m_look;
}

void CCamera3D::GetLookAt(float* const x, float* const y, float* const z)const
{
	*x = m_look.x;
	*y = m_look.y;
	*z = m_look.z;
}

void CCamera3D::SetUp(const CVector3D& up)
{
	m_up = up;
}

void CCamera3D::SetUp(const float x, const float y, const float z)
{
	m_up.x = x;
	m_up.y = y;
	m_up.z = z;
}

CVector3D CCamera3D::GetUp()const
{
	return m_up;
}

void CCamera3D::GetUp(float* const x, float* const y, float* const z)const
{
	*x = m_up.x;
	*y = m_up.y;
	*z = m_up.z;
}

void CCamera3D::LinkToObject(C3DObject* const obj)
{
	m_linkToObject = obj;
}

C3DObject* CCamera3D::GetLinkedToObject()
{
	return m_linkToObject;
}

void CCamera3D::SetFov(const float fov)
{
	m_fov = fov;
}

float CCamera3D::GetFov()const
{
	return m_fov;
}

void CCamera3D::SetNearPlane(const float pos)
{
	m_nearPlane = pos;
}

float CCamera3D::GetNearPlane()const
{
	return m_nearPlane;
}

void CCamera3D::SetFarPlane(const float pos)
{
	m_farPlane = pos;
}

float CCamera3D::GetFarPlane()const
{
	return m_farPlane;
}

CCamera3D* CCamera3D::SaveState()
{
	CCamera3D* saveCamera = MP_NEW(CCamera3D);
	*saveCamera = *this;
	return saveCamera;
}

void CCamera3D::RestoreState(CCamera3D* saveCamera)
{
	if (saveCamera)
	{
		*this = *saveCamera;
	}	
}

CCamera3D& CCamera3D::operator = (const CCamera3D& other)
{
	m_u = other.m_u;
	m_v = other.m_v;
	m_n = other.m_n;
	m_eye = other.m_eye;
	m_look = other.m_look;
	m_up = other.m_up;

	memcpy(m_matrix, other.m_matrix, sizeof(m_matrix));
	
	m_linkToObject = other.m_linkToObject;

	m_fov = other.m_fov;
	m_farPlane = other.m_farPlane;
	m_zOffset = other.m_zOffset;
	m_nearPlane = other.m_nearPlane;

	m_yaw = other.m_yaw;
	m_pitch = other.m_pitch;
	m_roll = other.m_roll;
	m_additionalPos = other.m_additionalPos;
	m_isLeftEye = other.m_isLeftEye;
	m_isAdditionalPosSet = other.m_isAdditionalPosSet;

	return *this;
}

CCamera3D::~CCamera3D()
{
}

