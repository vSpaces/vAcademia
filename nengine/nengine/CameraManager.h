
#pragma once

#include "GlobalInterfaceStorage.h"
#include "ICameraManager.h"
#include "Manager.h"
#include "Camera.h"

class CCameraManager :  public CManager<CCamera3D>/*,
						public ICameraManager,
						public ICamManager*/
{
public:
	friend class CGlobalSingletonStorage;

	// set 2d camera
	void SetCamera2d();
	// set 3d camera settings
	void SetCamera(float eye_x, float eye_y, float eye_z, float look_x, float look_y, 
			   float look_z, float up_x, float up_y, float up_z);
	// set FOV
	void SetFov(float fov,float _aspect = 0);
	// change camera's eye position
	void Slide(float dx, float dy, float dz);
	// roll camera
	void Roll(float angle);
	// pitch camera
	void Pitch(float angle);
	// yaw camera
	void Yaw(float angle);
	// return normal vector
	void GetCameraNormal(float *x,float *y,float *z);
	CVector3D GetCameraMainVector1();
	CVector3D GetCameraMainVector2();
	// return camera's eye position
	void GetCameraEye(float *x, float *y, float *z);
	// set near plane position
	void SetNearPlane(float pos);
	// set far plane position
	void SetFarPlane(float pos);

	// get near plane position
	float GetNearPlane();
	// get far plane position
	float GetFarPlane();

	__forceinline float CCameraManager::GetPointDistSq(float x, float y, float z)
	{
		if (!m_activeCamera)
		{
			return 0.0f;
		}

		return m_activeCamera->GetPointDistSq(x, y, z);
	}

	__forceinline bool IsCameraInObject(const float x1, const float y1, const float x2, const float y2)
	{
		if (!m_activeCamera)
		{
			return false;
		}

		return m_activeCamera->IsCameraInObject(x1, y1, x2, y2);
	}

	__forceinline float CCameraManager::GetPointDist(float x, float y, float z)
	{
		return sqrtf(GetPointDistSq(x, y, z));
	}

	__forceinline bool IsPointInFrustum(const float x, const float y, const float z)
	{
		//assert(m_isFrustumExtracted);

		float* pFrustum = (float*)&m_frustum;

		if ((*pFrustum) * x + (*(pFrustum + 1)) * y + (*(pFrustum + 2)) * z + (*(pFrustum + 3)) <= 0)
		{
			return false;
		}
		pFrustum += 4;
		if ((*pFrustum) * x + (*(pFrustum + 1)) * y + (*(pFrustum + 2)) * z + (*(pFrustum + 3)) <= 0)
		{
			return false;
		}
		pFrustum += 4;
		if ((*pFrustum) * x + (*(pFrustum + 1)) * y + (*(pFrustum + 2)) * z + (*(pFrustum + 3)) <= 0)
		{
			return false;
		}
		pFrustum += 4;
		if ((*pFrustum) * x + (*(pFrustum + 1)) * y + (*(pFrustum + 2)) * z + (*(pFrustum + 3)) <= 0)
		{
			return false;
		}
		pFrustum += 4;
		if ((*pFrustum) * x + (*(pFrustum + 1)) * y + (*(pFrustum + 2)) * z + (*(pFrustum + 3)) <= 0)
		{
			return false;
		}
		pFrustum += 4;
		if ((*pFrustum) * x + (*(pFrustum + 1)) * y + (*(pFrustum + 2)) * z + (*(pFrustum + 3)) <= 0)
		{
			return false;
		}

		return true;
	}

	bool IsBoundingBoxInFrustum(const float x, const float y, const float z, const float xSize, const float ySize, const float zSize);

	bool IsBoundingBoxInFrustum(const float x, const float y, const float z, const CVector3D* const corners);

	void SetActiveCamera(int cameraID);
	void SetActiveCamera(CCamera3D* camera);
	CCamera3D* GetActiveCamera();

	void ExtractFrustum();
	void ExtractFrustumFromCamera(CCamera3D* camera);

private:
	CCameraManager();
	CCameraManager(const CCameraManager&);
	CCameraManager& operator=(const CCameraManager&);
	~CCameraManager();

	void ExtractFrustum(float* modl);

	CCamera3D* m_activeCamera;

	float m_nearPlane;
	float m_farPlane;

	float m_frustum[6][4];
	float m_optimizedFrustum[6][3];

	bool m_isFrustumExtracted;
};
