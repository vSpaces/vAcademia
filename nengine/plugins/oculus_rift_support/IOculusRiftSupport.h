#pragma once
#include "../../nmath/Vector2D.h"
#include "../../nmath/matrix4d.h"

struct IOculusRiftSupport
{
	struct ShaderParams
	{
		CVector2D uvScaleOffset[2];
		CMatrix4D timeWarpMatrices[2];
	};

	struct DistortionVertex
	{
		CVector2D ScreenPosNDC;
		float TimeWarpFactor;
		float VignetteFactor;
		float EyeType;
		CVector2D TanEyeAnglesR;
		CVector2D TanEyeAnglesG;
		CVector2D TanEyeAnglesB;
	};

	struct DistortionMesh
	{
		DistortionVertex* pVertexData;
		unsigned short* pIndexData;
		unsigned int VertexCount;
		unsigned int IndexCount;
	};

	enum InitError {
		NONE,
		FAIL_INIT,
		FAIL_ON_CONNECT_TO_RUNTIME_SERVER,
		HMD_NOT_DETECTED, 
		MONITOR_NOT_ACTIVE,
		NOT_SUPPORTED_DIRECT_MODE,
		NOT_SUPPORTED_MONITOR_CLONED
	};

	virtual bool GetSensorPos(CVector3D& pos, float& x, float& y, float& z)const = 0;
	virtual bool GetDesktopParams(int& wResolution, int& hResolution, int& displayX, int& displayY) = 0;	
	virtual void SetLeftEye(bool isLeftEye) = 0;
	virtual void GetViewportParams(int& x, int& y, int& width, int& height) = 0;
	virtual void GetShaderParams(ShaderParams* params) = 0;
	virtual void BeginRender() = 0;
	virtual void EndRender() = 0;
	virtual DistortionMesh* GetDistortionMesh() = 0;
	virtual void DestroyDistortionMesh() = 0;
};