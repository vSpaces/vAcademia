
#include "StdAfx.h"
#include "Video.h"
#include <Assert.h>

CVideo::CVideo():
	m_currentStatus(STATUS_NOT_OPEN),
	m_isDirectChannelOrder(false),
	m_colorChannelCount(0),
	m_updateFrameNum(0),
	m_updateHeight(0),
	m_isLooped(false),
	m_frameData(NULL),
	m_aviStream(NULL),
	m_updateWidth(0),
	m_getFrame(NULL),
	m_frameCount(0),
	m_bitmap(NULL),
	m_duration(0),
	m_position(0),
	m_hdc(NULL),
	m_hdd(NULL),
	m_height(0),
	m_width(0)
{
	for (int k = 0; k < 4; k++)
	{
		m_userData[k] = NULL;
	}
}

void* CVideo::GetUserData(unsigned int userDataID)
{
	if (userDataID < 4)
	{
		return m_userData[userDataID];
	}
	else
	{
		return NULL;
	}
}

void CVideo::SetUserData(unsigned int userDataID, void* data)
{
	if (userDataID < 4)
	{
		m_userData[userDataID] = data;
	}
}

void CVideo::Start()
{
	if (STATUS_NOT_OPEN != m_currentStatus)
	{
		m_lastUpdateTime = GetTickCount();
		m_currentStatus = STATUS_PLAYING;
		m_position = 0;
		m_lastFrameNum = -1;
	}
}
	
void CVideo::Stop()
{
	if (STATUS_NOT_OPEN != m_currentStatus)
	{
		m_currentStatus = STATUS_STOPPED;
		m_position = 0;
		m_lastFrameNum = -1;
	}
}
	
void CVideo::Pause()
{
	if (STATUS_NOT_OPEN != m_currentStatus)
	{
		m_currentStatus = STATUS_PAUSED;
	}
}
	
void CVideo::Play()
{
	Start();
}

unsigned int CVideo::GetDuration()
{
	return m_duration;
}

void CVideo::SetLoop(bool isLooped)
{
	m_isLooped = isLooped;
}

int CVideo::GetPosition()
{
	return m_position;
}

void CVideo::SetPosition(unsigned int pos)
{
	if (pos < m_duration)
	{
		m_position = pos;
	}
}

int CVideo::GetFrameNum()
{
	return (int)(m_position * m_frameCount / m_duration);
}

void CVideo::SetFrameNum(unsigned int num)
{
	unsigned int pos = num * 1000 * m_frameCount / m_duration;

	if (pos < m_duration)
	{
		m_position = pos;
	}
}

bool CVideo::IsLooped()
{
	return m_isLooped;
}

bool CVideo::IsPlayed()
{
	return (STATUS_PLAYING == m_currentStatus);
}

bool CVideo::IsPaused()
{
	return (STATUS_PAUSED == m_currentStatus);
}

IDynamicTexture* CVideo::GetDynamicTexture()
{
	return this;
}

bool CVideo::Open(char* fileName)
{
	if (*fileName == ':')
	{
		fileName++;
	}

	AVIFileInit();
	
	if (AVIStreamOpenFromFile(&m_aviStream, fileName, streamtypeVIDEO, 0, OF_READ, NULL) != 0)
	{
		return false;
	}

	AVIStreamInfo(m_aviStream, &m_aviStreamInfo, sizeof(m_aviStreamInfo));
	m_width = (unsigned short)(m_aviStreamInfo.rcFrame.right - m_aviStreamInfo.rcFrame.left);
	m_height = (unsigned short)(m_aviStreamInfo.rcFrame.bottom - m_aviStreamInfo.rcFrame.top);
	m_frameCount = AVIStreamLength(m_aviStream);
	m_duration = AVIStreamSampleToTime(m_aviStream, m_frameCount);
	m_colorChannelCount = 3;

	m_getFrame = AVIStreamGetFrameOpen(m_aviStream, NULL);
	if (!m_getFrame)
	{
		return false;
	}

	m_currentStatus = STATUS_OPENED;

	m_hdd = DrawDibOpen();
	m_hdc = CreateCompatibleDC(0);
	
	SetTextureSize(m_width, m_height);

	return true;
}

void CVideo::Close()
{
	m_currentStatus = STATUS_NOT_OPEN;

	DeleteObject(m_bitmap);
	m_bitmap = NULL;
	DrawDibClose(m_hdd);
	DeleteDC(m_hdc);

	if (m_getFrame)
	{
		AVIStreamGetFrameClose(m_getFrame);
	}

	if (m_aviStream)
	{
		AVIStreamClose(m_aviStream);
	}

	AVIFileExit();
}

void CVideo::UpdateTime()
{
	if (STATUS_PLAYING == m_currentStatus)
	{
		m_position += (GetTickCount() - m_lastUpdateTime);
		m_lastUpdateTime = GetTickCount();

		if (m_position >= m_duration)
		{
			m_currentStatus = STATUS_OPENED;
			m_position = 0;
			m_lastFrameNum = -1;
		}
	}
	else if (STATUS_OPENED == m_currentStatus)
	{
		m_position = 0;
		m_lastFrameNum = -1;
	}
}

void CVideo::Update()
{
    UpdateTime();

	m_updateWidth = m_width;
	m_updateHeight = m_height;
	m_updateTextureWidth = m_textureWidth;
	m_updateTextureHeight = m_textureHeight;
	m_updateFrameNum = GetFrameNum();
	m_lastFrameNum = m_updateFrameNum;
	m_updateHDC = m_hdc;
	m_updateBitmap = m_bitmap;
	m_updateGetFrame = m_getFrame;
}

void CVideo::UpdateFrame()
{
	LPBITMAPINFOHEADER lpbi;
	lpbi = (LPBITMAPINFOHEADER)AVIStreamGetFrame(m_updateGetFrame, m_updateFrameNum);
	char* rawFrameData = (char *)lpbi + lpbi->biSize + lpbi->biClrUsed * sizeof(RGBQUAD);

	SelectObject(m_updateHDC, m_updateBitmap);
	DrawDibDraw(m_hdd, m_updateHDC, 0, 0, m_updateTextureWidth, m_updateTextureHeight, lpbi, rawFrameData, 0, 0, m_updateWidth, m_updateHeight, 0);
}

bool CVideo::IsNeedUpdate()
{
//	bool isPlaying = (m_currentStatus == STATUS_PLAYING);
	UpdateTime();

	return (m_lastFrameNum != GetFrameNum());	
}

void* CVideo::GetFrameData()
{
	return m_frameData;
}

unsigned short CVideo::GetSourceWidth()
{
	return m_width;
}

unsigned short CVideo::GetSourceHeight()
{
	return m_height;
}

unsigned short CVideo::GetTextureWidth()
{
	return m_textureWidth;
}

unsigned short CVideo::GetTextureHeight()
{
	return m_textureHeight;
}

void CVideo::SetTextureSize(int width, int height)
{
	assert(width > 0);
	assert(height > 0);

	if ((width <= 0) || (height <= 0))
	{
		return;
	}

	if ((m_textureWidth != width) || (m_textureHeight != height))
	{
		if (m_bitmap)
		{
			DeleteObject(m_bitmap);
		}

		BITMAPINFOHEADER bmih;
		bmih.biSize = sizeof (BITMAPINFOHEADER); 
		bmih.biPlanes = 1;       
		bmih.biBitCount = 24;      
		bmih.biWidth = width;     
		bmih.biHeight = -height;     
		bmih.biCompression = BI_RGB;  
		m_bitmap = CreateDIBSection(m_hdc, (BITMAPINFO*)(&bmih), DIB_RGB_COLORS, (void**)(&m_frameData), NULL, NULL);
	}

	m_textureWidth = (unsigned short)width;
	m_textureHeight = (unsigned short)height;
}

unsigned char CVideo::GetColorChannelCount()
{
	return m_colorChannelCount;
}

bool CVideo::IsDirectChannelOrder()
{
	return m_isDirectChannelOrder;
}

void CVideo::Destroy()
{
}

CVideo::~CVideo()
{
    if (STATUS_NOT_OPEN != m_currentStatus)
	{
		Close();
	}

	if (m_frameData)
	{
		delete[] m_frameData;
	}
}