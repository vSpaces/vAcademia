
#include "StdAfx.h"
#include "CubeMap.h"
#include "CubemapClouds.h"
#include "GlobalSingletonStorage.h"
#include "cal3d/memory_leak.h"

CCubemapClouds::CCubemapClouds():
	m_r(255),
	m_g(255),
	m_b(255),
	m_a(255),
	m_fov(0),
	m_mode(0),
	m_cubemapID(-1)
{
	memset(&m_textureIDs[0], -1, CUBEMAP_FACE_COUNT * sizeof(int));
}

void CCubemapClouds::Destroy()
{
	MP_DELETE_THIS;
}

void CCubemapClouds::SetFov(int fov)
{
	m_fov = fov;
}

int CCubemapClouds::GetFov()const
{
	return m_fov;
}

void CCubemapClouds::SetTexture(int orderID, std::string name)
{
	int textureID = g->tm.GetObjectNumber(name);
	m_textureIDs[orderID] = textureID;
}

void CCubemapClouds::SetCubemapTextures(std::string leftTextureName, std::string frontTextureName, 
										std::string rightTextureName, std::string backTextureName, 
										std::string topTextureName, std::string bottomTextureName)
{
	SetTexture(0, leftTextureName);
	SetTexture(1, frontTextureName);
	SetTexture(2, rightTextureName);
	SetTexture(3, backTextureName);
	SetTexture(4, topTextureName);
	SetTexture(5, bottomTextureName);

	m_mode = SIX_TEXTURES_MODE;
}

void CCubemapClouds::SetCubemap(std::string cubemapName)
{
	m_cubemapID = g->cmm.GetObjectNumber(cubemapName);
	
	m_mode = CUBEMAP_MODE;
}

void CCubemapClouds::DrawBottomFace(CVector3D& size)
{
	GLFUNC(glNormal3f)(0, 0, -1);
	GLFUNC(glTexCoord2f)(0, 0);
	GLFUNC(glVertex3f)(size.x, size.y, -size.z);
	GLFUNC(glTexCoord2f)(0, 1);
	GLFUNC(glVertex3f)(-size.x, size.y, -size.z);
	GLFUNC(glTexCoord2f)(1, 1);
	GLFUNC(glVertex3f)(-size.x, -size.y, -size.z);
	GLFUNC(glTexCoord2f)(1, 0);
	GLFUNC(glVertex3f)(size.x, -size.y, -size.z);	
}

void CCubemapClouds::DrawTopFace(CVector3D& size)
{
	GLFUNC(glNormal3f)(0, 0, 1);
	GLFUNC(glTexCoord2f)(0, 1);
	GLFUNC(glVertex3f)(-size.x, size.y, size.z);
	GLFUNC(glTexCoord2f)(1, 1);
	GLFUNC(glVertex3f)(size.x, size.y, size.z);
	GLFUNC(glTexCoord2f)(1, 0);
	GLFUNC(glVertex3f)(size.x, -size.y, size.z);
	GLFUNC(glTexCoord2f)(0, 0);
	GLFUNC(glVertex3f)(-size.x, -size.y, size.z);
}

void CCubemapClouds::DrawLeftFace(CVector3D& size)
{
	GLFUNC(glNormal3f)(-1, 0, 0);
	GLFUNC(glTexCoord2f)(1, 1);
	GLFUNC(glVertex3f)(-size.x, size.y, -size.z);
	GLFUNC(glTexCoord2f)(1, 0);
	GLFUNC(glVertex3f)(-size.x, size.y, size.z);
	GLFUNC(glTexCoord2f)(0, 0);
	GLFUNC(glVertex3f)(-size.x, -size.y, size.z);
	GLFUNC(glTexCoord2f)(0, 1);
	GLFUNC(glVertex3f)(-size.x, -size.y, -size.z);
}

void CCubemapClouds::DrawRightFace(CVector3D& size)
{
	GLFUNC(glNormal3f)(1, 0, 0);
	GLFUNC(glTexCoord2f)(0, 0);
	GLFUNC(glVertex3f)(size.x, size.y, size.z);
	GLFUNC(glTexCoord2f)(0, 1);
	GLFUNC(glVertex3f)(size.x, size.y, -size.z);
	GLFUNC(glTexCoord2f)(1, 1);
	GLFUNC(glVertex3f)(size.x, -size.y, -size.z);
	GLFUNC(glTexCoord2f)(1, 0);
	GLFUNC(glVertex3f)(size.x, -size.y, size.z);
}

void CCubemapClouds::DrawFrontFace(CVector3D& size)
{
	GLFUNC(glNormal3f)(0, 1, 0);
	GLFUNC(glTexCoord2f)(0, 0);
	GLFUNC(glVertex3f)(-size.x, size.y, size.z);
	GLFUNC(glTexCoord2f)(1, 0);
	GLFUNC(glVertex3f)(size.x, size.y, size.z);
	GLFUNC(glTexCoord2f)(1, 1);
	GLFUNC(glVertex3f)(size.x, size.y, -size.z);
	GLFUNC(glTexCoord2f)(0, 1);
	GLFUNC(glVertex3f)(-size.x, size.y, -size.z);
}

void CCubemapClouds::DrawBackFace(CVector3D& size)
{
	GLFUNC(glNormal3f)(0, -1, 0);
	GLFUNC(glTexCoord2f)(0, 0);
	GLFUNC(glVertex3f)(size.x, -size.y, size.z);
	GLFUNC(glTexCoord2f)(1, 0);
	GLFUNC(glVertex3f)(-size.x, -size.y, size.z);
	GLFUNC(glTexCoord2f)(1, 1);
	GLFUNC(glVertex3f)(-size.x, -size.y, -size.z);
	GLFUNC(glTexCoord2f)(0, 1);
	GLFUNC(glVertex3f)(size.x, -size.y, -size.z);
}

void CCubemapClouds::SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	m_r = r;
	m_g = g;
	m_b = b;
	m_a = a;
}

void CCubemapClouds::Draw()
{
	if (!g->rs.GetBool(SHOW_SKY))
	{
		return;
	}

	assert(m_mode != 0);

	if (g->rs.GetBool(SHADERS_USING))
	{
		g->sm.UnbindShader();
	}

 	g->stp.SetMaterial(-1);
	g->stp.PrepareForRender();

	g->stp.PushMatrix();

	g->br.NewFrame();

	float aspect = g->scm.GetActiveScene()->GetViewport()->GetAspect();
	g->cm.SetFov((float)(g->rs.GetInt(FOV) - 45), aspect);
	if (m_fov != 0)
	{
		g->cm.SetFov((float)m_fov, aspect);
	}

	//glDisable(GL_TEXTURE	_2D);
    g->stp.SetDefaultStates();
	//glDisable(GL_DEPTH_TEST);
	g->stp.SetState(ST_ZTEST, false);

	//glDisable(GL_CULL_FACE);
	g->stp.SetState(ST_CULL_FACE, false);
	g->stp.SetState(ST_ZWRITE, false);

	float x, y, z;
	g->cm.GetCameraEye(&x, &y, &z);
	if (g->rs.GetBool(VERTICAL_AXE_Z))
	{
		z = 0;
	}
	else
	{
		y = 0;
	}
	GLFUNC(glTranslatef)(x, y, z);

	GLFUNC(glEnable)(GL_CLIP_PLANE0);
	double plane[4] = {0, 0, 1, 0.0f};
	GLFUNC(glClipPlane)(GL_CLIP_PLANE0, (double *)&plane);
	
	g->stp.SetColor(m_r, m_g, m_b, m_a);
	
	switch (m_mode)
	{
	case SIX_TEXTURES_MODE:
		RenderSixTextures();
		break;

	case CUBEMAP_MODE:
		RenderCubemap();
		break;
	}

	GLFUNC(glDisable)(GL_CLIP_PLANE0);

	g->stp.SetState(ST_ZWRITE, true);
	g->stp.SetState(ST_ZTEST, true);
	g->cm.SetFov((float)(g->rs.GetInt(FOV) - 45), aspect);
	g->stp.PopMatrix();

	g->cm.ExtractFrustum();

	g->stp.SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	g->stp.SetState(ST_BLEND, false);
	g->stp.SetColor(255, 255, 255, 255);
}

void CCubemapClouds::RenderSixTextures()
{
	int nonDrawingFace = 3;

	if (g->rs.GetBool(VERTICAL_AXE_Z))
	{
		nonDrawingFace = 5;
	}

	for (int i = 0; i < CUBEMAP_FACE_COUNT; i++)
	if ((m_textureIDs[i] != -1) && (i != nonDrawingFace))
	{
		g->tm.BindTexture(m_textureIDs[i]);

		g->stp.PrepareForRender();
		GLFUNC(glBegin)(GL_QUADS);

#pragma warning(push)
#pragma warning(disable : 4239)

		switch (i)
		{
		case 0:
			DrawLeftFace(CVector3D(GetSize(), GetSize(), GetSize()));
			break;

		case 1:
			DrawFrontFace(CVector3D(GetSize(), GetSize(), GetSize()));
			break;

		case 2:
			DrawRightFace(CVector3D(GetSize(), GetSize(), GetSize()));
			break;

		case 3:
			DrawBackFace(CVector3D(GetSize(), GetSize(), GetSize()));
			break;

		case 4:
			DrawTopFace(CVector3D(GetSize(), GetSize(), GetSize()));
			break;

		case 5:
			DrawBottomFace(CVector3D(GetSize(), GetSize(), GetSize()));
			break;
		}

#pragma warning(pop)

		GLFUNC(glEnd)();
	}
}

void CCubemapClouds::RenderCubemap()
{
	g->stp.PrepareForRender();

	if (g->rs.GetBool(SHADERS_USING))
	{
		g->sm.UnbindShader();
	}

#pragma warning(push)
#pragma warning(disable : 4239)

	CCubeMap* cubeMap = g->cmm.GetObjectPointer(m_cubemapID);
	cubeMap->Draw(CVector3D(GetSize(), GetSize(), GetSize()));

#pragma warning(pop)
}

CCubemapClouds::~CCubemapClouds()
{
}