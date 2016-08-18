
#include "StdAfx.h"
#include "DXTCompressor.h"
#include "TextureManager.h"
#include "LoadingTexturesTask.h"
#include "AddNCacheDataTask.h"
#include "GlobalSingletonStorage.h"
#include "cal3d/memory_leak.h"

_SLoadingTexture::_SLoadingTexture():
	MP_WSTRING_INIT(path),
	MP_STRING_INIT(cacheFileName)
{
	textureLoader = NULL;
	texture = NULL;
	size = 0;
	data = NULL;
	path = L"";
	isMipmap = false;
	isMaskNeeded = false;
	isCompressedExists = false;
	cacheFileName = "";

	textureWidth = 0;
	textureHeight = 0;
	realTextureWidth = 0;
	realTextureHeight = 0;
}

_SLoadingTexture::~_SLoadingTexture()
{
	if (data != NULL)
		MP_DELETE_ARR(data);
	data = NULL;
}

CLoadingTexturesTask::CLoadingTexturesTask():
	m_loadingTexture(NULL),
	m_isLoaded(false),
	m_loaderCS(NULL),
	m_siveDiv(1)
{
	m_textureLoader = MP_NEW(CTextureLoader);
	OnDone();
	m_loaderCS = MP_NEW(CRITICAL_SECTION);
	InitializeCriticalSection(m_loaderCS);
}

void CLoadingTexturesTask::SetCriticalSection(CRITICAL_SECTION* cs)
{
	m_loaderCS = cs;
}

void CLoadingTexturesTask::PrintInfo()
{
	g->lw.WriteLn("CLoadingTexturesTask ", m_loadingTexture->path, " ", m_loadingTexture->texture->GetName());
}

void CLoadingTexturesTask::SetTexture(SLoadingTexture* loadingTexture, int sizeDiv)
{
	m_loadingTexture = loadingTexture;
	m_siveDiv = sizeDiv;
	m_isLoaded = false;

	if (m_loadingTexture->texture->GetMaxDecreaseKoef() < m_siveDiv)
	{
		m_siveDiv = m_loadingTexture->texture->GetMaxDecreaseKoef();
	}
}

SLoadingTexture* CLoadingTexturesTask::GetTexture()
{
	return m_loadingTexture;
}

void CLoadingTexturesTask::Start()
{
	if ((!m_loadingTexture) || (!m_loadingTexture->texture) || (!m_loadingTexture->texture->GetLoadingInfo()))
	{
		m_isLoaded = false;
		return;
	}

	m_loadingTexture->textureLoader = m_textureLoader;
	if ((m_loadingTexture->size < 256) && (m_loadingTexture->path.find(L".jpg") != -1))
	{
		// Не будем пытаться, FreeImage падает
		m_isLoaded = false;
	}
	else
	{
		m_isLoaded = m_loadingTexture->textureLoader->LoadTexture(m_loadingTexture->path, m_loadingTexture->data, m_loadingTexture->size);
	}

	if ((m_isLoaded) && (m_loadingTexture->textureLoader->GetFrameCount() == 1))
	{
		m_loadingTexture->realTextureWidth = m_loadingTexture->textureLoader->GetWidth();
		m_loadingTexture->realTextureHeight = m_loadingTexture->textureLoader->GetHeight();		
		m_loadingTexture->textureWidth = m_loadingTexture->textureLoader->GetWidth();
		m_loadingTexture->textureHeight = m_loadingTexture->textureLoader->GetHeight();		

		if (((!g->gi.IsNPOTSupport()) || ((m_loadingTexture->texture->GetLoadingInfo()->CanBeCompressed(4, 1)) && (g->gi.GetVendorID() == VENDOR_ATI))) && (!m_loadingTexture->isMipmap))
		{
			m_loadingTexture->textureLoader->EnlargeTexture();	
			m_loadingTexture->realTextureWidth = m_loadingTexture->textureLoader->GetWidth();
			m_loadingTexture->realTextureHeight = m_loadingTexture->textureLoader->GetHeight();		
		}
		else if ((!g->gi.IsNPOTSupport()) && (m_loadingTexture->isMipmap))
		{
			if (m_loadingTexture->textureLoader->ToPOT())
			if (m_siveDiv != 1)
			{
				m_siveDiv /= 2;
			}
		}

		if ((m_loadingTexture->textureLoader->GetWidth() > 8) && (m_loadingTexture->textureLoader->GetHeight() > 8))
		if ((m_loadingTexture->isMipmap) && (m_siveDiv != 1))
		{
			m_loadingTexture->textureLoader->Resize(m_loadingTexture->textureLoader->GetWidth() / m_siveDiv, m_loadingTexture->textureLoader->GetHeight() / m_siveDiv);
			m_loadingTexture->textureWidth /= m_siveDiv;
			m_loadingTexture->textureHeight /= m_siveDiv;
		}

		if (m_loadingTexture->texture->GetLoadingInfo()->CanBeCompressed((unsigned char)m_loadingTexture->textureLoader->GetChannelCount(), (unsigned char)m_loadingTexture->textureLoader->GetFrameCount()))
		{
			unsigned char* data = NULL;
			unsigned int size = 0;

			if (CompressImage(m_loadingTexture->textureLoader->GetData(0), (void **)&data, (unsigned short)m_loadingTexture->textureWidth, (unsigned short)m_loadingTexture->textureHeight,
				m_loadingTexture->textureLoader->GetChannelCount(), m_loadingTexture->textureLoader->IsDirectChannelOrder(), true, size))
			{
				g->dc.AddData(data, size, m_loadingTexture->cacheFileName);

				MP_DELETE_ARR(data);
				if (m_loadingTexture->data)
				{
					MP_DELETE_ARR(m_loadingTexture->data);
				}
				
				m_loadingTexture->textureLoader =	NULL;
				m_loadingTexture->size = 0;
				m_loadingTexture->data = NULL;
				m_loadingTexture->isCompressedExists = true;
			}
			else
			{
				if (m_loadingTexture->data)
				{
					MP_DELETE_ARR(m_loadingTexture->data);
				}
			}
		}
	}
}

bool CLoadingTexturesTask::IsLoaded()const
{
	return m_isLoaded;
}

void CLoadingTexturesTask::OnLoaded()
{
	m_isLoaded = false;
}

void CLoadingTexturesTask::SelfDestroy()
{
	MP_DELETE_THIS;
}

CLoadingTexturesTask::~CLoadingTexturesTask()
{
	MP_DELETE(m_textureLoader);
	DeleteCriticalSection(m_loaderCS);
	MP_DELETE(m_loaderCS);
}