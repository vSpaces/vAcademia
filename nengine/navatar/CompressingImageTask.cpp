
#include "StdAfx.h"
#include "CompressingImageTask.h"
#include "DXTCompressor.h"
#include "GlobalSingletonStorage.h"

CCompressingImageTask::CCompressingImageTask():
	m_textureData(NULL),
	m_data(NULL),
	m_texture(NULL),
	m_size(0),
	m_isCompressed(false),
	m_isTextureRGBA(false),
	MP_STRING_INIT(m_cacheFileName)
{
}

void CCompressingImageTask::SetParams(unsigned char* textureData, CTexture* texture, const std::string& cacheFileName, bool isTextureRGBA)
{
	m_textureData = textureData;
	m_texture = texture;
	m_cacheFileName = cacheFileName;
	m_isTextureRGBA = isTextureRGBA;
}

void CCompressingImageTask::Start()
{
	if (!m_textureData || !m_texture)
	{
		return;
	}

	m_isCompressed = CompressImage(m_textureData, (void **)&m_data, (unsigned short)m_texture->GetTextureWidth(), (unsigned short)m_texture->GetTextureHeight(),m_isTextureRGBA ? -3 : 3, true, false, m_size);

	MP_DELETE_ARR(m_textureData);
	m_textureData = NULL;	
}

void CCompressingImageTask::Destroy()
{
	if (m_data)
	{
		MP_DELETE_ARR(m_data);
		m_data = NULL;
	}
}

void CCompressingImageTask::PrintInfo()
{
	g->lw.WriteLn("CompressingImageTask ", m_cacheFileName);
}

bool CCompressingImageTask::IsCompressed()const
{
	return m_isCompressed;
}

unsigned int CCompressingImageTask::GetSize()
{
	return m_size;
}

CTexture* CCompressingImageTask::GetTexture()
{
	return m_texture;
}

unsigned char* CCompressingImageTask::GetData()
{
	return m_data;
}

std::string CCompressingImageTask::GetCacheFileName()
{
	return m_cacheFileName;
}

void CCompressingImageTask::SelfDestroy()
{
	MP_DELETE_THIS;
}

CCompressingImageTask::~CCompressingImageTask()
{
	if (m_textureData)
	{
		MP_DELETE_ARR(m_textureData);		
	}
}