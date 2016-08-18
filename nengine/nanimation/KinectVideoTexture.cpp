
#include "StdAfx.h"
#include "KinectVideoTexture.h"

#define REAL_KINECT_VIDEO_WIDTH		1024
#define REAL_KINECT_VIDEO_HEIGHT	512
#define COLOR_THRESHOLD				75
#define DEPTH_OFFSET_X				20
#define DEPTH_OFFSET_Y				30

CKinectVideoTexture::CKinectVideoTexture(IKinect* kinect):
	m_isAnalyzeEnabled(false),
	m_kinect(kinect),
	m_data(NULL),
	m_size(0)
{
	assert(m_kinect);
	if (m_kinect)
	{
		m_kinect->EnableVideo(true);
	}
}

// Обновить состояние объекта (не текстуру)
void CKinectVideoTexture::Update()
{
}

void CKinectVideoTexture::AllocBufferIfNeeded(unsigned int size)
{
	if (m_size < size)
	{
		if (m_data)
		{
			MP_DELETE_ARR(m_data);
			m_data = NULL;
		}

		m_data = MP_NEW_ARR(unsigned char, size);
		m_size = size;
	}
}

// Обновить текстуру
void CKinectVideoTexture::UpdateFrame()
{
	unsigned int videoSize;
	if (m_kinect)
	if ((videoSize = m_kinect->GetNextVideoFrameSize()) > 0)
	{
		unsigned int size = REAL_KINECT_VIDEO_WIDTH * REAL_KINECT_VIDEO_HEIGHT * 4;
		AllocBufferIfNeeded(size);
		m_kinect->GetNextVideoFrame((void**)&m_data);

		for (unsigned int i = 0; i < videoSize; i += 4)
		{
			m_data[i + 3] = 255;
		}

		for (int i = KINECT_VIDEO_HEIGHT - 1; i >= 0; i--)
		{
			memcpy(m_data + i * REAL_KINECT_VIDEO_WIDTH * 4, m_data + i * KINECT_VIDEO_WIDTH * 4, KINECT_VIDEO_WIDTH * 4);
		}
	}
}

unsigned int CKinectVideoTexture::GetPixelsWithGivenChannel(char channelID)
{	
	int otherChannelID1 = 0;
	int otherChannelID2 = 1;
	if (channelID == 0)
	{
		otherChannelID1 = 1;
		otherChannelID2 = 2;
	}
	else if (channelID == 1)
	{
		otherChannelID1 = 0;
		otherChannelID2 = 2;
	}

	unsigned int count = 0;

	unsigned int videoSize = m_kinect->GetNextVideoFrameSize();
	unsigned int depthSize = m_kinect->GetNextDepthFrameSize();

	if ((videoSize == 0) || (depthSize == 0))
	{
		return 0;
	}

	unsigned short* depthData = MP_NEW_ARR(unsigned short, depthSize / 2);
	m_kinect->GetNextDepthFrame((void**)&depthData);

	unsigned char* data = MP_NEW_ARR(unsigned char, videoSize);
	m_kinect->GetNextVideoFrame((void**)&data);

	int k = 0;

	const int offsetX = DEPTH_OFFSET_X;
	const int offsetY = DEPTH_OFFSET_Y;

	for (int y = 0; y < KINECT_VIDEO_HEIGHT; y++)
	{
		for (int x = 0; x < KINECT_VIDEO_WIDTH; x++, k++)			
		{
			unsigned short value = depthData[k] << 13;
			value = value >> 13;
			
			if (value == m_kinect->GetPlayerIndex() + 1)				
			{
				unsigned int i = (y + offsetY) * KINECT_VIDEO_WIDTH * 4 + (x + offsetX) * 4;
				if (i < videoSize)
				{
					if (((data[i + 2] > COLOR_THRESHOLD) && (data[i + 1] * 2 <= data[i + 2]) &&
						(data[i + 0] * 4 <= data[i + 2] * 3)) ||
						((data[i + 2] > COLOR_THRESHOLD) && (data[i + 1] * 4 <= data[i + 2] * 3) &&
						(data[i + 0] * 2 <= data[i + 2])))
					{
						count++;
					}
				}
			}				
		}
	}

	MP_DELETE_ARR(data);
	MP_DELETE_ARR(depthData);

	return count;
}

void CKinectVideoTexture::EnableAnalyzeMode(bool isEnabled)
{
	m_isAnalyzeEnabled = isEnabled;	
}

bool CKinectVideoTexture::IsAnalyzeEnabled()const
{
	return m_isAnalyzeEnabled;
}

// Вернуть ссылку на массив с построчным содержимым кадра	
void* CKinectVideoTexture::GetFrameData()
{
	return m_data;
}

// Вернуть ширину исходной картинки в пикселях
unsigned short CKinectVideoTexture::GetSourceWidth()
{
	return REAL_KINECT_VIDEO_WIDTH;
}

// Вернуть высоту исходной картинки в пикселях
unsigned short CKinectVideoTexture::GetSourceHeight()
{
	return REAL_KINECT_VIDEO_HEIGHT;
}

// Вернуть ширину результирующей текстуры в пикселях
unsigned short CKinectVideoTexture::GetTextureWidth()
{
	return REAL_KINECT_VIDEO_WIDTH;
}

// Вернуть высоту результирующей текстуры в пикселях
unsigned short CKinectVideoTexture::GetTextureHeight()
{
	return REAL_KINECT_VIDEO_HEIGHT;
}

// Установить размер результирующей текстуры
void CKinectVideoTexture::SetTextureSize(int /*width*/, int /*height*/)
{
}
	
// Вернуть количество цветовых каналов, 3 для RGB, 4 для RGBA
unsigned char CKinectVideoTexture::GetColorChannelCount()
{
	return 4;
}

// Порядок цветовых каналов прямой (RGB) или обратный (BGR)
bool CKinectVideoTexture::IsDirectChannelOrder()
{
	return true;
}

// Нужно ли обновлять текстуру (или сейчас её содержимое не изменяется?)
bool CKinectVideoTexture::IsNeedUpdate()
{	
	if (m_kinect)
	{
		return m_kinect->IsNextVideoFrameAvailable();
	}

	return false;
}

// Уничтожь себя
// (требуется для того, чтобы nengine мог сам решить когда его безопасно удалить)
void CKinectVideoTexture::Destroy()
{
	MP_DELETE_THIS;
}

// для обоих случаев реализации IDynamicTexture должны 
// поддерживать userDataID хотя бы от 0 до 3 включительно
// получить пользовательские (специфичные для вида динамической текстуры) данные
void* CKinectVideoTexture::GetUserData(unsigned int userDataID)
{
	if ((userDataID >= 0) && (userDataID <= 3))
	{
		return m_userData[userDataID];
	}

	return NULL;
}

// установить пользовательские (специфичные для вида динамической текстуры) данные
void CKinectVideoTexture::SetUserData(unsigned int userDataID, void* data) 
{
	if ((userDataID >= 0) && (userDataID <= 3))
	{
		m_userData[userDataID] = data;
	}
}

void CKinectVideoTexture::SetFreezState(bool /*isFreezed*/)
{
}

bool CKinectVideoTexture::IsFreezed()
{
	return false;
}

bool CKinectVideoTexture::IsFirstFrameSetted()
{
	return (m_data != NULL);
}

CKinectVideoTexture::~CKinectVideoTexture()
{
	if (m_kinect)
	{
		m_kinect->EnableVideo(false);
		m_kinect = NULL;
	}

	if (m_data)
	{
		MP_DELETE_ARR(m_data);
		m_data = NULL;
	}
}