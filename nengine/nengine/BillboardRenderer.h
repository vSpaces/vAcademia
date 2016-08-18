#pragma once

#include "Vector3D.h"

class CBillboardRenderer
{
public:
	CBillboardRenderer();
	~CBillboardRenderer();

	void NewFrame();
	void ShowBillboard(float xx, float yy, float zz, float size);

	CVector3D m_particleVector1, m_particleVector2;

private:
	void GetLook(CVector3D &look);

	CVector3D m_particleLook;
};