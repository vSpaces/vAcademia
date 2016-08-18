
#include "StdAfx.h"
#include "TinyXML.h"
#include "HelperFunctions.h"
#include "SimplePostEffect.h"
#include "GlobalSingletonStorage.h"

CSimplePostEffect::CSimplePostEffect():
	m_buffer(NULL),
	m_textureWidth(0),
	m_textureHeight(0)
{
	GetTextureSize(&m_textureWidth, &m_textureHeight);
	
	InitBuffers();
}

void CSimplePostEffect::PrepareForReading()
{
	// do nothing
}

void CSimplePostEffect::EndReading()
{
	// do nothing
}

void CSimplePostEffect::OnSceneDrawStart(CViewport* /*viewport*/)
{
	if (IsTextureSizeChanged())
	{
		InitBuffers();
	}

	m_buffer->Bind();
	g->stp.SetViewport(g->stp.GetCurrentWidth(), g->stp.GetCurrentHeight());
	GLFUNC(glClear)(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void CSimplePostEffect::OnSceneDrawEnd(CViewport* /*viewport*/, CRenderTarget* /*renderTarget*/)
{
	m_buffer->Unbind();
	
	g->stp.SetState(ST_ZTEST, false);
	g->stp.SetState(ST_CULL_FACE, false);

	g->stp.PushMatrix();
	g->cm.SetCamera2d();

	g->stp.PushMatrix();
	g->stp.SetColor(255, 255, 255);
	g->stp.SetState(ST_BLEND, false);
	g->cm.SetCamera2d();
	g->stp.SetState(ST_ZWRITE, false);
#pragma warning(push)
#pragma warning(disable : 4239)
	g->sp.PutNormalSprite(0, -(int)(m_textureHeight - g->stp.GetCurrentHeight()), 0, 0, m_textureWidth, m_textureHeight, m_bufferTexID, CColor3(255, 255, 255));
#pragma warning(pop)
	g->stp.SetState(ST_ZWRITE, true);
	g->stp.PopMatrix();

	g->stp.PopMatrix();
}

void CSimplePostEffect::GetTextureSize(int* textureWidth, int* textureHeight)
{
	int windowWidth = g->stp.GetCurrentWidth();
	int windowHeight = g->stp.GetCurrentHeight();
	
	int width = 2;
	int height = 2;
	while (width < windowWidth)
	{
		width <<= 1;
	}
	*textureWidth = width;

	while (height < windowHeight)
	{
		height <<= 1;
	}
	*textureHeight = height;
}

bool CSimplePostEffect::IsTextureSizeChanged()
{
	int newTextureWidth = 0;
	int newTextureHeight = 0;

	GetTextureSize(&newTextureWidth, &newTextureHeight);
	
	if ((newTextureWidth != m_textureWidth) || (newTextureHeight != m_textureHeight))
	{
		m_textureWidth = newTextureWidth;
		m_textureHeight = newTextureHeight;

		return true;
	}

	return false;
}

void CSimplePostEffect::InitBuffers()
{
	if (m_buffer != NULL)
	{
		MP_DELETE(m_buffer);
	}

	MP_NEW_V3(m_buffer, CFrameBuffer, m_textureWidth, m_textureHeight, CFrameBuffer::depth24_stencil8);

	unsigned int screenMap = m_buffer->CreateColorTexture(GL_RGBA, GL_RGBA, true);
	m_bufferTexID = g->tm.ManageGLTexture("simple_posteffect", screenMap, m_textureWidth, m_textureHeight);
    CTexture* texture = g->tm.GetObjectPointer(m_bufferTexID);
	texture->UpdateCurrentFiltration(GL_LINEAR, GL_LINEAR);

	/*bool isCreated = */m_buffer->Create(true);
	m_buffer->Bind();
	m_buffer->AttachColorTexture (screenMap);
	m_buffer->Unbind();
}

void CSimplePostEffect::OnLoadFromXML(std::wstring& /*fileName*/)
{

}

void CSimplePostEffect::SaveScreen(std::wstring fileName, SCropRect* const rect)
{
	g->tm.SaveTexture(m_bufferTexID, fileName, rect);
}

unsigned char CSimplePostEffect::GetMultisamplePower()const
{
	return 0;
}

CSimplePostEffect::~CSimplePostEffect()
{
	if (m_buffer != NULL)
	{
		MP_DELETE(m_buffer);
	}
}