
#pragma once

#include "CommonObject.h"
#include "Vector3D.h"

class CCubeMap : public CCommonObject
{
public:
	CCubeMap();
	~CCubeMap();

	void Draw(CVector3D& size);
	void DrawWithoutBottomFace(CVector3D& size);
	void DrawHalf(CVector3D& size);

	void SetCubeMapID(int id);
	int GetCubeMapID()const;

private:
	void StartDraw();
	void EndDraw();

	void DrawBackFace(CVector3D& size, float koef = 1);
	void DrawFrontFace(CVector3D& size, float koef = 1);
	void DrawLeftFace(CVector3D& size, float koef = 1);
	void DrawRightFace(CVector3D& size, float koef = 1);
	void DrawTopFace(CVector3D& size);
	void DrawBottomFace(CVector3D& size);

	int m_cubeMapID;
};