
#include "StdAfx.h"
#include "SimpleDrawingTarget.h"
#include "GlobalSingletonStorage.h"

#define MAX_WIDTH		1024
#define MAX_HEIGHT		1024

CSimpleDrawingTarget::CSimpleDrawingTarget():
	m_backBufferTextureID(0xFFFFFFFF),
	MP_STRING_INIT(m_name),
	m_isTransparent(false),
	m_clearColorR(255),
	m_clearColorG(255),
	m_clearColorB(255),
	m_clearColorA(255),
	m_isCreated(false),	
	m_isBinded(false),
	m_texture(NULL),
	m_invScale(1),
	m_isOk(true),
	m_height(0),
	m_width(0),
	m_flags(0),
	m_fbo(NULL)
{
	m_isCleared = false;	
}

void CSimpleDrawingTarget::SetInvScale(unsigned char invScale)
{
	m_invScale = invScale;
}

unsigned char CSimpleDrawingTarget::GetInvScale()const
{
	return m_invScale;
}

void CSimpleDrawingTarget::SaveTexture(std::wstring fileName)
{
	g->tm.SaveTexture(m_texture, fileName);
}

void CSimpleDrawingTarget::SetClearColor(unsigned char r, unsigned char g,unsigned char b, unsigned char a)
{
	m_clearColorR = r;
	m_clearColorG = g;
	m_clearColorB = b;
	m_clearColorA = a;
}

void CSimpleDrawingTarget::SetFormat(bool isTransparent, unsigned int flags)
{
	m_isTransparent = isTransparent;
	m_flags = flags;
}

void CSimpleDrawingTarget::SetWidth(unsigned int width)
{
	m_width = width;
}

void CSimpleDrawingTarget::SetHeight(unsigned int height)
{
	m_height = height;
}

unsigned int CSimpleDrawingTarget::GetWidth()
{
	return m_width;
}

unsigned int CSimpleDrawingTarget::GetHeight()
{
	return m_height;
}

void CSimpleDrawingTarget::AttachTexture(CTexture* texture)
{
	m_texture = texture;

	if (m_fbo)
	{
		m_fbo->Bind();
		m_fbo->AttachColorTexture(m_texture->GetNumber());
		m_fbo->Unbind();
	}
}

unsigned char* CSimpleDrawingTarget::GetClearedMemory(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	static unsigned char* data = NULL;
	static unsigned char _r = 0;
	static unsigned char _g = 0;
	static unsigned char _b = 0;
	static unsigned char _a = 0;
	if (!data)
	{
		data = MP_NEW_ARR(unsigned char, MAX_WIDTH * MAX_HEIGHT * 4);
	}
	if ((_r != r) || (_g != g) || (_b != b) || (_a != a)) 
	{
		_r = r;
		_g = g;
		_b = b;
		_a = a;
		
		for (unsigned int i = 0; i < MAX_WIDTH * MAX_HEIGHT * 4; i += 4)
		{
			data[i] = r;
			data[i + 1] = g;
			data[i + 2] = b;
			data[i + 3] = a;
		}	
	}

	return data;
}

unsigned char* CSimpleDrawingTarget::GetMemory()
{
	static unsigned char* data = NULL;
	if (!data)
	{
		data = MP_NEW_ARR(unsigned char, MAX_WIDTH * MAX_HEIGHT * 4);
	}
	return data;
}

bool CSimpleDrawingTarget::Create()
{
	if (m_isCreated)
	{
		return true;
	}

	if (m_texture)
	{
		if (m_texture->GetTextureWidth() == 4)
		{
			g->tm.BindTexture(m_texture->GetID());
			
			GLFUNC(glTexImage2D)(GL_TEXTURE_2D, 0, GL_RGBA, GetScaledWidth(), GetScaledHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

			g->tm.RefreshTextureOptimization();
			m_texture->SetTextureWidth(GetScaledWidth());
			m_texture->SetTextureHeight(GetScaledHeight());
			m_texture->UpdateCurrentFiltration(GL_LINEAR, GL_LINEAR);
		}
	}

	MP_NEW_V3(m_fbo, CRenderTarget, GetScaledWidth(), GetScaledHeight(), m_flags);

	if (m_texture)
	if (((unsigned int)m_texture->GetTextureWidth() != GetScaledWidth()) || ((unsigned int)m_texture->GetTextureHeight() != GetScaledHeight())
		|| (m_texture->IsTransparent() != m_isTransparent))
	{
		m_texture->MarkDeleted();
		unsigned int textureID = m_fbo->CreateColorTexture(m_isTransparent ? GL_RGBA : GL_RGB, m_isTransparent ? GL_RGBA : GL_RGB8, true);
		m_texture->SetNumber(textureID);
		m_texture->SetTextureWidth(GetScaledWidth());
		m_texture->SetTextureHeight(GetScaledHeight());
		m_texture->SetTransparentStatus(m_isTransparent);
		m_texture->UnmarkDeleted();
		m_texture->UpdateCurrentFiltration(GL_LINEAR, GL_LINEAR);
	}

	unsigned char* a = NULL;

	m_fbo->Create(true);
	if (m_fbo->GetType() == RENDER_TARGET_MAINBUFFER)
	{
		m_fbo->SetOk(true);

		g->tm.BindTexture(m_texture->GetID());				
		a = GetClearedMemory(m_clearColorR, m_clearColorG, m_clearColorB, m_clearColorA);
				
		g->tm.BindTexture(m_texture->GetID());
		GLFUNC(glTexImage2D)(GL_TEXTURE_2D, 0, GL_RGBA, GetScaledWidth(), GetScaledHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, a);		
		m_texture->SetCurrentFiltration(GL_LINEAR, GL_LINEAR, 1, 0);
		g->tm.RefreshTextureOptimization();
	}
	else if (m_texture)
	{
		m_fbo->Bind();
		m_fbo->AttachColorTexture(m_texture->GetNumber());
		m_fbo->Unbind();
	}

	if (m_backBufferTextureID != 0xFFFFFFFF)
	{
		GLFUNC(glDeleteTextures)(1, &m_backBufferTextureID);
	}
	
	m_backBufferTextureID = CFrameBuffer::CreateColorTexture(GL_TEXTURE_2D, GetScaledWidth(), GetScaledHeight(), m_isTransparent ? GL_RGBA : GL_RGB, m_isTransparent ? GL_RGBA : GL_RGB8, true);

	g->tm.RefreshTextureOptimization();

	m_isOk = m_texture ? m_fbo->IsOk() : true;

	m_isCreated = true;
	if (m_isOk)
	{
		if (m_fbo->GetType() != RENDER_TARGET_MAINBUFFER)
		{
			Clear();
			SaveBackBuffer(-1);
		}
		else
		{
			GLFUNC(glBindTexture)(GL_TEXTURE_2D, m_backBufferTextureID);			
			GLFUNC(glTexImage2D)(GL_TEXTURE_2D, 0, GL_RGBA, GetScaledWidth(), GetScaledHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, a);		
			g->tm.RefreshTextureOptimization();
		}
	}

	return m_isOk;
}

bool CSimpleDrawingTarget::IsCreatedNow()
{
	return m_isCreated;
}

void CSimpleDrawingTarget::Clear()
{
	if (!m_isCreated)
	{
		return;
	}

	if ((!m_fbo) || (!m_isOk))
	{
		return;
	}

	if (m_fbo->GetType() != RENDER_TARGET_MAINBUFFER)
	{
		m_fbo->Bind();
		GLfloat a[4];
		GLFUNC(glGetFloatv)(GL_COLOR_CLEAR_VALUE, &a[0]);
		GLFUNC(glClearColor)((float)m_clearColorR / 255.0f, (float)m_clearColorG / 255.0f, 
				 (float)m_clearColorB / 255.0f, (float)m_clearColorA / 255.0f);
		GLFUNC(glClear)(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GLFUNC(glClearColor)(a[0], a[1], a[2], a[3]);
		m_fbo->Unbind();
	}
	else
	{
		unsigned char* a = GetClearedMemory(m_clearColorR, m_clearColorG, m_clearColorB, m_clearColorA);
		g->tm.BindTexture(m_texture->GetID());
        GLFUNC(glTexSubImage2D)(GL_TEXTURE_2D, 0, 0, 0, GetScaledWidth(), GetScaledHeight(), GL_RGBA, GL_UNSIGNED_BYTE, a);				
	}
}

unsigned int CSimpleDrawingTarget::CreateEmptySmallTexture()
{
	const unsigned int defaultSize = 4;
	const unsigned int allSize = defaultSize * defaultSize * 4;
	unsigned char transparentTexture[allSize];
	memset(&transparentTexture[0], 0, allSize);
	for (int i = 3; i < allSize; i += 4)
	{
		transparentTexture[i] = 1;
	}

	unsigned int textureID;

	GLFUNC(glGenTextures)(1, &textureID);
	GLFUNC(glBindTexture)(GL_TEXTURE_2D, textureID);
	GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	GLFUNC(glTexImage2D)(GL_TEXTURE_2D, 0, GL_RGBA, defaultSize, defaultSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, &transparentTexture[0]);
	GLFUNC(glBindTexture)(GL_TEXTURE_2D, 0);

	return textureID;
}

unsigned int CSimpleDrawingTarget::CreateTempTexture()
{
	return CFrameBuffer::CreateColorTexture(GL_TEXTURE_2D, GetScaledWidth(), GetScaledHeight(), m_isTransparent ? GL_RGBA : GL_RGB, m_isTransparent ? GL_RGBA : GL_RGB8, true);
}

unsigned int CSimpleDrawingTarget::GetScaledWidth()
{
	return (int)(m_width / m_invScale);
}

unsigned int CSimpleDrawingTarget::GetScaledHeight()
{
	return (int)(m_height / m_invScale);
}

void CSimpleDrawingTarget::ApplyFilter(CFilter* filter)
{
	Create();

	if ((!m_fbo) || (!m_isOk))
	{
		return;
	}

	m_fbo->Bind();
	g->stp.PushMatrix();
	g->stp.SetViewport(GetScaledWidth(), GetScaledHeight());	
	g->cm.SetCamera2d();
	g->stp.SetState(ST_ZTEST, false);
	g->stp.SetState(ST_CULL_FACE, false);
	g->stp.SetColor(255, 255, 255);
	g->stp.PrepareForRender();

	filter->Bind(m_width, m_height);

	GLFUNC(glBegin)(GL_QUADS);
	GLFUNC(glTexCoord2f)(0.0f, 0.0f);
	GLFUNC(glVertex2f)(-1.0f, -1.0f);
	GLFUNC(glTexCoord2f)(0.0f, 1.0f);
	GLFUNC(glVertex2f)(-1.0f, 1.0f);
	GLFUNC(glTexCoord2f)(1.0f, 1.0f);
	GLFUNC(glVertex2f)(1.0f, 1.0f);
	GLFUNC(glTexCoord2f)(1.0f, 0.0f);
	GLFUNC(glVertex2f)(1.0f, -1.0f);
	GLFUNC(glEnd)();

	filter->Unbind();

	g->stp.SetState(ST_ZTEST, true);
	g->stp.SetState(ST_CULL_FACE, true);
	g->stp.RestoreViewport();
	g->stp.PopMatrix();
	m_fbo->Unbind();

	g->tm.RefreshTextureOptimization();
}

void CSimpleDrawingTarget::SaveBackBuffer(int textureID)
{
	Create();

	if ((!m_fbo) || (!m_isOk))
	{
		return;
	}

	textureID = (textureID < 0) ? m_backBufferTextureID : textureID;

	switch (m_fbo->GetType())
	{
	case RENDER_TARGET_MAINBUFFER:
		{
			unsigned char* a = GetMemory();
			GLFUNC(glBindTexture)(GL_TEXTURE_2D, m_texture->GetNumber());
			GLFUNC(glGetTexImage)(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, a);
			GLFUNC(glBindTexture)(GL_TEXTURE_2D, textureID);		
			GLFUNC(glTexSubImage2D)(GL_TEXTURE_2D, 0, 0, 0, GetScaledWidth(), GetScaledHeight(), GL_RGBA, GL_UNSIGNED_BYTE, a);			
		}
		break;
	case RENDER_TARGET_PBUFFER:
		m_fbo->Bind();
		GLFUNC(glBindTexture)(GL_TEXTURE_2D, textureID);
		GLFUNC(glCopyTexSubImage2D)(GL_TEXTURE_2D, 0, 0, 0, 0, 0, GetScaledWidth(), GetScaledHeight());
		m_fbo->Unbind();
		break;
	case RENDER_TARGET_FBO:
		m_fbo->Bind();
		m_fbo->AttachColorTexture(textureID);
		g->stp.PushMatrix();
		g->stp.SetViewport(GetScaledWidth(), GetScaledHeight());	
		g->cm.SetCamera2d();
		GLFUNC(glBindTexture)(GL_TEXTURE_2D, m_texture->GetNumber());
		GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		g->stp.SetState(ST_ZTEST, false);
		g->stp.SetState(ST_CULL_FACE, false);
		g->stp.SetColor(255, 255, 255);
		g->stp.SetState(ST_BLEND, true);
		g->stp.SetBlendFunc(GL_ONE, GL_ONE);
		g->stp.PrepareForRender();

		GLfloat a[4];
		GLFUNC(glGetFloatv)(GL_COLOR_CLEAR_VALUE, &a[0]);
		GLFUNC(glClearColor)(0.0f, 0.0f, 0.0f, 0.0f);
		GLFUNC(glClear)(GL_COLOR_BUFFER_BIT);
		GLFUNC(glClearColor)(a[0], a[1], a[2], a[3]);
		
		GLFUNC(glBegin)(GL_QUADS);
		GLFUNC(glTexCoord2f)(0.0f, 0.0f);
		GLFUNC(glVertex3f)(-1.0f, -1.0f, 0);
		GLFUNC(glTexCoord2f)(0.0f, 1.0f);
		GLFUNC(glVertex3f)(-1.0f, 1.0f, 0);
		GLFUNC(glTexCoord2f)(1.0f, 1.0f);
		GLFUNC(glVertex3f)(1.0f, 1.0f, 0);
		GLFUNC(glTexCoord2f)(1.0f, 0.0f);
		GLFUNC(glVertex3f)(1.0f, -1.0f, 0);
		GLFUNC(glEnd)();
		g->stp.SetState(ST_ZTEST, true);
		g->stp.SetState(ST_CULL_FACE, true);
		g->stp.RestoreViewport();
		g->stp.PopMatrix();
		GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		m_fbo->AttachColorTexture(m_texture->GetNumber());
		m_fbo->Unbind();
		break;
	}
	
	g->tm.RefreshTextureOptimization();
}

void CSimpleDrawingTarget::RestoreBackBuffer(int textureID)
{
	Create();

	if ((!m_fbo) || (!m_isOk))
	{
		return;
	}

	textureID = (textureID < 0) ? m_backBufferTextureID : textureID;

	if (m_fbo->GetType() != RENDER_TARGET_MAINBUFFER)
	{
	m_fbo->Bind();

		GLfloat a[4];
		GLFUNC(glGetFloatv)(GL_COLOR_CLEAR_VALUE, &a[0]);
		GLFUNC(glClearColor)(0.0f, 0.0f, 0.0f, 0.0f);
		GLFUNC(glClear)(GL_COLOR_BUFFER_BIT);
		GLFUNC(glClearColor)(a[0], a[1], a[2], a[3]);
	
		g->stp.SetViewport(GetScaledWidth(), GetScaledHeight());	
		g->cm.SetCamera2d();
		GLFUNC(glBindTexture)(GL_TEXTURE_2D, textureID);
		g->stp.SetState(ST_ZTEST, false);
		g->stp.SetState(ST_CULL_FACE, false);
		g->stp.SetColor(255, 255, 255);
		g->stp.SetState(ST_BLEND, true);
		g->stp.SetBlendFunc(GL_ONE, GL_ONE);
		g->stp.PrepareForRender();
		GLFUNC(glBegin)(GL_QUADS);
		GLFUNC(glTexCoord2f)(0.0f, 0.0f);
		GLFUNC(glVertex3f)(-1.0f, -1.0f, 0);
		GLFUNC(glTexCoord2f)(0.0f, 1.0f);
		GLFUNC(glVertex3f)(-1.0f, 1.0f, 0);
		GLFUNC(glTexCoord2f)(1.0f, 1.0f);
		GLFUNC(glVertex3f)(1.0f, 1.0f, 0);
		GLFUNC(glTexCoord2f)(1.0f, 0.0f);
		GLFUNC(glVertex3f)(1.0f, -1.0f, 0);
		GLFUNC(glEnd)();
		g->stp.SetState(ST_ZTEST, true);
		g->stp.SetState(ST_CULL_FACE, true);
		g->stp.SetState(ST_BLEND, false);
		g->stp.RestoreViewport();
		m_fbo->Unbind();

		g->tm.RefreshTextureOptimization();
	}
	else
	{
		unsigned char* a = GetMemory();
		GLFUNC(glBindTexture)(GL_TEXTURE_2D, textureID);		
		GLFUNC(glGetTexImage)(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, a);
		GLFUNC(glBindTexture)(GL_TEXTURE_2D, m_texture->GetNumber());
		GLFUNC(glTexSubImage2D)(GL_TEXTURE_2D, 0, 0, 0, GetScaledWidth(), GetScaledHeight(), GL_RGBA, GL_UNSIGNED_BYTE, a);				
	}
}

void CSimpleDrawingTarget::BindToApplyGeometry()
{
	Create();

	if ((!m_fbo) || (!m_isOk))
	{
		return;
	}

	if (m_fbo->GetType() != RENDER_TARGET_MAINBUFFER)
	{
		m_fbo->Bind();
		m_fbo->AttachColorTexture(m_texture->GetNumber());
	}
	else
	{
		g->ne.EndFrame();
		GLfloat a[4];
		GLFUNC(glGetFloatv)(GL_COLOR_CLEAR_VALUE, &a[0]);
		GLFUNC(glClearColor)(0.0f, 0.0f, 0.0f, 0.0f);
		GLFUNC(glClear)(GL_COLOR_BUFFER_BIT);
		GLFUNC(glClearColor)(a[0], a[1], a[2], a[3]);
	}

	g->stp.SetViewport(GetScaledWidth(), GetScaledHeight());	
	GLFUNC(glBindTexture)(GL_TEXTURE_2D, 0);

	g->stp.PushMatrix();
	g->cm.SetCamera2d();
	g->stp.SetState(ST_ZTEST, false);
    
	m_isBinded = true;

	if (m_fbo->GetType() == RENDER_TARGET_MAINBUFFER)
	{
		GLFUNC(glBindTexture)(GL_TEXTURE_2D, m_texture->GetNumber());
		g->stp.SetState(ST_CULL_FACE, false);
		g->stp.SetColor(255, 255, 255);
		g->stp.SetState(ST_BLEND, true);
		g->stp.SetBlendFunc(GL_ONE, GL_ONE);
		g->stp.PrepareForRender();
		GLFUNC(glBegin)(GL_QUADS);
		GLFUNC(glTexCoord2f)(0.0f, 0.0f);
		GLFUNC(glVertex3f)(-1.0f, -1.0f, 0);
		GLFUNC(glTexCoord2f)(0.0f, 1.0f);
		GLFUNC(glVertex3f)(-1.0f, 1.0f, 0);
		GLFUNC(glTexCoord2f)(1.0f, 1.0f);
		GLFUNC(glVertex3f)(1.0f, 1.0f, 0);
		GLFUNC(glTexCoord2f)(1.0f, 0.0f);
		GLFUNC(glVertex3f)(1.0f, -1.0f, 0);
		GLFUNC(glEnd)();
		GLFUNC(glBindTexture)(GL_TEXTURE_2D, 0);
	}
}

bool CSimpleDrawingTarget::IsBindedToApplyGeometry()const
{
	return m_isBinded;
}

void CSimpleDrawingTarget::UnbindToApplyGeometry()
{
	if (!m_isBinded)
	{
		return;
	}

	Create();

	if ((!m_fbo) || (!m_isOk))
	{
		return;
	}

	g->stp.SetState(ST_ZTEST, true);
	g->stp.PopMatrix();			

	GLFUNC(glBindTexture)(GL_TEXTURE_2D, 0);
	g->stp.RestoreViewport();

	if (m_fbo->GetType() != RENDER_TARGET_MAINBUFFER)
	{
		m_fbo->Unbind();
	}
	else
	{
		GLFUNC(glBindTexture)(GL_TEXTURE_2D, m_texture->GetNumber());
		GLFUNC(glCopyTexSubImage2D)(GL_TEXTURE_2D, 0, 0, 0, 0, 0, GetScaledWidth(), GetScaledHeight());		
		GLFUNC(glBindTexture)(GL_TEXTURE_2D, 0);
	}

	g->tm.RefreshTextureOptimization();

	m_isBinded = false;
}

void CSimpleDrawingTarget::SetName(char* name)
{
	m_name = name;
}

unsigned char CSimpleDrawingTarget::GetType()
{
	if (m_fbo)
	{
		return m_fbo->GetType();
	}
	else
	{
		return RENDER_TARGET_MAINBUFFER;
	}
}

void CSimpleDrawingTarget::ApplyGeometry(void (__cdecl *callbackfunc)(void))
{
	Create();

	if ((!m_fbo) || (!m_isOk))
	{
		return;
	}

	if (m_fbo->GetType() != RENDER_TARGET_MAINBUFFER)
	{
		m_fbo->Bind();
	}
	else
	{
		g->ne.EndFrame();
		GLfloat a[4];
		GLFUNC(glGetFloatv)(GL_COLOR_CLEAR_VALUE, &a[0]);
		GLFUNC(glClearColor)(0.0f, 0.0f, 0.0f, 0.0f);
		GLFUNC(glClear)(GL_COLOR_BUFFER_BIT);
		GLFUNC(glClearColor)(a[0], a[1], a[2], a[3]);
	}
	g->stp.SetViewport(GetScaledWidth(), GetScaledHeight());	
	GLFUNC(glBindTexture)(GL_TEXTURE_2D, 0);

	g->stp.PushMatrix();
	g->cm.SetCamera2d();
	g->stp.SetState(ST_ZTEST, false);

	if (m_fbo->GetType() == RENDER_TARGET_MAINBUFFER)
	{
		GLFUNC(glBindTexture)(GL_TEXTURE_2D, m_texture->GetNumber());
		g->stp.SetState(ST_CULL_FACE, false);
		g->stp.SetColor(255, 255, 255, 255);
		g->stp.SetState(ST_BLEND, true);
		g->stp.SetBlendFunc(GL_ONE, GL_ONE);
		g->stp.PrepareForRender();
		GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);		
		GLFUNC(glBegin)(GL_QUADS);
		GLFUNC(glTexCoord2f)(0.0f, 0.0f);
		GLFUNC(glVertex3f)(-1.0f, -1.0f, 0);
		GLFUNC(glTexCoord2f)(0.0f, 1.0f);
		GLFUNC(glVertex3f)(-1.0f, 1.0f, 0);
		GLFUNC(glTexCoord2f)(1.0f, 1.0f);
		GLFUNC(glVertex3f)(1.0f, 1.0f, 0);
		GLFUNC(glTexCoord2f)(1.0f, 0.0f);
		GLFUNC(glVertex3f)(1.0f, -1.0f, 0);
		GLFUNC(glEnd)();
		GLFUNC(glBindTexture)(GL_TEXTURE_2D, 0);
	}

	callbackfunc();

	if (m_fbo->GetType() == RENDER_TARGET_MAINBUFFER)
	{
		GLFUNC(glBindTexture)(GL_TEXTURE_2D, m_texture->GetNumber());
		GLFUNC(glCopyTexSubImage2D)(GL_TEXTURE_2D, 0, 0, 0, 0, 0, GetScaledWidth(), GetScaledHeight());				
		GLFUNC(glBindTexture)(GL_TEXTURE_2D, 0);		
	}

	g->stp.SetState(ST_ZTEST, true);
	g->stp.PopMatrix();			

	GLFUNC(glBindTexture)(GL_TEXTURE_2D, 0);
	g->stp.RestoreViewport();

	if (m_fbo->GetType() != RENDER_TARGET_MAINBUFFER)
	{
		m_fbo->Unbind();
	}
	else
	{
	}

	g->tm.RefreshTextureOptimization();
}

float CSimpleDrawingTarget::GetGLX(int x)
{
	return ((float)x / (float)m_width * 2.0f - 1.0f);
}

float CSimpleDrawingTarget::GetGLY(int y)
{
	return ((float)y / (float)m_height * 2.0f - 1.0f);
}

void CSimpleDrawingTarget::Destroy()
{
	MP_DELETE_THIS;
}

bool CSimpleDrawingTarget::IsOk()
{
	return m_isOk;
}

void CSimpleDrawingTarget::FreeResourcesIfNeeded()
{
	if (m_backBufferTextureID != 0xFFFFFFFF)
	{
		GLFUNC(glDeleteTextures)(1, &m_backBufferTextureID);
		m_backBufferTextureID = 0xFFFFFFFF;
	}

	g->tm.BindTexture(m_texture->GetID());
	GLFUNC(glTexImage2D)(GL_TEXTURE_2D, 0, GL_RGBA, 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	g->tm.RefreshTextureOptimization();

	m_isCreated = false;
}

CSimpleDrawingTarget::~CSimpleDrawingTarget()
{
	if (m_fbo)
	{
		MP_DELETE(m_fbo);
	}

	g->tm.DeleteObject(m_texture->GetID());

	if (m_backBufferTextureID != 0xFFFFFFFF)
	{
		GLFUNC(glDeleteTextures)(1, &m_backBufferTextureID);
	}
}
