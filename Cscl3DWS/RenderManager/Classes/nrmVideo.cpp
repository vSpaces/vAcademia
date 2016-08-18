// nrmVideo.cpp: implementation of the nrmVideo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "nrmVideo.h"
#include "nrmImage.h"
#include "rmn3dVideo.h"
#include "VideoPlugin.h"
#include "IDynamicTexture.h"

#define	VIDEOTIMEEFFECTOR	10000

nrmVideo::nrmVideo(mlMedia* apMLMedia): 
	nrmObject(apMLMedia),
	m_video(NULL)
{
	
}

void	nrmVideo::OnSetRenderManager()
{
}

// загрузка нового ресурса
bool nrmVideo::SrcChanged()
{
	const wchar_t* src = mpMLMedia->GetILoadable()->GetSrc();
	if (!src) 
	{
		return false;
	}

	USES_CONVERSION;
	std::string path = W2A(src);

	m_video = CreateVideoObject();
	if (!m_video->Open((char *)path.c_str()))
	{
		DeleteVideoObject(m_video);
		m_video = NULL;
		return false;
	}
	
	mlSize sz;
	sz.width = m_video->GetDynamicTexture()->GetSourceWidth();
	sz.height = m_video->GetDynamicTexture()->GetSourceHeight();
	mpMLMedia->GetIVisible()->SetSize(sz);
	
	AbsDepthChanged();
	AbsXYChanged();
	AbsVisibleChanged();
	PlayingChanged();

	return true;
}

// изменилось абсолютное положение 
void nrmVideo::AbsXYChanged()
{
	// Deprecated
} 

void nrmVideo::SizeChanged()
{
	// Deprecated
}

// изменилась абсолютная глубина
void nrmVideo::AbsDepthChanged()
{
	// Deprecated
}

// изменилась абсолютная видимость
void nrmVideo::AbsVisibleChanged()
{
	// do nothing
}

mlSize nrmVideo::GetMediaSize()
{
	mlSize size; 
	ML_INIT_SIZE(size);
	if (!m_video)
	{
		rm_trace("Error: in nrmVideo::GetMediaSize()\n");
		return size;
	}
	
	size.width = m_video->GetDynamicTexture()->GetSourceWidth();
	size.height = m_video->GetDynamicTexture()->GetSourceHeight();

	return size;
}

void nrmVideo::PlayingChanged()
{
	if (!m_video)
	{
		rm_trace("Error: in nrmVideo::PlayingChanged()\n");
		return;
	}

	bool play = GetMLMedia()->GetIContinuous()->GetPlaying();
	if (play)
	{
		m_video->Play();
	}
	else
	{
		m_video->Stop();
	}
}

void nrmVideo::FrameChanged()
{
	if (!m_video)
	{
		rm_trace("Error: in nrmVideo::FrameChanged()\n");
		return;
	}

	unsigned long frame=GetMLMedia()->GetIContinuous()->GetCurFrame();
	m_video->SetFrameNum(frame);
	GetMLMedia()->GetIContinuous()->SetCurFrame(m_video->GetFrameNum());
}

void nrmVideo::FrozenChanged()
{
}

// переставить на новое место (ms)
void nrmVideo::PosChanged()
{
	if (!m_video)
	{
		rm_trace("Error: in nrmVideo::PosChanged()\n");
		return;
	}

	unsigned long pos = GetMLMedia()->GetIContinuous()->GetCurPos();
	m_video->SetPosition(pos * VIDEOTIMEEFFECTOR);
	GetMLMedia()->GetIContinuous()->SetCurPos(m_video->GetPosition() / VIDEOTIMEEFFECTOR);
}

unsigned long nrmVideo::GetCurPos()
{
	if (!m_video)
	{
		return 0;
	}

	return m_video->GetPosition();
}

// запрашивает значение _duration (продолжительность в ms)
long nrmVideo::GetDuration()
{
	if (!m_video)
	{
		return 0;
	}

	return m_video->GetDuration();
}

void nrmVideo::WindowedChanged()
{
	assert(false);
}

void nrmVideo::OnLoaded()
{
	PlayingChanged();
}

void nrmVideo::AbsOpacityChanged()
{
	// изменилась абсолютная видимость
}

nrmVideo::~nrmVideo()
{
}