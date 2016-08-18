
#include "StdAfx.h"
#include "TinyXML.h"
#include "HelperFunctions.h"
#include "HDRPostEffect.h"
#include "GlobalSingletonStorage.h"
#include "cal3d/memory_leak.h"

CHDRPostEffect::CHDRPostEffect():
	m_isOk(true),
	m_buffer(NULL),
	m_buffer0(NULL),
	m_buffer2(NULL),
	m_buffer3(NULL),
	m_fixedWidth(0),
	m_fixedHeight(0),
	m_textureWidth(0),
	m_textureHeight(0),
	m_isFirstCopy(true),
	m_multisamplePower(1),
	m_xBlurShaderID(0xFFFFFFFF),
	m_yBlurShaderID(0xFFFFFFFF),	
	m_refractionTextureWidth(0),
	m_refractionTextureHeight(0),
	m_zbufferTextureID(0xFFFFFFFF),
	m_filterMapShaderID(0xFFFFFFFF),
	m_refractionTextureID(0xFFFFFFFF),
	m_xBlurShader(NULL),
	m_yBlurShader(NULL),
	m_filterMapShader(NULL),
	m_toneMapShader(NULL),
	m_toneMapOculusRiftShader(NULL),
	m_toneMapShaftsShader(NULL)
{
	GetTextureSize(m_textureWidth, m_textureHeight);

	InitOculusRiftSupport();

	for (int k = 0; k < 40; k++)
	{
		g->tm.AddObject("garbage_texture_" + IntToStr(k));
	}
		
	InitBuffers();

	g->sm.AddChangesListener(this);

	OnChanged(0);	
}

CHDRPostEffect::CHDRPostEffect(int fixedWidth, int fixedHeight):
	m_isOk(true),
	m_buffer(NULL),
	m_buffer0(NULL),
	m_buffer2(NULL),
	m_buffer3(NULL),
	m_textureWidth(0),
	m_textureHeight(0),
	m_isFirstCopy(true),
	m_multisamplePower(1),
	m_fixedWidth(fixedWidth),
	m_fixedHeight(fixedHeight),
	m_xBlurShaderID(0xFFFFFFFF),
	m_yBlurShaderID(0xFFFFFFFF),	
	m_refractionTextureWidth(0),
	m_refractionTextureHeight(0),
	m_filterMapShaderID(0xFFFFFFFF),
	m_refractionTextureID(0xFFFFFFFF),
	m_xBlurShader(NULL),
	m_yBlurShader(NULL),
	m_filterMapShader(NULL),
	m_toneMapShader(NULL),
	m_toneMapOculusRiftShader(NULL),
	m_toneMapShaftsShader(NULL)
{
	GetTextureSize(m_textureWidth, m_textureHeight);

	InitOculusRiftSupport();
		
	InitBuffers();

	OnChanged(0);	
}

void CHDRPostEffect::InitOculusRiftSupport()
{
	m_toneMapOculusRiftShaderID = g->sm.GetObjectNumber("shaders/tonemap_oculusrift");
	m_toneMapOculusRiftShader = g->sm.GetObjectPointer(m_toneMapOculusRiftShaderID);
}

bool CHDRPostEffect::IsOk()
{
	return m_isOk;	
}

void CHDRPostEffect::PrepareForReading()
{
	m_buffer->PrepareForReading();
}

void CHDRPostEffect::EndReading()
{
	m_buffer->EndReading();
}

void CHDRPostEffect::SetMultisamplePower(const unsigned char multisamplePower)
{
	m_multisamplePower = multisamplePower;
}

unsigned char CHDRPostEffect::GetMultisamplePower()const
{
	return m_multisamplePower;
}

CShader* CHDRPostEffect::GetOculusRiftShader()
{
	return m_toneMapOculusRiftShader;
}

void CHDRPostEffect::OnChanged(int /*eventID*/)
{
	m_toneMapShaderID = g->sm.GetObjectNumber("shaders/tonemap");
	m_toneMapShader = g->sm.GetObjectPointer(m_toneMapShaderID);

	m_filterMapShaderID = g->sm.GetObjectNumber("shaders/filtermap");
	m_filterMapShader = g->sm.GetObjectPointer(m_filterMapShaderID);
	
	m_filterMapShader->Bind(0);
	m_filterMapShader->SetUniformTexture("mainMap", 0);
	m_filterMapShader->Unbind();

	m_xBlurShaderID = g->sm.GetObjectNumber("shaders/xblur");
	m_xBlurShader = g->sm.GetObjectPointer(m_xBlurShaderID);

	m_xBlurShader->Bind(0);
	m_xBlurShader->SetUniformTexture("mainTex", 0);
	m_xBlurShader->Unbind();

	m_yBlurShaderID = g->sm.GetObjectNumber("shaders/yblur");
	m_yBlurShader = g->sm.GetObjectPointer(m_yBlurShaderID);

	m_yBlurShader->Bind(0);
	m_yBlurShader->SetUniformTexture("mainTex", 0);
	m_yBlurShader->Unbind();

	m_toneMapShader->Bind(0);
	m_toneMapShader->SetUniformFloat("blurness", g->dtc.GetBlurness());
	m_toneMapShader->SetUniformFloat("exposure", 1.0f);
	m_toneMapShader->SetUniformTexture("mainMap", 0);
	m_toneMapShader->SetUniformTexture("blurMap", 1);	
	m_toneMapShader->SetUniformTexture("depthMap", 2);	
	m_toneMapShader->Unbind();
	
	m_toneMapOculusRiftShader->Bind(0);
	m_toneMapOculusRiftShader->SetUniformFloat("blurness", g->dtc.GetBlurness());
	m_toneMapOculusRiftShader->SetUniformTexture("mainMap", 0);
	m_toneMapOculusRiftShader->SetUniformTexture("blurMap", 1);	
	m_toneMapOculusRiftShader->SetUniformTexture("depthMap", 2);
	m_toneMapOculusRiftShader->Unbind();
	
	m_toneMapShaftsShaderID = g->sm.GetObjectNumber("shaders/tonemap_shafts");
	m_toneMapShaftsShader = g->sm.GetObjectPointer(m_toneMapShaftsShaderID);

	m_toneMapShaftsShader->Bind(0);
	m_toneMapShaftsShader->SetUniformFloat("blurness", g->dtc.GetBlurness());
	m_toneMapShaftsShader->SetUniformFloat("exposure", 1.0f);
	m_toneMapShaftsShader->SetUniformTexture("mainMap", 0);
	m_toneMapShaftsShader->SetUniformTexture("blurMap", 1);	
	m_sunLocation = m_toneMapShaftsShader->SetUniformVector("sunPos", CVector4D(1, 1, 1, 1));
	m_toneMapShaftsShader->Unbind();
}

void CHDRPostEffect::SaveScreen(std::wstring fileName, SCropRect* const rect)
{
	if (rect)
	{
		rect->x = 0;
		rect->y = 0;
	}

	g->tm.SaveTexture(m_bufferTexID, fileName, rect);
}

void CHDRPostEffect::OnSceneDrawStart(CViewport* /*viewport*/)
{
	if (!g->rs.GetBool(POSTEFFECT_ENABLED))
	{
		return;
	}

	if (AreTextureSettingsChanged())
	{
		InitBuffers();
	}

	if (g->rs.GetInt(RENDER_TARGET_WITH_STENCIL_MODE) != RENDER_TARGET_WITH_STENCIL_COPY_FROM_FRAMEBUFFER)
	{
		m_buffer->Bind();
	}
	else
	{
		g->stp.PushMatrix();
	}

	g->stp.SetViewport(g->stp.GetCurrentWidth(), g->stp.GetCurrentHeight());

	GLFUNC(glClear)(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void CHDRPostEffect::DoYBlur()
{
	g->stp.SetViewport((int)(m_textureWidth / 4), (int)(m_textureHeight / 4));

	m_buffer3->Bind();
	m_yBlurShader->Bind(0);

	g->stp.PushMatrix();
	GLFUNC(glScalef)(1.0f, 0.5f, 1.0f);

	g->stp.SetState(ST_ZWRITE, false);
#pragma warning(push)
#pragma warning(disable : 4239)
	g->sp.PutNormalSprite(0, -(int)m_textureHeight/8, 0, 0, m_textureWidth/4, m_textureHeight/2, m_buffer2TexID, CColor3(255, 255, 255));
#pragma warning(pop)
	g->stp.SetState(ST_ZWRITE, true);

	g->stp.PopMatrix();

	m_yBlurShader->Unbind();
	m_buffer3->Unbind();
}

void CHDRPostEffect::DoXBlur()
{
	g->stp.SetViewport((int)(m_textureWidth / 4), (int)(m_textureHeight / 2));

	m_buffer2->Bind();
	m_yBlurShader->Bind(0);

	g->stp.PushMatrix();
	GLFUNC(glScalef)(0.5f, 1.0f, 1.0f);

	g->stp.SetState(ST_ZWRITE, false);
#pragma warning(push)
#pragma warning(disable : 4239)
	g->sp.PutNormalSprite(-(int)m_textureWidth/8, 0, 0, 0, m_textureWidth/2, m_textureHeight/2, m_buffer0TexID, CColor3(255, 255, 255));
#pragma warning(pop)
	g->stp.SetState(ST_ZWRITE, true);

	g->stp.PopMatrix();

	m_yBlurShader->Unbind();
	m_buffer2->Unbind();
}

void CHDRPostEffect::DoFilterMap()
{
	g->stp.SetViewport((int)(m_textureWidth / 2), (int)(m_textureHeight / 2));

	m_buffer0->Bind();
	m_filterMapShader->Bind(0);
	
	g->stp.PushMatrix();
	GLFUNC(glScalef)(0.5f, 0.5f, 1.0f);

	g->stp.SetState(ST_ZWRITE, false);
#pragma warning(push)
#pragma warning(disable : 4239)
	g->sp.PutNormalSprite(-(int)m_textureWidth/4, -(int)m_textureHeight/4, 0, 0, m_textureWidth, m_textureHeight, m_bufferTexID, CColor3(255, 255, 255));
#pragma warning(pop)
	g->stp.SetState(ST_ZWRITE, true);

	g->stp.PopMatrix();

	m_filterMapShader->Unbind();
	m_buffer0->Unbind();
}

void CHDRPostEffect::SetSunProjection(float sunX, float sunY)
{
	m_sunX = sunX;
	m_sunY = sunY;
}

void CHDRPostEffect::ComposeMaps(CViewport* viewport, CRenderTarget* renderTarget)
{
	g->stp.RestoreViewport();
	
	g->stp.SetActiveTexture(1);
	g->tm.BindTexture(m_buffer3TexID);
	g->stp.SetActiveTexture(0);	

	float w = 0.0f;
	if (g->cm.GetActiveCamera())
	{
		CVector3D cam = g->cm.GetActiveCamera()->GetLookAt();
		cam -= g->cm.GetActiveCamera()->GetEyePosition();
		cam.Normalize();
		CVector3D pos2 = g->dtc.GetSunPosition();
		pos2.Normalize();
		w = cam.Dot(pos2);

		if (w < 0.0f)
		{
			w = 0.0f;
		}
	}

	CShader* shader = NULL;
	if ((0 == w) || (g->dtc.GetTimeOfDay() < 60 * 4) || (g->dtc.GetTimeOfDay() > 22 * 60)  || ((g->dtc.GetTimeOfDay() > 60 * 10) && (g->dtc.GetTimeOfDay() < 60 * 17)))
	{
		if (g->rs.IsOculusRiftEnabled())
		{
			shader = m_toneMapOculusRiftShader;
		}
		else
		{
			shader = m_toneMapShader;
		}
		shader->Bind(0);
	}
	else
	{
		shader = m_toneMapShaftsShader;
		shader->Bind(0);
		if (w != 0.0f)
		{
			shader->SetUniformVector(m_sunLocation, CVector4D(m_sunX / (float)m_buffer->GetWidth(), 1.0f - m_sunY / (float)m_buffer->GetHeight(), 1.0f, w));
		}
	}

	if (renderTarget)
	{
		renderTarget->Bind();
	}
		
	g->stp.PushMatrix();

	g->stp.SetColor(255, 255, 255);
	g->stp.SetState(ST_BLEND, false);
	g->cm.SetCamera2d();
	g->stp.SetState(ST_ZWRITE, false);
#pragma warning(push)
#pragma warning(disable : 4239)
	if (g->rs.IsOculusRiftEnabled()) {
		IOculusRiftSupport* oculus = g->scm.GetActiveScene()->GetOculusRift();
		IOculusRiftSupport::ShaderParams params[2];
		float centerX = (float)g->stp.GetCurrentWidth() / (float)m_textureWidth / 2.0f;
		float centerY = (float)g->stp.GetCurrentHeight() / (float)m_textureHeight;
		oculus->GetShaderParams(params);
		for (int eye = 0; eye < 2; ++eye) {
			for(int i = 0; i < 2; ++i) {
				params[eye].uvScaleOffset[i].x *= centerX;
				params[eye].uvScaleOffset[i].y *= centerY;
			}
		}
		shader->SetUniformVector2D("uvScale_l", params[0].uvScaleOffset[0]);
		shader->SetUniformVector2D("uvOffset_l", params[0].uvScaleOffset[1]);
		shader->SetUniformMatrix("eyeRotationStart_l", params[0].timeWarpMatrices[0].m[0]);
		shader->SetUniformMatrix("eyeRotationEnd_l", params[0].timeWarpMatrices[1].m[0]);
		shader->SetUniformVector2D("uvScale_r", params[1].uvScaleOffset[0]);
		shader->SetUniformVector2D("uvOffset_r", params[1].uvScaleOffset[1]);
		shader->SetUniformMatrix("eyeRotationStart_r", params[1].timeWarpMatrices[0].m[0]);
		shader->SetUniformMatrix("eyeRotationEnd_r", params[1].timeWarpMatrices[1].m[0]);
		g->sp.PutOculusDistortionMesh(m_bufferTexID);
		oculus->EndRender();
	} else {
		g->sp.PutNormalSprite(viewport->GetX(), -((int)m_textureHeight - (int)g->stp.GetCurrentHeight()) - viewport->GetY(), 0, 0, m_textureWidth, m_textureHeight, m_bufferTexID, CColor3(255, 255, 255));
	}
#pragma warning(pop)
	g->stp.SetState(ST_ZWRITE, true);

	g->stp.PopMatrix();
	shader->Unbind();	

	if (renderTarget)
	{
		renderTarget->Unbind();
	}

	g->tm.DisableMultitexturing2TL();
}

void CHDRPostEffect::OnSceneDrawEnd(CViewport* viewport, CRenderTarget* renderTarget)
{
	m_lensFlares.Draw();

	if (!g->rs.GetBool(POSTEFFECT_ENABLED))
	{
		return;
	}	

	if (g->rs.GetInt(RENDER_TARGET_WITH_STENCIL_MODE) != RENDER_TARGET_WITH_STENCIL_COPY_FROM_FRAMEBUFFER)
	{
		m_buffer->Unbind();
	}
	else
	{
		g->stp.PopMatrix();

		g->tm.BindTexture(m_bufferTexID);
		GLFUNC(glCopyTexSubImage2D)(GL_TEXTURE_2D, 0 ,0 ,0 ,0 ,0, g->stp.GetCurrentWidth(), g->stp.GetCurrentHeight());
		
		if (m_isFirstCopy)
		{
			m_isFirstCopy = false;

			int glError = glGetError();
			if (glError != GL_NO_ERROR)
			{
				m_textureWidth = 1023;
				m_textureHeight = 1023;
				g->rs.SetInt(RENDER_TARGET_WITH_STENCIL_MODE, RENDER_TARGET_WITH_STENCIL_PACKED_DEPTH_STENCIL);
			}
		}
		
		GLFUNC(glClear)(GL_COLOR_BUFFER_BIT);
	}

	g->stp.SetState(ST_ZTEST, false);
	g->stp.SetState(ST_CULL_FACE, false);

	g->stp.PushMatrix();
	g->cm.SetCamera2d();

	bool isTexturesDisabled = g->rs.GetBool(DISABLE_TEXTURES);
	g->rs.SetBool(DISABLE_TEXTURES, false);

	// ------------------BlurMap(); 

	static int calls = 0;

	calls++;

	if (calls%2 >= 0)
	{
		g->stp.PushMatrix();

		DoFilterMap();
		DoXBlur();
		DoYBlur();

		g->stp.PopMatrix();
	}

	// ----------------BlurMap();

	ComposeMaps(viewport, renderTarget);

	g->rs.SetBool(DISABLE_TEXTURES, isTexturesDisabled);

	g->stp.PopMatrix();
}

void CHDRPostEffect::GetShaderName(void* document, std::wstring fileName, std::string name, std::string& shaderName)
{
	TiXmlDocument* doc = (TiXmlDocument*)document;

	TiXmlNode* object = doc->FirstChild((name + "_shader").c_str());

	USES_CONVERSION;
	if ((doc->NoChildren()) || (!object))
	{
		g->lw.WriteLn("[ERROR] HDR post-effect ", W2A(fileName.c_str()), "has no ", name, " shader");
		return;
	}

	if (object->ToElement()->GetText())
	{
		shaderName = object->ToElement()->GetText();
	}
	else
	{		
		g->lw.WriteLn("[ERROR] HDR post-effect ", W2A(fileName.c_str())," has invalid ", name," shader");
		return;
	}
}

void CHDRPostEffect::OnLoadFromXML(std::wstring& fileName)
{
	USES_CONVERSION;
	TiXmlDocument doc( W2A(fileName.c_str()));
	bool isLoaded = g->rl->LoadXml( (void*)&doc, fileName);

	if (!isLoaded)
	{
		g->lw.WriteLn("[ERROR] Loading HDR post-effect ", fileName, " failed...");
		return;
	}

	std::string shaderName;

	GetShaderName(&doc, fileName, "filtermap", shaderName);

	m_filterMapShaderID = g->sm.GetObjectNumber(shaderName);
	if (m_filterMapShaderID == -1)
	{
		g->lw.WriteLn("[ERROR] Filtermap shader for HDR post-effect ", fileName, " is not found!");
		return;
	}	

	GetShaderName(&doc, fileName, "xblur", shaderName);

	m_xBlurShaderID = g->sm.GetObjectNumber(shaderName);
	if (m_xBlurShaderID == -1)
	{
		g->lw.WriteLn("[ERROR] X-Blur shader for HDR post-effect ", fileName, " is not found!");
		return;
	}	

	GetShaderName(&doc, fileName, "yblur", shaderName);

	m_yBlurShaderID = g->sm.GetObjectNumber(shaderName);
	if (m_yBlurShaderID == -1)
	{
		g->lw.WriteLn("[ERROR] Y-Blur shader for HDR post-effect ", fileName, " is not found!");
		return;
	}	
}

void CHDRPostEffect::GetTextureSize(unsigned int& textureWidth, unsigned int& textureHeight)
{
	if ((0 != m_fixedWidth) && (0 != m_fixedHeight))
	{
		textureWidth = m_fixedWidth;
		textureHeight = m_fixedHeight;
		return;
	}

	int x, y;
	unsigned int windowWidth, windowHeight;
	g->stp.GetSize(x, y, windowWidth, windowHeight);

	if (g->rs.GetBool(CAVE_SUPPORT))
	{
		windowWidth /= 4;
	}
	
	unsigned int width = 2;
	unsigned int height = 2;
	while (width < windowWidth)
	{
		width <<= 1;
	}
	textureWidth = width;

	while (height < windowHeight)
	{
		height <<= 1;
	}
	textureHeight = height;
}

bool CHDRPostEffect::AreTextureSettingsChanged()
{
	unsigned int newTextureWidth = 0;
	unsigned int newTextureHeight = 0;

	GetTextureSize(newTextureWidth, newTextureHeight);
	
	if ((newTextureWidth != m_textureWidth) || (newTextureHeight != m_textureHeight))
	{
		m_textureWidth = newTextureWidth;
		m_textureHeight = newTextureHeight;

		int x, y;
		unsigned int windowWidth, windowHeight;
		g->stp.GetSize(x, y, windowWidth, windowHeight);
		g->lw.WriteLn("Window size changed: ", newTextureWidth, " ", newTextureHeight, " ", windowWidth, " ", windowHeight);

		return true;
	}

	if (m_multisamplePower != m_buffer->GetMultisamplePower())
	{
		return true;
	}

	return false;
}

void CHDRPostEffect::InitBuffers()
{
	DeleteBuffers();

	m_isOk = true;	

	int format = CFrameBuffer::depth24_stencil8;
	if (g->gi.GetVendorID() == VENDOR_INTEL)
	{
		format = CFrameBuffer::depth32;
	}
	
	switch (g->rs.GetInt(RENDER_TARGET_WITH_STENCIL_MODE))
	{
	case RENDER_TARGET_WITH_STENCIL_COPY_FROM_FRAMEBUFFER:
		MP_NEW_V3(m_buffer, CFrameBuffer, m_textureWidth, m_textureHeight, 0);
		break;

	case RENDER_TARGET_WITH_STENCIL_PACKED_DEPTH_STENCIL:
		MP_NEW_V3(m_buffer, CFrameBuffer, m_textureWidth, m_textureHeight, format);
		break;

	case RENDER_TARGET_WITH_STENCIL_SEPARATE_BUFFERS:
		MP_NEW_V3(m_buffer, CFrameBuffer, m_textureWidth, m_textureHeight, format);
		break;
	}

	m_buffer->SetMultisamplePower(m_multisamplePower);
	
	MP_NEW_V3(m_buffer0, CFrameBuffer, m_textureWidth / 2, m_textureHeight / 2, 0);

	MP_NEW_V3(m_buffer2, CFrameBuffer, m_textureWidth / 4, m_textureHeight / 2, 0);

	MP_NEW_V3(m_buffer3, CFrameBuffer, m_textureWidth / 4, m_textureHeight / 4, 0);
	
	unsigned screenMap = m_buffer->CreateColorTexture(GL_RGBA, GL_RGBA, true);
	m_bufferTexID = g->tm.ManageGLTexture("hdr_buffer", screenMap, m_textureWidth, m_textureHeight);
	{
		CTexture* texture = g->tm.GetObjectPointer(m_bufferTexID);
		texture->UpdateCurrentFiltration(GL_LINEAR, GL_LINEAR);
	}

	if (g->rs.GetInt(RENDER_TARGET_WITH_STENCIL_MODE) != RENDER_TARGET_WITH_STENCIL_COPY_FROM_FRAMEBUFFER)
	{
		/*bool isCreated = */m_buffer->Create(true);
		m_buffer->Bind();
		m_buffer->AttachColorTexture(screenMap);
		if (!m_buffer->IsOk())
		{
			m_isOk = false;
		}

		if (m_multisamplePower > 1)
		if (!m_buffer->IsMultisampleOk())
		{
			m_isOk = false;
		}
		m_buffer->Unbind();		
	}

	screenMap = m_buffer0->CreateColorTexture(GL_RGBA, GL_RGBA, true);
	m_buffer0TexID = g->tm.ManageGLTexture("hdr_buffer0", screenMap, m_textureWidth / 2, m_textureHeight / 2);

	{
		CTexture* texture = g->tm.GetObjectPointer(m_buffer0TexID);
		texture->UpdateCurrentFiltration(GL_LINEAR, GL_LINEAR);
	}

	m_buffer0->Create(false);
	m_buffer0->Bind();
	m_buffer0->AttachColorTexture(screenMap);
	if (!m_buffer0->IsOk())
	{
		m_isOk = false;
	}
	m_buffer0->Unbind();

	screenMap = m_buffer3->CreateColorTexture(GL_RGBA, GL_RGBA, true);
	m_buffer3TexID = g->tm.ManageGLTexture("hdr_buffer3", screenMap, m_textureWidth / 4, m_textureHeight / 4);

	{
		CTexture* texture = g->tm.GetObjectPointer(m_buffer3TexID);
		texture->UpdateCurrentFiltration(GL_LINEAR, GL_LINEAR);
	}

	m_buffer3->Create(false);
	m_buffer3->Bind();
	m_buffer3->AttachColorTexture(screenMap);
	if (!m_buffer3->IsOk())
	{
		m_isOk = false;
	}
	m_buffer3->Unbind();

	screenMap = m_buffer2->CreateColorTexture(GL_RGBA, GL_RGBA, true);
	m_buffer2TexID = g->tm.ManageGLTexture("hdr_buffer2", screenMap, m_textureWidth / 4, m_textureHeight / 2);

	{
		CTexture* texture = g->tm.GetObjectPointer(m_buffer2TexID);
		texture->UpdateCurrentFiltration(GL_LINEAR, GL_LINEAR);
	}

	m_buffer2->Create(false);
	m_buffer2->Bind();
	m_buffer2->AttachColorTexture(screenMap);
	if (!m_buffer2->IsOk())
	{
		m_isOk = false;
	}
	m_buffer2->Unbind();
}

void CHDRPostEffect::DeleteBuffers()
{
	g->lw.WriteLn("Delete main buffer");

	if (m_buffer != NULL)
	{
		MP_DELETE(m_buffer);
		if (m_bufferTexID != -1)
		{
			g->tm.DeleteObject(m_bufferTexID);
		}
	}

	g->lw.WriteLn("Delete 2st buffer");

	if (m_buffer0 != NULL)
	{
		MP_DELETE(m_buffer0);
		g->tm.DeleteObject(m_buffer0TexID);
	}

	g->lw.WriteLn("Delete 3rd buffer");

	if (m_buffer2 != NULL)
	{
		MP_DELETE(m_buffer2);
		g->tm.DeleteObject(m_buffer2TexID);
	}

	g->lw.WriteLn("Delete 4st buffer");

	if (m_buffer3 != NULL)
	{
		MP_DELETE(m_buffer3);
		g->tm.DeleteObject(m_buffer3TexID);
	}

	g->lw.WriteLn("Deleting - ok.");	
}

unsigned int CHDRPostEffect::GetRefractionTextureID()const
{
	return m_refractionTextureID;
}

void CHDRPostEffect::BakeRefractionTexture()
{
	if (!g->rs.GetBool(REFRACTION_TEXTURE_NEEDED))
	{
		return;
	}

	int x, y;
	unsigned int windowWidth, windowHeight;
	g->stp.GetSize(x, y, windowWidth, windowHeight);

	if ((windowWidth != m_refractionTextureWidth) && (windowHeight != m_refractionTextureHeight))
	{
		m_refractionTextureWidth = windowWidth;
		m_refractionTextureHeight = windowHeight;

		m_refractionTextureID = m_buffer->CreateColorTexture(GL_TEXTURE_2D, m_refractionTextureWidth,
			m_refractionTextureHeight, GL_RGBA, GL_RGBA, true);
		m_refractionTextureID = g->tm.ManageGLTexture("refraction", m_refractionTextureID, m_refractionTextureWidth,
			m_refractionTextureHeight);	

		CTexture* texture = g->tm.GetObjectPointer(m_refractionTextureID);
		GLFUNC(glBindTexture)(GL_TEXTURE_2D, texture->GetNumber());
		GLFUNC(glTexImage2D)(GL_TEXTURE_2D, 0, GL_RGBA, 0, m_refractionTextureWidth, m_refractionTextureHeight, GL_RGBA,
			GL_UNSIGNED_BYTE, NULL);
		g->tm.RefreshTextureOptimization();
	}

	CTexture* texture = g->tm.GetObjectPointer(m_refractionTextureID);
	GLFUNC(glBindTexture)(GL_TEXTURE_2D, texture->GetNumber());
	GLFUNC(glCopyTexSubImage2D)(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_refractionTextureWidth, m_refractionTextureHeight);
	g->tm.RefreshTextureOptimization();

	/*static int cnt = 0;
	cnt++;

	if (cnt%250 == 0)
	{
		CTexture* texture = g->tm.GetObjectPointer(m_refractionTextureID);
		texture->SetTextureWidth(m_refractionTextureWidth);
		texture->SetTextureHeight(m_refractionTextureHeight);
		texture->SetTextureRealWidth(m_refractionTextureWidth);
		texture->SetTextureRealHeight(m_refractionTextureHeight);
		std::string fileName = "d:\\refraction.png";
		g->tm.SaveTexture(m_refractionTextureID, fileName);
	}*/
}

int CHDRPostEffect::GetMainTextureID()const
{
	return m_bufferTexID;
}

void CHDRPostEffect::DetachMainTexture()
{
	m_bufferTexID = -1;
	if (m_buffer)
	{
		m_buffer->DetachColorTexture();
	}
}

CHDRPostEffect::~CHDRPostEffect()
{
	g->sm.DeleteChangesListener(this);
	DeleteBuffers();
}