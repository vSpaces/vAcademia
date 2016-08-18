
#include "StdAfx.h"
#include "NECompressedTextureLoader.h"

CNECompressedTextureLoader::CNECompressedTextureLoader():
	m_width(0),
	m_height(0),
	m_isTransparent(false)
{
}

bool CNECompressedTextureLoader::LoadTexture(std::wstring& /*fileName*/)
{
	return false;
}

bool CNECompressedTextureLoader::LoadTexture(std::wstring& /*fileName*/, const void* /*buffer*/, int /*size*/)
{
	return false;
}

bool CNECompressedTextureLoader::LoadFromMemory(void* data, int width, int height, bool /*isTransparent*/)
{
	CleanUp();

	int allSize = width;
	
	int levelCount = *(unsigned short *)data;
	m_isTransparent = *((unsigned short *)(data) + 1) == 1;
	width = *((unsigned short *)(data) + 2);
	height = *((unsigned short *)(data) + 3);
	m_width = width;
	m_height = height;

	int checkedSize = 8 + levelCount * 8;

	if ((m_width <= 0) || (m_height <= 0) || (levelCount <= 0))
	{
		return false;
	}

	data = ((unsigned int *)(data) + 2);

	for (int i = 0; i < levelCount; i++)
	{
		int size = *(unsigned int *)data;
		if (size < 0)
		{
			return false;
		}

		checkedSize += size;
		if (checkedSize > allSize)
		{
			return false;
		}

		int format = *((unsigned int *)(data) + 1);

		data = ((unsigned int *)(data) + 2);

		m_mipMapData.push_back(data);
		m_mipMapSize.push_back(size);
		m_mipMapFormat.push_back(format);

		data = ((unsigned char *)(data) + size);

		width = width / 2;
		height = height / 2;
		if (width == 0)
		{
			width = 1;
		}
		if (height == 0)
		{
			height = 1;
		}
	}

	return (checkedSize == allSize);
}

unsigned char CNECompressedTextureLoader::GetChannelCount()const
{
	return (3 + (unsigned char)m_isTransparent);
}

unsigned int CNECompressedTextureLoader::GetWidth()const
{
	return m_width;
}

unsigned int CNECompressedTextureLoader::GetHeight()const
{
	return m_height;
}

bool CNECompressedTextureLoader::ApplyKeyColor(unsigned char /*keyR*/, unsigned char /*keyG*/, unsigned char /*keyB*/)
{
	return false;
}

bool CNECompressedTextureLoader::ApplyKeyColorFromImage()
{
	return false;
}

bool CNECompressedTextureLoader::ApplyDefaultAlpha(unsigned char /*alpha*/)
{
	return false;
}

void CNECompressedTextureLoader::EnlargeTexture()
{
}

bool CNECompressedTextureLoader::ToPOT()
{
	return false;
}

void CNECompressedTextureLoader::Resize(int /*width*/, int /*height*/)
{
}

void CNECompressedTextureLoader::Crop(unsigned int /*width*/, unsigned int /*height*/)
{
}

void* CNECompressedTextureLoader::GetData(unsigned int frameID)const
{
	if ((frameID >= 0) && (frameID < m_mipMapData.size()))
	{
		return m_mipMapData[frameID];
	}
	else
	{
		return NULL;
	}
}

unsigned int CNECompressedTextureLoader::GetDataSize(unsigned int frameID)const
{
	if ((frameID >= 0) && (frameID < m_mipMapSize.size()))
	{
		return m_mipMapSize[frameID];
	}
	else
	{
		return 0;
	}
}

unsigned int CNECompressedTextureLoader::GetUserData(unsigned int frameID)const
{
	if ((frameID >= 0) && (frameID < m_mipMapFormat.size()))
	{
		return m_mipMapFormat[frameID];
	}
	else
	{
		return 0;
	}
}

bool CNECompressedTextureLoader::IsDirectChannelOrder()const
{
	return true;
}

unsigned char CNECompressedTextureLoader::GetFrameCount()
{
	return (unsigned char)m_mipMapData.size();
}

void CNECompressedTextureLoader::CleanUp()
{
	m_mipMapData.clear();
	m_mipMapSize.clear();
	m_mipMapFormat.clear();
}

unsigned char CNECompressedTextureLoader::GetUsageType()
{
	return USAGE_TYPE_DIFFUSE_COMPRESSED;
}

CNECompressedTextureLoader::~CNECompressedTextureLoader()
{
	CleanUp();
}