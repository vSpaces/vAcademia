

#include "StdAfx.h"
#include "Math.h"
#include <Assert.h>
#include "DynamicClouds.h"
#include "HDRPostEffect.h"
#include "GlobalSingletonStorage.h"
#include "miniglut.h"

CDynamicClouds::CDynamicClouds():
	m_modelID(-1),
	m_shader(NULL),
	m_sunTextureID(0),
	m_timeLocation(-1),
	m_displayListID(-1),
	m_alphaLocation(-1),
	m_noiseTextureID(0),
	m_starsTextureID(0),
	m_sunposLocation(-1),	
	MP_STRING_INIT(m_shaderName)	
{
	m_shaderName = "shaders/sky";
	g->sm.AddChangesListener(this);

	SetStarsTextureName("COSMOS.JPG");
}

void CDynamicClouds::Destroy()
{
	MP_DELETE_THIS;
}

void CDynamicClouds::OnChanged(int /*eventID*/)
{
	m_shader = NULL;
}

void CDynamicClouds::Draw()
{
	if (!g->rs.GetBool(SHOW_SKY))
	{
		return;
	}

	if (!m_shader)
	{
		int shaderID = g->sm.GetObjectNumber(m_shaderName);
		m_shader = g->sm.GetObjectPointer(shaderID);

		m_shader->Bind(0);

		m_shader->SetUniformVector("eyePos", CVector4D(0, 0, 0.2f, 1));
		m_shader->SetUniformFloat("turbidity", 2.0f);
		m_shader->SetUniformTexture("noiseMap", 0);

		m_alphaLocation = m_shader->SetUniformFloat("alpha", 1.0f);
		m_timeLocation = m_shader->SetUniformFloat("time", (float)(g->ne.time%(g->dtc.GetDayDuration() * 4 * 1000)) / 2000.0f * (float)g->rs.GetInt(CLOUDS_SPEED) / 100.0f);
		m_sunposLocation = m_shader->SetUniformVector("sunPos", CVector4D(g->dtc.GetSunPosition(), 1));

		m_shader->Unbind();
	}

	if ((g->dtc.GetTimeOfDay() > 22 * 60) || (g->dtc.GetTimeOfDay() < 4 * 60))
	{
		m_cubemapClouds.SetSize(GetSize() * 0.5f);

		float x, y, z;
		g->cm.GetCameraEye(&x, &y, &z);
		g->stp.SetColor(128, 128, 128);
		if ((g->dtc.GetTimeOfDay() > 22 * 60) && (g->dtc.GetTimeOfDay() < 24 * 60))
		{
			unsigned char color = (unsigned char)(128 - (24 * 60 - g->dtc.GetTimeOfDay()) * 128 / 120);
			m_cubemapClouds.SetColor(color, color, color, 255);
		}
		if ((g->dtc.GetTimeOfDay() > 2 * 60) && (g->dtc.GetTimeOfDay() < 4 * 60))
		{
			unsigned char color = (unsigned char)((4 * 60 - g->dtc.GetTimeOfDay()) * 128 / 120);
			m_cubemapClouds.SetColor(color, color, color, 255);
		}
		//m_cubemapClouds.SetFov(45);
		m_cubemapClouds.SetCamera(x, y, z);
		g->cm.SetFarPlane(GetSize() * 3.0f);
		m_cubemapClouds.Draw();
		
		g->stp.SetState(ST_BLEND, true);
		g->stp.SetBlendFunc(GL_ONE, GL_ONE);
	}

	g->stp.SetMaterial(-1);
	g->stp.PrepareForRender();
	
	g->stp.SetState(ST_ZWRITE, false);
	g->stp.SetState(ST_ZTEST, false);

	assert(m_noiseTextureID != -1);

	m_shader->Bind(0);
		
	float x, y, z;
	g->cm.GetCameraEye(&x, &y, &z);

	float alpha = 1.0f;
	if ((g->dtc.GetTimeOfDay() > 4 * 60) && (g->dtc.GetTimeOfDay() < 5 * 60))
	{
		alpha = (float)(g->dtc.GetTimeOfDay() - 4 * 60) / 120.0f + 0.5f;
	}

	if (g->dtc.GetTimeOfDay() <= 4 * 60)
	{
		alpha = 0.5f;
	}

	if (g->dtc.GetTimeOfDay() >= 22 * 60)
	{
		alpha = 0.5f;
	}

	if ((g->dtc.GetTimeOfDay() > 21 * 60) && (g->dtc.GetTimeOfDay() < 22 * 60))
	{
		alpha = 1.0f - (float)(g->dtc.GetTimeOfDay() - 21 * 60) / 120.0f;
	}

	m_shader->SetUniformFloat(m_alphaLocation, alpha);
	m_shader->SetUniformFloat(m_timeLocation, (float)(g->ne.time%(g->dtc.GetDayDuration() * 4 * 1000)) / 2000.0f * (float)g->rs.GetInt(CLOUDS_SPEED) / 100.0f);
	m_shader->SetUniformVector(m_sunposLocation, CVector4D(g->dtc.GetSunPosition(), 1));
	float dT = 9.5f * sin(3.14f * (float)(g->ne.time%(g->dtc.GetDayDuration() * 1000) - g->dtc.GetDayDuration() * 1000 / 6) / (float)(g->dtc.GetDayDuration() * 750));
	dT -= 2.0f;
	if (dT < 0)
	{
		dT = 0;
	}
		
	GLFUNC(glDisable)(GL_TEXTURE_2D);	
	GLFUNC(glEnable)(GL_TEXTURE_3D);	
	g->t3dm.BindTexture3D(m_noiseTextureID);

	g->cm.SetFarPlane(GetSize() * 5.0f);
	float aspect = g->scm.GetActiveScene()->GetViewport()->GetAspect();
	g->cm.SetFov((float)(g->rs.GetInt(FOV) - 45), aspect);
	g->stp.PushMatrix();	
	if (!g->rs.GetBool(VERTICAL_AXE_Z))
	{
		GLFUNC(glTranslatef)(x, 0.0f, z);
	}
	else
	{
		GLFUNC(glTranslatef)(x, y, -3.0f);
	}
	
	g->stp.PrepareForRender();
	
	GLFUNC(glDisable)(GL_CULL_FACE);
	g->stp.SetState(ST_CULL_FACE, false);

	float scl = GetSize() / 22.0f;
	GLFUNC(glScalef)(scl, scl, scl);

	GLFUNC(glEnable)(GL_CLIP_PLANE0);
	double plane[4] = {0, 0, 1, 0.0f};
	GLFUNC(glClipPlane)(GL_CLIP_PLANE0, (double *)&plane);

	if (g->gi.GetVendorID() != VENDOR_INTEL)
	{
		if (-1 == m_displayListID)
		{
			m_displayListID = g->dlm.CreateDisplayList("dynamic_clouds");
			g->dlm.BindDisplayList(m_displayListID);
			miniglut::glutSolidSphere (22.0f, 40, 40);
			g->dlm.UnbindDisplayList();
		}
		else
		{
			g->dlm.CallDisplayList(m_displayListID);
		}
	}
	else
	{
		miniglut::glutSolidSphere (22.0f, 40, 40);
	}

	GLFUNC(glEnable)(GL_TEXTURE_2D);	

	GLFUNC(glDisable)(GL_CLIP_PLANE0);

	GLFUNC(glDisable)(GL_TEXTURE_3D);

	g->stp.SetState(ST_BLEND, false);
	g->stp.PrepareForRender();
	
	g->stp.SetBlendFunc(GL_ONE, GL_ONE);
	int sunTextureID = g->tm.GetObjectNumber("SUN.JPG");
	g->tm.BindTexture(sunTextureID);
	CVector3D sunPos = g->dtc.GetSunPosition() * (float)(20.0f / 150.0f);
	g->br.NewFrame();
	g->stp.SetColor(255, 255, 255);

	float screenX = 0.0f, screenY = 0.0f/*, screenZ = 0.0f*/;
	// commented because of sun rays are now not used
	//g->scm.GetActiveScene()->GetViewport()->GetActualScreenCoords(sunPos.x, sunPos.y, sunPos.z, &screenX, &screenY, &screenZ);
	CHDRPostEffect::SetSunProjection(screenX, screenY);

	g->sm.UnbindShader();

	g->stp.SetState(ST_BLEND, true);
	g->br.ShowBillboard(sunPos.x, sunPos.y, sunPos.z, 3.0f);	

	g->cm.SetFov((float)(g->rs.GetInt(FOV) - 45), aspect);
		
	g->stp.PopMatrix();

	g->stp.SetColor(255, 255, 255);

	g->cm.ExtractFrustum();

	g->stp.SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	g->stp.SetState(ST_BLEND, false);
	g->stp.SetState(ST_ZWRITE, true);
	g->stp.SetState(ST_ZTEST, true);
}

void CDynamicClouds::SetSunTextureName(std::string name)
{
	m_sunTextureID = g->tm.GetObjectNumber(name);
//	assert(m_sunTextureID >= 0);
}

void CDynamicClouds::SetSunTextureID(int textureID)
{
	m_sunTextureID = textureID;
	//assert(m_sunTextureID >= 0);
}

void CDynamicClouds::SetStarsTextureName(std::string name)
{
	m_starsTextureID = g->tm.GetObjectNumber(name);
//	assert(m_starsTextureID >= 0);

	m_cubemapClouds.SetCubemapTextures(name, name, name, name, name, name);
}

void CDynamicClouds::SetStarsTextureID(int textureID)
{
	m_starsTextureID = textureID;
	assert(m_starsTextureID >= 0);
}

void CDynamicClouds::Set3DNoiseTextureName(std::string name)
{
	m_noiseTextureID = g->t3dm.GetObjectNumber(name);
	assert(m_noiseTextureID >= 0);
}

void CDynamicClouds::Set3DNoiseTextureID(int textureID)
{
	m_noiseTextureID = textureID;
	assert(m_noiseTextureID >= 0);
}

void CDynamicClouds::SetShader(std::string shader)
{
	m_shaderName = shader;
}

CDynamicClouds::~CDynamicClouds()
{
	g->sm.DeleteChangesListener(this);
}