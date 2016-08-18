
#include "stdafx.h"
#include "texture.h"
#include "GlobalSingletonStorage.h"
#include "UserData.h"
#include "__resource_log.h"
#include "MemoryProfiler.h"

//#define SUPER_SCREENSHOT_MODE		1

CTexture::CTexture():
	m_loadedStatus(TEXTURE_NOT_LOADED_YET),
	m_keyColorType(KEY_COLOR_NOT_USED),
	m_isNeedToCheckVisibility(false),
	m_isRawDirectChannelOrder(true),	
	m_isKeepInSystemMemory(false),
	m_dynamicTexture(NULL),
	m_occlusionQuery(NULL),
	m_isTransparent(false),
	MP_WSTRING_INIT(m_path),
	m_hasNoMipmaps(false),
	m_isReallyUsed(false),
	m_maxDecreaseKoef(4),
	m_passFrameCount(0),	
	m_oglTextureID(-1),
	m_isXInvert(false),
	m_isEnabled(false),
	m_isYInvert(true),
	m_isBinded(false),
	m_rawDataSize(0),
	m_realHeight(0),
	m_anisotropy(0),
	m_rawData(NULL),
	m_minFilter(0),
	m_magFilter(0),
	m_realWidth(0),
	m_maskSize(0),
	m_mask(NULL),
	m_height(0),
	m_width(0),
	m_alpha(255),
	m_type(0),	
	m_r(255),
	m_g(255),
	m_b(255),
	m_mipmapBias(0),
	m_isNeedPreMultiply(false),
	m_isMipmapsObligatory(false)
{
	m_oglTextureID = g->tm.GetErrorTextureID();
}

void CTexture::AddOwner(void* owner)
{
	std::vector<void *>::iterator it = m_owners.begin();
	std::vector<void *>::iterator itEnd = m_owners.end();

	for ( ; it != itEnd; it++)
	if (*it == owner)
	{
		return;
	}

	m_owners.push_back(owner);
}

std::vector<void *>* CTexture::GetOwnersPtr()
{
	return &m_owners;
}

void CTexture::DeleteOwner(void* owner)
{
	std::vector<void *>::iterator it = m_owners.begin();
	std::vector<void *>::iterator itEnd = m_owners.end();

	for ( ; it != itEnd;)
	{
		if (*it == owner)
		{
			it = m_owners.erase(it);
			itEnd = m_owners.end();			
		}
		else
			it++;
	}
}

void CTexture::SetObligatoryMipmaps(bool isEnabled)
{
	m_isMipmapsObligatory = isEnabled;
}

void CTexture::QuerySizeFromOpenGL(int& width, int& height)
{
	glBindTexture(GL_TEXTURE_2D, m_oglTextureID);

	GLFUNC(glGetTexLevelParameteriv)(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	GLFUNC(glGetTexLevelParameteriv)(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

	g->tm.RefreshTextureOptimization();
}

void CTexture::SetNeedToCheckVisibility(bool isNeeded)
{
	m_isNeedToCheckVisibility = isNeeded;
}

void CTexture::SetMaxDecreaseKoef(const unsigned char koef)
{
	if ((1 == koef) || (2 == koef) || (4 == koef))
	{
		m_maxDecreaseKoef = koef;
	}
}

unsigned char CTexture::GetMaxDecreaseKoef()const
{
	return m_maxDecreaseKoef;	
}

void CTexture::UpdateCurrentFiltration(const int minFilter, const int magFilter)
{	
	m_minFilter = minFilter;
	m_magFilter = magFilter;	

	if (m_isMipmapsObligatory)
	if ((m_minFilter != GL_LINEAR_MIPMAP_LINEAR) || (m_magFilter != GL_LINEAR))
	{
		SetCurrentFiltration(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, 1, 0);
	}
}

void CTexture::SetCurrentFiltration(int minFilter, int magFilter, const int anisotropy, const float mipmapBias)
{
	if ((m_hasNoMipmaps) && ((minFilter != GL_LINEAR) && (minFilter != GL_NEAREST)))
	{
		minFilter = GL_LINEAR;
		magFilter = GL_LINEAR;
	}
	else if (m_isMipmapsObligatory)
	{
		m_minFilter = GL_LINEAR_MIPMAP_LINEAR;
		m_magFilter = GL_LINEAR;
	}

	/*GLint a;
	//glGetIntegerv(GL_TEXTURE_BINDING_2D, &a);
	if (a != m_oglTextureID)
	{
		assert(false);
	}*/
	
	if (minFilter != m_minFilter)
	{
		m_minFilter = minFilter;		
		GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_minFilter);
	}

	if (magFilter != m_magFilter)
	{
		m_magFilter = magFilter;
		GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_magFilter);		
	}

	static bool isAnisotropySupported = g->ei.IsExtensionSupported(GLEXT_ext_texture_filter_anisotropic);

	if (isAnisotropySupported)
	if (anisotropy != m_anisotropy)
	{
		m_anisotropy = anisotropy;
		if (m_anisotropy < 1)
		{
			m_anisotropy = 1;
		}
#ifndef SUPER_SCREENSHOT_MODE
		GLFUNC(glTexParameterf)(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (GLfloat)m_anisotropy);
#endif
	}

#ifdef SUPER_SCREENSHOT_MODE
	GLFUNC(glTexParameterf)(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
#endif

#ifndef SUPER_SCREENSHOT_MODE
	if (mipmapBias != m_mipmapBias)
	{
		m_mipmapBias = mipmapBias;	
		g->tm.SetCurrentMipMapBias(m_mipmapBias);
	}
#else
	m_mipmapBias = 2.5f;
#endif
}

void CTexture::StartOcclusionQuery()
{
	if (!m_occlusionQuery)
	{
		m_occlusionQuery = MP_NEW(COcclusionQuery);
	}

	m_occlusionQuery->StartQuery();
}

void CTexture::EndOcclusionQuery()
{
	m_occlusionQuery->EndQuery();
}

unsigned int CTexture::GetOcclusionQueryPixels()
{
	if (m_occlusionQuery)
	{
		return m_occlusionQuery->GetFragmentCount();
	}
	else
	{
		return 0;
	}
}

bool CTexture::GetOcclusionQueryResult()
{
	if (m_occlusionQuery)
	{
		return (m_occlusionQuery->GetFragmentCount() > 0);
	}
	else
	{
		return true;
	}
}

int CTexture::GetTextureWidth()const
{
	return m_width;
}

int CTexture::GetTextureRealWidth()const
{
	return m_realWidth;
}

void CTexture::SetTextureWidth(const int width)
{
	m_width = width;
	m_realWidth = width;
}

void CTexture::SetTextureRealWidth(const int width)
{
	m_realWidth = width;
}

int CTexture::GetTextureHeight()const
{
	return m_height;
}

int CTexture::GetTextureRealHeight()const
{
	return m_realHeight;
}

void CTexture::SetTextureHeight(const int height)
{
	m_height = height;
	m_realHeight = height;
}

void CTexture::SetTextureRealHeight(const int height)
{
	m_realHeight = height;
}

void CTexture::SetTextureSizeBeforeResize(const int width, const int height, const int realWidth, const int realHeight)
{
	m_textureWidthBeforeResize = width;
	m_textureHeightBeforeResize = height;
	m_textureRealWidthBeforeResize = realWidth;
	m_textureRealHeightBeforeResize = realHeight;
}

int CTexture::GetTextureRealWidthBeforeResize()const
{
	return m_textureRealWidthBeforeResize;
}

int CTexture::GetTextureRealHeightBeforeResize()const
{
	return m_textureRealHeightBeforeResize;
}

int CTexture::GetTextureHeightBeforeResize()const
{
	return m_textureHeightBeforeResize;
}

int CTexture::GetTextureWidthBeforeResize()const
{
	return m_textureWidthBeforeResize;
}

void CTexture::SetNumber(const int oglTextureID)
{
	m_oglTextureID = oglTextureID;
	m_isEnabled = true;
	stat_textureCount++;
}

int CTexture::GetTextureType()const
{
	return m_type;
}

void CTexture::SetTextureType(const int type)
{
	m_type = type;
}

bool CTexture::IsTransparent()const
{
	return m_isTransparent;
}

void CTexture::SetTransparentStatus(const bool isTransparent)
{
	m_isTransparent = isTransparent;
}

bool CTexture::IsYInvert()const
{
	return m_isYInvert;
}

void CTexture::SetYInvertStatus(const bool isYInvert)
{
	m_isYInvert = isYInvert;
}

bool CTexture::IsXInvert()const
{
	return m_isXInvert;
}

void CTexture::SetXInvertStatus(const bool isXInvert)
{
	m_isXInvert = isXInvert;
}

int CTexture::GetKeyColorType()const
{
	return m_keyColorType;
}

void CTexture::SetColor(const BYTE r, const BYTE g, const BYTE b)
{
	m_r = r;
	m_g = g;
	m_b = b;
}

void CTexture::GetColor(BYTE& r, BYTE& g, BYTE& b)
{
	r = m_r;
	g = m_g;
	b = m_b;
}

bool CTexture::GetPixelColor(const int x, const int y, unsigned char& r, unsigned char& _g, unsigned char& b)
{
	if ((x >= m_width) || (y >= m_height) || (x < 0) || (y < 0))
	{
		return false; // неверные ширина или высота
	}

	BYTE* pixels = MP_NEW_ARR(BYTE,m_width * m_height * 3);

	GLFUNC(glBindTexture)(GL_TEXTURE_2D, m_oglTextureID);
	GLFUNC(glGetTexImage)(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	g->tm.RefreshTextureOptimization();

	r = pixels[(y * m_width + x) * 3];
	_g = pixels[(y * m_width + x) * 3 + 1];
	b = pixels[(y * m_width + x) * 3 + 2];

	MP_DELETE_ARR(pixels);

	return true;
}

void CTexture::FreeResources()
{
	Free();
	ClearListeners();
	
	if (GetDynamicTexture() != NULL)
	{
		g->tm.RemoveDynamicTexture(GetDynamicTexture());
	}

	g->tm.DeleteObject(GetID());
}

void CTexture::Free()
{
	if ((m_isEnabled) && (m_oglTextureID != g->tm.GetErrorTextureID()))
	{		
		GLFUNC(glDeleteTextures)(1, (unsigned int*)&m_oglTextureID);
		m_oglTextureID = g->tm.GetErrorTextureID();
	}

	stat_textureCount--;

	if (m_mask != NULL)
	{
		MP_DELETE_ARR(m_mask);//delete[] m_mask;
		m_mask = NULL;
	}

	if (m_rawData != NULL)
	{
		MP_DELETE_ARR(m_rawData);//delete[] m_rawData;
		m_rawData = NULL;
	}

	if (GetUserData(USER_DATA_SRC))
	{
		std::wstring* ptr = (std::wstring*)GetUserData(USER_DATA_SRC);
		MP_DELETE(ptr);//delete GetUserData(USER_DATA_SRC);
		SetUserData(USER_DATA_SRC, NULL);
	}

	m_isEnabled = false;	
}

void CTexture::OnAsynchResourceLoadedPersent(IAsynchResource* /*asynch*/, unsigned char/* percent*/)
{
	// do nothing
}

void CTexture::SetLoadedStatus(const int loadedStatus)
{
	m_loadedStatus = loadedStatus;
}

void CTexture::OnAsynchResourceLoaded(IAsynchResource* asynch)
{
#ifdef RESOURCE_LOADING_LOG
	g->lw.WriteLn("loaded texture: ", asynch->GetFileName(), " deleted ", IsDeleted());
#endif

	if (IsDeleted())
	{
		return;
	}	

	GetLoadingInfo()->SetCanBeResized(true);
	g->tm.LoadFromIFile(this, asynch->GetIFile(), GetLoadingInfo()->IsMipMap());
}

void CTexture::OnAsynchResourceError(IAsynchResource* asynch)
{
    g->lw.WriteLn("[LOADWARNING] Load failed: ", asynch->GetIFile()->get_file_path());

	m_loadedStatus = TEXTURE_LOAD_ERROR;
	OnAfterChanges();

	//g->tm.DeleteObject(g->tm.GetObjectNumber(GetName()));
}

void CTexture::SetMask(unsigned char* const mask, const unsigned int maskSize)
{
	m_mask = mask;
	m_maskSize = maskSize;
}

bool CTexture::IsPixelTransparent(const int x, const int y)const
{
	if ((x < 0) || (x >= m_realWidth) || (y < 0) || (y >= m_realHeight))
	{
		return true;
	}

	if (m_mask)
	{
		if ((x >= m_width) || (y >= m_height))
		{
			return true;
		}

		unsigned int offset = (int)((y * m_realWidth + x) / 8);

		if ((offset >= 0) && (offset < m_maskSize))
		{
			static unsigned char BitSetArr[8] = {1, 2, 4, 8, 16, 32, 64, 128};
			unsigned int checkValue = *(m_mask + offset) & BitSetArr[(y * m_realWidth + x) % 8];
			if (checkValue > 0)
			{
				return false;
			}
		}
		else
		{
			assert(false);
		}

		return true;
	}
	else
	{
		return false;
	}
}

void CTexture::SetReallyUsedStatus(const bool isReallyUsed)
{
	m_isReallyUsed = isReallyUsed;
}

bool CTexture::IsNeedPreMultiply()
{
	return m_isNeedPreMultiply;
}

void CTexture::SetPreMultiplyStatus(bool isNeedPreMultiply)
{
	m_isNeedPreMultiply = isNeedPreMultiply;
}

void CTexture::PremultiplyAlpha()
{
	if (m_rawData == NULL || !m_isTransparent)
	{
		return;
	}

	unsigned char alpha;

	unsigned char* rawData = (unsigned char*)m_rawData;

	for (int i = 0; i < m_realHeight * m_realWidth * 4 ; i=i+4)
	{	
		alpha = rawData[i+3];
		rawData[i] = rawData[i]*alpha >> 8;
		rawData[i+1] = rawData[i+1]*alpha >> 8;
		rawData[i+2] = rawData[i+2]*alpha >> 8;
	}
}

void CTexture::DetachRawData()
{
	m_rawData = NULL;
	m_rawDataSize = 0;
}

void* CTexture::GetRawData()
{
	if (m_isKeepInSystemMemory)
	{
		return m_rawData;
	}

	if ((m_oglTextureID == -1) || (m_oglTextureID == g->tm.GetErrorTextureID()))
	{
		return NULL;
	}

	bool isTexture = GLFUNCR(glIsTexture(m_oglTextureID)) > 0;
	if (!isTexture)
	{
		return NULL;
	}

	GLFUNC(glBindTexture)(GL_TEXTURE_2D, m_oglTextureID);

	GLint width = 0;
	GLFUNC(glGetTexLevelParameteriv)(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	if (width != m_realWidth)
	{
		return NULL;
	}

	GLint height = 0;
	GLFUNC(glGetTexLevelParameteriv)(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
	if (height != m_realHeight)
	{
		return NULL;
	}

	int channelCount = m_isTransparent ? 4 : 3;
	
	void* pictureData = MP_NEW_ARR(unsigned char, channelCount * m_realWidth * m_realHeight);

	if (pictureData)
	{		
		try
		{
			GLFUNC(glPixelStorei)(GL_PACK_ALIGNMENT, 1);
			GLFUNC(glGetTexImage)(GL_TEXTURE_2D, 0, channelCount == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, pictureData);
		}
		catch (...)
		{
		}		
		g->tm.RefreshTextureOptimization();
	}

	return pictureData;
}

void CTexture::SetRawData(void* const rawData, const unsigned int rawDataSize, const bool isDirectChannelOrder)
{
	m_rawData = rawData;
	m_rawDataSize = rawDataSize;
	m_isRawDirectChannelOrder = isDirectChannelOrder;
}

bool CTexture::IsRawDirectChannelOrder()const
{
	return m_isRawDirectChannelOrder;
}

void CTexture::SetPath(const std::wstring& path)
{
	m_path = path;
}

std::wstring CTexture::GetPath()const
{
	return m_path;
}

void CTexture::SetHasNoMipmaps(const bool hasNoMipmaps)
{
	m_hasNoMipmaps = hasNoMipmaps;
}

bool CTexture::HasNoMipmaps()const
{
	return m_hasNoMipmaps;
}

CTexture::~CTexture()
{
	Free();
}