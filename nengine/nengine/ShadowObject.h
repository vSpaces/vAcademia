#pragma once

#include "ShadowVolumeCreatingTask.h"
#include "CommonObject.h"
#include "Vector3d.h"

struct Plane
{
	float a, b, c, d;
};

struct Face
{
	int vertexIndices[3]; 
	int neighbourIndices[3]; 
	Plane planeEquation;  
	bool isVisible;         
};

#define INFINITY	0.09f

class CShadowObject : public CCommonObject
{
public:
	CShadowObject();
	~CShadowObject();

	void SetShadowSource(float x, float y, float z);   
	void SetShadowSource(CVector3D& pos);
	CVector3D& CShadowObject::GetLightPosition();

	void SetShadowPower(float power);
	float GetShadowPower()const;

	void SetInfinity(float inf);
	float GetInfinity()const;

	void SetVertexCount(int count);
	int GetVertexCount()const;

	void SetFaceCount(int count);
	int GetFaceCount()const;

	void SetModelHeight(float modelHeight);
	float GetModelHeight()const;

	void SetCastedStatus(bool isCasted);
	bool IsCasted()const;

	void Create();
	bool IsCreated();

	void FreeResources();

	CVector3D* m_vx;
	CVector3D* m_vn;
	Face* m_fs;

private:
	int m_vertexCount;
	int m_faceCount;
	float m_infinity;
	float m_modelHeight;
	float m_shadowPower;

	CVector3D m_lightPosition;

	CShadowVolumeCreatingTask m_task;

	bool m_isCasted;
	bool m_isCreated;
};

