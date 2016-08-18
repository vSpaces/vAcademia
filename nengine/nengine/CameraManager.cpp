
#include "StdAfx.h"
#include "GlobalSingletonStorage.h"
#include "CameraManager.h"
#include "NEngine.h"
#include <math.h>
#include "cal3d/memory_leak.h"

CCameraManager::CCameraManager():
	m_nearPlane(10000.1f),
	m_farPlane(10.0f),
	m_isFrustumExtracted(false),
	m_activeCamera(NULL)
{
	ReserveMemory(10);
}

void CCameraManager::SetCamera2d()
{
	GLFUNC(glMatrixMode)(GL_PROJECTION);

	GLFUNC(glLoadIdentity)();
	GLFUNC(gluOrtho2D)(-1.0f, 1.0f, -1.0f, 1.0f);

	GLFUNC(glMatrixMode)(GL_MODELVIEW);

	//m_isFrustumExtracted = false;
}

void CCameraManager::SetFov(float fov, float _aspect)
{
	if (g->rs.IsOculusRiftEnabled())
	{
		return;
	}

	GLFUNC(glMatrixMode)(GL_PROJECTION);

	GLFUNC(glLoadIdentity)();
	float aspect; 
	if (_aspect == 0 )
	{
		aspect = (float)g->stp.GetWidth() / (float)g->stp.GetHeight();
	}
	else
	{
		aspect = _aspect;
	}
	//float aspect = (float)g->ne.GetWidth() / (float)g->ne.GetHeight();
	GLFUNC(gluPerspective)(45.0f + fov, aspect, m_nearPlane, m_farPlane);

	GLFUNC(glMatrixMode)(GL_MODELVIEW);

	m_isFrustumExtracted = false;
}

void CCameraManager::SetCamera(float eye_x, float eye_y, float eye_z, float look_x, float look_y, 
			   float look_z, float up_x, float up_y, float up_z)
{
	if (m_activeCamera)
	{
		CVector3D eye, look, up;
	
		eye.x = eye_x;
		eye.y = eye_y;
		eye.z = eye_z;

		look.x = look_x;
		look.y = look_y;
		look.z = look_z;

		up.x = up_x;
		up.y = up_y;
		up.z = up_z;

		m_activeCamera->Set(eye,look,up);

		m_isFrustumExtracted = false;
	}
}

void CCameraManager::Slide(float dx, float dy, float dz)
{
	if (m_activeCamera)
	{
		m_activeCamera->Slide(dx, dy, dz);

		m_isFrustumExtracted = false;
	}
}

void CCameraManager::Roll(float angle)
{
	if (m_activeCamera)
	{
		m_activeCamera->Roll(angle);

		m_isFrustumExtracted = false;
	}
}

void CCameraManager::Pitch(float angle)
{
	if (m_activeCamera)
	{
		m_activeCamera->Pitch(angle);

		m_isFrustumExtracted = false;
	}
}

void CCameraManager::Yaw(float angle)
{
	if (m_activeCamera)
	{
		m_activeCamera->Yaw(angle);

		m_isFrustumExtracted = false;
	}
}

void CCameraManager::GetCameraNormal(float *x, float *y, float *z)
{
	if (m_activeCamera)
	{
		CVector3D tmp = m_activeCamera->GetCameraNormal();

		*x = tmp.x;
		*y = tmp.y;
		*z = tmp.z;
	}
	else
	{
		*x = 0.0f;
		*y = 0.0f;
		*z = 0.0f;
	}
}

CVector3D CCameraManager::GetCameraMainVector1()
{
	if (m_activeCamera)
	{
		return m_activeCamera->GetMainVector1();
	}
	else
	{
		return CVector3D(0.0f, 0.0f, 0.0f);
	}
}

CVector3D CCameraManager::GetCameraMainVector2()
{
	if (m_activeCamera)
	{
		return m_activeCamera->GetMainVector2();
	}
	else
	{
		return CVector3D(0.0f, 0.0f, 0.0f);
	}
}

void CCameraManager::GetCameraEye(float *x, float *y, float *z)
{
	if (m_activeCamera)
	{
		CVector3D tmp = m_activeCamera->GetEyePosition();

		*x = tmp.x;
		*y = tmp.y;
		*z = tmp.z;
	}
	else
	{
		*x = 0.0f;
		*y = 0.0f;
		*z = 0.0f;
	}
}

float CCameraManager::GetFarPlane()
{
	return m_farPlane;
}

float CCameraManager::GetNearPlane()
{
	return m_nearPlane;
}

void CCameraManager::SetNearPlane(float pos)
{
	m_nearPlane = pos;
}

void CCameraManager::SetFarPlane(float pos)
{
	m_farPlane = pos;
}

void CCameraManager::ExtractFrustumFromCamera(CCamera3D* camera)
{
	camera->UpdateModelViewMatrix();
	ExtractFrustum(camera->GetMatrix());
}

void CCameraManager::ExtractFrustum()
{
	float modl[16];
	GLFUNC(glGetFloatv)(GL_MODELVIEW_MATRIX, modl);

	ExtractFrustum(&modl[0]);
}

void CCameraManager::ExtractFrustum(float* modl)
{
	m_isFrustumExtracted = true;

	float proj[16], clip[16], t;

	GLFUNC(glGetFloatv)(GL_PROJECTION_MATRIX, proj);


	// Make multiplication of matrixs
	clip[0] = modl[0] * proj[0] + modl[1] * proj[4] + modl[2] * proj[8] + modl[3] * proj[12];
	clip[1] = modl[0] * proj[1] + modl[1] * proj[5] + modl[2] * proj[9] + modl[3] * proj[13];
	clip[2] = modl[0] * proj[2] + modl[1] * proj[6] + modl[2] * proj[10] + modl[3] * proj[14];
	clip[3] = modl[0] * proj[3] + modl[1] * proj[7] + modl[2] * proj[11] + modl[3] * proj[15];

	clip[4] = modl[4] * proj[0] + modl[5] * proj[4] + modl[6] * proj[8] + modl[7] * proj[12];
	clip[5] = modl[4] * proj[1] + modl[5] * proj[5] + modl[6] * proj[9] + modl[7] * proj[13];
	clip[6] = modl[4] * proj[2] + modl[5] * proj[6] + modl[6] * proj[10] + modl[7] * proj[14];
	clip[7] = modl[4] * proj[3] + modl[5] * proj[7] + modl[6] * proj[11] + modl[7] * proj[15];

	clip[8] = modl[8] * proj[0] + modl[9] * proj[4] + modl[10] * proj[8] + modl[11] * proj[12];
	clip[9] = modl[8] * proj[1] + modl[9] * proj[5] + modl[10] * proj[9] + modl[11] * proj[13];
	clip[10] = modl[8] * proj[2] + modl[9] * proj[6] + modl[10] * proj[10] + modl[11] * proj[14];
	clip[11] = modl[8] * proj[3] + modl[9] * proj[7] + modl[10] * proj[11] + modl[11] * proj[15];

	clip[12] = modl[12] * proj[0] + modl[13] * proj[4] + modl[14] * proj[8] + modl[15] * proj[12];
	clip[13] = modl[12] * proj[1] + modl[13] * proj[5] + modl[14] * proj[9] + modl[15] * proj[13];
	clip[14] = modl[12] * proj[2] + modl[13] * proj[6] + modl[14] * proj[10] + modl[15] * proj[14];
	clip[15] = modl[12] * proj[3] + modl[13] * proj[7] + modl[14] * proj[11] + modl[15] * proj[15];

	// calculate A, B, C, D for right plane
	m_frustum[0][0] = clip[3] - clip[0];
	m_frustum[0][1] = clip[7] - clip[4];
	m_frustum[0][2] = clip[11] - clip[8];
	m_frustum[0][3] = clip[15] - clip[12];

	// tranform equation of plane to normalized
	t = sqrt( m_frustum[0][0] * m_frustum[0][0] + m_frustum[0][1] * m_frustum[0][1] + m_frustum[0][2] * m_frustum[0][2] );
	m_frustum[0][0] /= t;
	m_frustum[0][1] /= t;
	m_frustum[0][2] /= t;
	m_frustum[0][3] /= t;

	// calculate A, B, C, D for left plane
	m_frustum[1][0] = clip[3] + clip[0];
	m_frustum[1][1] = clip[7] + clip[4];
	m_frustum[1][2] = clip[11] + clip[8];
	m_frustum[1][3] = clip[15] + clip[12];

	// tranform equation of plane to normalized
	t = sqrtf(m_frustum[1][0] * m_frustum[1][0] + m_frustum[1][1] * m_frustum[1][1] + m_frustum[1][2] * m_frustum[1][2] );
	m_frustum[1][0] /= t;
	m_frustum[1][1] /= t;
	m_frustum[1][2] /= t;
	m_frustum[1][3] /= t;

	// Calculate  A, B, C, D for bottom plane
	m_frustum[2][0] = clip[3] + clip[1];
	m_frustum[2][1] = clip[7] + clip[5];
	m_frustum[2][2] = clip[11] + clip[9];
	m_frustum[2][3] = clip[15] + clip[13];

	// tranform equation of plane to normalized
	t = sqrtf(m_frustum[2][0] * m_frustum[2][0] + m_frustum[2][1] * m_frustum[2][1] + m_frustum[2][2] * m_frustum[2][2] );
	m_frustum[2][0] /= t;
	m_frustum[2][1] /= t;
	m_frustum[2][2] /= t;
	m_frustum[2][3] /= t;

	// top plane
	m_frustum[3][0] = clip[3] - clip[1];
	m_frustum[3][1] = clip[7] - clip[5];
	m_frustum[3][2] = clip[11] - clip[9];
	m_frustum[3][3] = clip[15] - clip[13];

	// normal view   
	t = sqrtf( m_frustum[3][0] * m_frustum[3][0] + m_frustum[3][1] * m_frustum[3][1] + m_frustum[3][2] * m_frustum[3][2] );
	m_frustum[3][0] /= t;
	m_frustum[3][1] /= t;
	m_frustum[3][2] /= t;
	m_frustum[3][3] /= t;

	// back plane
	m_frustum[4][0] = clip[3] - clip[2];
	m_frustum[4][1] = clip[7] - clip[6];
	m_frustum[4][2] = clip[11] - clip[10];
	m_frustum[4][3] = clip[15] - clip[14];

	// normal view
	t = sqrtf(m_frustum[4][0] * m_frustum[4][0] + m_frustum[4][1] * m_frustum[4][1] + m_frustum[4][2] * m_frustum[4][2] );
	m_frustum[4][0] /= t;
	m_frustum[4][1] /= t;
	m_frustum[4][2] /= t;
	m_frustum[4][3] /= t;

	// front plane
	m_frustum[5][0] = clip[3] + clip[2];
	m_frustum[5][1] = clip[7] + clip[6];
	m_frustum[5][2] = clip[11] + clip[10];
	m_frustum[5][3] = clip[15] + clip[14];

	// normal view
	t = sqrtf(m_frustum[5][0] * m_frustum[5][0] + m_frustum[5][1] * m_frustum[5][1] + m_frustum[5][2] * m_frustum[5][2] );
	m_frustum[5][0] /= t;
	m_frustum[5][1] /= t;
	m_frustum[5][2] /= t;
	m_frustum[5][3] /= t;	
}

#define MAX_SIZE	1000.0f

bool CCameraManager::IsBoundingBoxInFrustum(const float x, const float y, const float z, 
											const float xSize, const float ySize, const float zSize)
{
	float x1 = x - xSize;
	float x2 = x + xSize;
	float y1 = y - ySize;
	float y2 = y + ySize;
	float z1 = z - zSize;
	float z2 = z + zSize;

	float* pFrustum = (float*)&m_frustum;

	for (int k = 0; k < 6; k++)
	{
		if (((*pFrustum) * x1 + (*(pFrustum + 1)) * y2 + (*(pFrustum + 2)) * z2 + (*(pFrustum + 3)) <= 0) &&
		((*pFrustum) * x1 + (*(pFrustum + 1)) * y2 + (*(pFrustum + 2)) * z1 + (*(pFrustum + 3)) <= 0) &&
		((*pFrustum) * x1 + (*(pFrustum + 1)) * y1 + (*(pFrustum + 2)) * z2 + (*(pFrustum + 3)) <= 0) &&
		((*pFrustum) * x1 + (*(pFrustum + 1)) * y1 + (*(pFrustum + 2)) * z1 + (*(pFrustum + 3)) <= 0) &&
		((*pFrustum) * x2 + (*(pFrustum + 1)) * y2 + (*(pFrustum + 2)) * z2 + (*(pFrustum + 3)) <= 0) &&
		((*pFrustum) * x2 + (*(pFrustum + 1)) * y2 + (*(pFrustum + 2)) * z1 + (*(pFrustum + 3)) <= 0) &&
		((*pFrustum) * x2 + (*(pFrustum + 1)) * y1 + (*(pFrustum + 2)) * z2 + (*(pFrustum + 3)) <= 0) &&
		((*pFrustum) * x2 + (*(pFrustum + 1)) * y1 + (*(pFrustum + 2)) * z1 + (*(pFrustum + 3)) <= 0))
		{
			return false;
		}
		pFrustum += 4;
	}	

	return true;
}

bool CCameraManager::IsBoundingBoxInFrustum(const float x, const float y, const float z, const CVector3D* const corners)
{
	float* pFrustum = (float*)&m_frustum;

	for (int k = 0; k < 6; k++)
	{
		if (((*pFrustum) * (corners[0].x + x) + (*(pFrustum + 1)) * (corners[0].y + y) + (*(pFrustum + 2)) * (corners[0].z + z) + (*(pFrustum + 3)) <= 0) &&
		((*pFrustum) * (corners[1].x + x) + (*(pFrustum + 1)) * (corners[1].y + y) + (*(pFrustum + 2)) * (corners[1].z + z) + (*(pFrustum + 3)) <= 0) &&
		((*pFrustum) * (corners[2].x + x) + (*(pFrustum + 1)) * (corners[2].y + y) + (*(pFrustum + 2)) * (corners[2].z + z) + (*(pFrustum + 3)) <= 0) &&
		((*pFrustum) * (corners[3].x + x) + (*(pFrustum + 1)) * (corners[3].y + y) + (*(pFrustum + 2)) * (corners[3].z + z) + (*(pFrustum + 3)) <= 0) &&
		((*pFrustum) * (corners[4].x + x) + (*(pFrustum + 1)) * (corners[4].y + y) + (*(pFrustum + 2)) * (corners[4].z + z) + (*(pFrustum + 3)) <= 0) &&
		((*pFrustum) * (corners[5].x + x) + (*(pFrustum + 1)) * (corners[5].y + y) + (*(pFrustum + 2)) * (corners[5].z + z) + (*(pFrustum + 3)) <= 0) &&
		((*pFrustum) * (corners[6].x + x) + (*(pFrustum + 1)) * (corners[6].y + y) + (*(pFrustum + 2)) * (corners[6].z + z) + (*(pFrustum + 3)) <= 0) &&
		((*pFrustum) * (corners[7].x + x) + (*(pFrustum + 1)) * (corners[7].y + y) + (*(pFrustum + 2)) * (corners[7].z + z) + (*(pFrustum + 3)) <= 0))
		{
			return false;
		}
		pFrustum += 4;
	}	

	return true;
}

void CCameraManager::SetActiveCamera(int cameraID)
{
	m_activeCamera = g->cm.GetObjectPointer(cameraID);
}

void CCameraManager::SetActiveCamera(CCamera3D* camera)
{
	m_activeCamera = camera;
}

CCamera3D* CCameraManager::GetActiveCamera()
{
	return m_activeCamera;
}

CCameraManager::~CCameraManager()
{
}