m
#include "StdAfx.h"
#include "BillboardRenderer.h"
#include "GlobalSingletonStorage.h"
#include "cal3d/memory_leak.h"

CBillboardRenderer::CBillboardRenderer()
{
}

void CBillboardRenderer::GetLook(CVector3D &look)
{
	float matrix[4][4];
	
	GLFUNC(glGetFloatv)(GL_MODELVIEW_MATRIX, &matrix[0][0]);
	look.x = matrix[0][3] + matrix[0][2];
	look.y = matrix[1][3] + matrix[1][2];
	look.z = matrix[2][3] + matrix[2][2];
}

void CBillboardRenderer::NewFrame()
{
	m_particleVector1 = g->cm.GetCameraMainVector1();
	m_particleVector1.Normalize();

	m_particleVector2 = g->cm.GetCameraMainVector2();
	m_particleVector2.Normalize();
}

void CBillboardRenderer::ShowBillboard(float xx, float yy, float zz, float size)
{
	float sizeX1 = m_particleVector1.x * size;
	float sizeY1 = m_particleVector1.y * size;
	float sizeZ1 = m_particleVector1.z * size;
	float sizeX2 = m_particleVector2.x * size;
	float sizeY2 = m_particleVector2.y * size;
	float sizeZ2 = m_particleVector2.z * size;

	g->stp.PrepareForRender();	

	GLFUNC(glBegin)(GL_QUADS);

	GLFUNC(glTexCoord2f)(0, 0);
	GLFUNC(glVertex3f)(xx + sizeX1 + sizeX2, yy + sizeY1 + sizeY2, zz + sizeZ1 + sizeZ2);

	GLFUNC(glTexCoord2f)(0, 1);
	GLFUNC(glVertex3f)(xx + sizeX1 - sizeX2, yy + sizeY1 - sizeY2, zz + sizeZ1 - sizeZ2);	

	GLFUNC(glTexCoord2f)(1, 1);
	GLFUNC(glVertex3f)(xx - sizeX1 - sizeX2, yy - sizeY1 - sizeY2, zz - sizeZ1 - sizeZ2);		

	GLFUNC(glTexCoord2f)(1, 0);
	GLFUNC(glVertex3f)(xx - sizeX1 + sizeX2, yy - sizeY1 + sizeY2, zz - sizeZ1 + sizeZ2);

	GLFUNC(glEnd)();
}

CBillboardRenderer::~CBillboardRenderer()
{
}