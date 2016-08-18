
#pragma once

#include "CommonObject.h"
#include "3DObject.h"
#include "Vector3d.h"
#include "Point3d.h"

class CCamera3D : public CCommonObject
{
public:
	CCamera3D();
	~CCamera3D();

	void IncOffset();
	void DecOffset();

	void ClearAdditionalPos();	
	void SetAdditionalPos(const CVector3D& pos, const float yaw, const float pitch, const float roll);
	void GetAdditionalRotation(float& yaw, float& pitch, float& roll) const;

	// Set camera settings and bind camera
	void Set(const CVector3D& eye, const CVector3D& look, const CVector3D& up);
	// Bind camera with latest settings
	void Bind(const float aspect = 0.0f);
	// Change eye position relatively
	void Slide(const float delU, const float delV, const float delN);
	// Pitch camera (angle in degrees)
	void Pitch(const float angle);
	// Roll camera (angle in degrees)
	void Roll(const float angle);
	// Yaw camera (angle in degrees)
	void Yaw(const float angle);

	CVector3D GetExtractedLookAt() const;
	void SetOculusLookAt(const CVector3D& look);
	void ResetOculusLookAt();
	
	// Return vectors for billboard constructing
	CVector3D GetMainVector1()const;
	CVector3D GetMainVector2()const;

	// Set absolute eye position
	void SetEyePosition(const CVector3D& eye);
	void SetEyePosition(const float x, const float y, const float z);
	// Set look at
	void SetLookAt(const CVector3D& at);
	void SetLookAt(const float x, const float y, const float z);
	// Set up
	void SetUp(const CVector3D& up);
	void SetUp(const float x, const float y, const float z);

	// Return eye position
	CVector3D GetEyePosition()const;
	void GetEyePosition(float* const x, float* const y, float* const z)const;
	// Return normal vector
	CVector3D GetCameraNormal()const;
	// Return look point
	CVector3D GetLookAt()const;
	void GetLookAt(float* const x, float* const y, float* const z)const;
	// Return up vector
	CVector3D GetUp()const;
	void GetUp(float* const x, float* const y, float* const z)const;

	void LinkToObject(C3DObject* const obj);
	C3DObject* GetLinkedToObject();

	void SetFov(const float fov);
	float GetFov()const;

	void SetZOffset(const float zOffset);
	float GetZOffset()const;

	__forceinline bool IsCameraInObject(const float x1, const float y1, const float x2, const float y2)
	{
		return ((m_eye.x >= x1) && (m_eye.x <= x2) && (m_eye.y >= y1) && (m_eye.y <= y2));
	}
	
	__forceinline float GetPointDistSq(const float x, const float y, const float z)
	{
		return (m_eye.x - x) * (m_eye.x - x) + (m_eye.y - y) * (m_eye.y - y) + (m_eye.z - z) * (m_eye.z - z);
	}

	__forceinline float GetPointDistSq(const CVector3D& vec)
	{
		return (m_eye - vec).GetLengthSq();
	}

	__forceinline float GetPointDist(const float x, const float y, const float z)
	{
		return sqrtf(GetPointDistSq(x, y, z));
	}

	__forceinline float GetPointDist(const CVector3D& vec)
	{
		return (m_eye - vec).GetLength();
	}	

	float* GetMatrix();

	void SetNearPlane(const float pos);
	float GetNearPlane()const;
	
	void SetFarPlane(const float pos);
	float GetFarPlane()const;
	
	void UpdateModelViewMatrix();

	CCamera3D* SaveState();
	void RestoreState(CCamera3D* saveCamera);

	CCamera3D& operator = (const CCamera3D& other);

	void SetLeftEye(bool isLeftEye);
	
private:
	void SetModelViewMatrix();
	void CalcNormalFromLook(CVector3D & n, CVector3D const& look, bool doNormalize = true) const;
	void CalcNormalFromLookWithEyeOffset(CVector3D & n, CVector3D  const& u, CVector3D const& look, bool isLeftEye, float offset) const;
	void CalcUVFromNormal(CVector3D & u, CVector3D & v, CVector3D const& n) const;
	void CalcAdditionalRotPart(CVector3D & v1, CVector3D & v2, float a) const;
	void CalcAdditionalRot(CVector3D & n, CVector3D & u, CVector3D & v, float yaw, float roll, float pitch) const;
	void RotateByVector(CVector3D & v, CVector3D const& n, float a) const;

	float DegreesToRadians(const float angle) const;

	// Internal parameters of camera
	CVector3D m_u, m_v, m_n;
	// External parameters of camera
	CVector3D m_eye, m_look, m_up;

	float m_matrix[16];

	// Camera linked to this object
	C3DObject* m_linkToObject;

	float m_fov;
	float m_farPlane;
	float m_zOffset;
	float m_nearPlane;

	float m_yaw;
	float m_pitch;
	float m_roll;

	//внешний дополнительный поворот направления Oculus, изменяемый через SetOculusLookAt
	float m_addOculusLookA;

	CVector3D m_additionalPos;
	bool m_isLeftEye;
	bool m_isAdditionalPosSet;	

	float m_offset;
};
