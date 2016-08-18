
#include "StdAfx.h"
#include <Assert.h>
#include "CubeMap.h"
#include "cal3d/memory_leak.h"

CCubeMap::CCubeMap():
	m_cubeMapID(-1)
{

}

void CCubeMap::DrawBackFace(CVector3D& size, float koef)
{
	GLFUNC(glNormal3f)(0, 0, -1);
	GLFUNC(glTexCoord3f)(size.x, -size.y * koef, -size.z);
	GLFUNC(glVertex3f)(size.x, size.y, -size.z);
	GLFUNC(glTexCoord3f)(-size.x, -size.y * koef, -size.z);
	GLFUNC(glVertex3f)(-size.x, size.y, -size.z);
	GLFUNC(glTexCoord3f)(-size.x, size.y, -size.z);
	GLFUNC(glVertex3f)(-size.x, -size.y * koef, -size.z);	
	GLFUNC(glTexCoord3f)(size.x, size.y, -size.z);
	GLFUNC(glVertex3f)(size.x, -size.y * koef, -size.z);	
}

void CCubeMap::DrawFrontFace(CVector3D& size, float koef)
{
	GLFUNC(glNormal3f)(0, 0, 1);
	GLFUNC(glTexCoord3f)(-size.x, -size.y * koef, size.z);
	GLFUNC(glVertex3f)(-size.x, size.y, size.z);
	GLFUNC(glTexCoord3f)(size.x, -size.y * koef, size.z);
	GLFUNC(glVertex3f)(size.x, size.y, size.z);
	GLFUNC(glTexCoord3f)(size.x, size.y, size.z);
	GLFUNC(glVertex3f)(size.x, -size.y * koef, size.z);
	GLFUNC(glTexCoord3f)(-size.x, size.y, size.z);
	GLFUNC(glVertex3f)(-size.x, -size.y * koef, size.z);
}

void CCubeMap::DrawLeftFace(CVector3D& size, float koef)
{
	GLFUNC(glNormal3f)(-1, 0, 0);
	GLFUNC(glTexCoord3f)(-size.x, -size.y * koef, -size.z);
	GLFUNC(glVertex3f)(-size.x, size.y, -size.z);
	GLFUNC(glTexCoord3f)(-size.x, -size.y * koef, size.z);
	GLFUNC(glVertex3f)(-size.x, size.y, size.z);
	GLFUNC(glTexCoord3f)(-size.x, size.y, size.z);
	GLFUNC(glVertex3f)(-size.x, -size.y * koef, size.z);
	GLFUNC(glTexCoord3f)(-size.x, size.y, -size.z);
	GLFUNC(glVertex3f)(-size.x, -size.y * koef, -size.z);
}

void CCubeMap::DrawRightFace(CVector3D& size, float koef)
{
	GLFUNC(glNormal3f)(1, 0, 0);
	GLFUNC(glTexCoord3f)(size.x, -size.y * koef, size.z);
	GLFUNC(glVertex3f)(size.x, size.y, size.z);
	GLFUNC(glTexCoord3f)(size.x, -size.y * koef, -size.z);
	GLFUNC(glVertex3f)(size.x, size.y, -size.z);
	GLFUNC(glTexCoord3f)(size.x, size.y, -size.z);
	GLFUNC(glVertex3f)(size.x, -size.y * koef, -size.z);
	GLFUNC(glTexCoord3f)(size.x, size.y, size.z);
	GLFUNC(glVertex3f)(size.x, -size.y * koef, size.z);
}

void CCubeMap::DrawTopFace(CVector3D& size)
{
	GLFUNC(glNormal3f)(0, 1, 0);
	GLFUNC(glTexCoord3f)(-size.x, size.y, size.z);
	GLFUNC(glVertex3f)(-size.x, size.y, size.z);
	GLFUNC(glTexCoord3f)(-size.x, size.y, -size.z);
	GLFUNC(glVertex3f)(size.x, size.y, size.z);
	GLFUNC(glTexCoord3f)(size.x, size.y, -size.z);
	GLFUNC(glVertex3f)(size.x, size.y, -size.z);
	GLFUNC(glTexCoord3f)(size.x, size.y, size.z);
	GLFUNC(glVertex3f)(-size.x, size.y, -size.z);
}

void CCubeMap::DrawBottomFace(CVector3D& size)
{
	GLFUNC(glNormal3f)(0, -1, 0);
	GLFUNC(glTexCoord3f)(-size.x, -size.y, -size.z);
	GLFUNC(glVertex3f)(size.x, -size.y, size.z);
	GLFUNC(glTexCoord3f)(-size.x, -size.y, size.z);
	GLFUNC(glVertex3f)(-size.x, -size.y, size.z);
	GLFUNC(glTexCoord3f)(size.x, -size.y, size.z);
	GLFUNC(glVertex3f)(-size.x, -size.y, -size.z);
	GLFUNC(glTexCoord3f)(size.x, -size.y, -size.z);
	GLFUNC(glVertex3f)(size.x, -size.y, -size.z);
}

void CCubeMap::StartDraw()
{	
	if (-1 == m_cubeMapID)
	{
		return;
	}

	GLFUNC(glBindTexture)(GL_TEXTURE_CUBE_MAP, m_cubeMapID);
	GLFUNC(glDisable)(GL_TEXTURE_2D);
    GLFUNC(glEnable)(GL_TEXTURE_CUBE_MAP);

    GLFUNC(glBegin)(GL_QUADS);
}

void CCubeMap::EndDraw()
{
	if (-1 == m_cubeMapID)
	{
		return;
	}

	GLFUNC(glEnd)();

	GLFUNC(glEnable)(GL_TEXTURE_2D);
    GLFUNC(glDisable)(GL_TEXTURE_CUBE_MAP);
}

void CCubeMap::Draw(CVector3D& size)
{
	if (-1 == m_cubeMapID)
	{
		return;
	}

	StartDraw();

	DrawLeftFace(size);
	DrawRightFace(size);
	DrawTopFace(size);
	DrawBottomFace(size);
	DrawBackFace(size);
	DrawFrontFace(size);

	EndDraw();
}

void CCubeMap::DrawWithoutBottomFace(CVector3D& size)
{
	if (-1 == m_cubeMapID)
	{
		return;
	}

	StartDraw();

	DrawLeftFace(size);
	DrawRightFace(size);
	DrawTopFace(size);
	DrawBackFace(size);
	DrawFrontFace(size);

	EndDraw();
}

void CCubeMap::DrawHalf(CVector3D& size)
{
	if (-1 == m_cubeMapID)
	{
		return;
	}

	StartDraw();

	DrawLeftFace(size, 0.0f);
	DrawRightFace(size, 0.0f);
	DrawTopFace(size);
	DrawBackFace(size, 0.0f);
	DrawFrontFace(size, 0.0f);

	EndDraw();
}

void CCubeMap::SetCubeMapID(int id)
{
	m_cubeMapID = id;
}

int CCubeMap::GetCubeMapID()const
{
	return m_cubeMapID;
}

CCubeMap::~CCubeMap()
{

}