
#include "StdAfx.h"
#include "nrmAnim.h"
#include "nrmImage.h"

#include "GlobalSingletonStorage.h"
#include "RMContext.h"

nrmAnim::nrmAnim(mlMedia* apMLMedia): 
	m_isFirstLoaded(false),
	nrmObject(apMLMedia)
{
}

// загрузка нового ресурса
bool nrmAnim::SrcChanged()
{
	CalQuaternion	a(CalVector(0,0,0), 1);
	CalVector	axes; float angle;
	a.extract(axes, angle);

	USES_CONVERSION;
	// 1. проверить был ли до этого создан буфер вершин
	// 2. проверить, добавлен ли этот объект в сцену
	const wchar_t* pwcSrc=mpMLMedia->GetILoadable()->GetSrc();
	if(pwcSrc==NULL) return false;
	mlString sSRC=pwcSrc;
	if(sSRC.empty()) return false;

	CWComString name;
	if (mpMLMedia->GetStringProp("name"))
	{
		name = mpMLMedia->GetStringProp("name");
	}
	if (!m_sprite)
	{
		name += L"_";
		name += pwcSrc;
		name += L"_";
		name += IntToWStr(g->sprm.GetCount()).c_str();

		int spriteID = g->sprm.AddObject(name.GetBuffer(0));
		m_sprite = g->sprm.GetObjectPointer(spriteID);
		m_sprite->AddChangesListener(this);
		gRM->scene2d->AddSprite(spriteID, 0, 0);
//		m_sprite->SetVisibilityProvider(this);
	}

	m_sprite->LoadFromTexture( sSRC);

	bool isLoaded = m_sprite->IsLoaded();
		
	if (m_sprite)
	{
		mlSize sz = mpMLMedia->GetIVisible()->GetSize();
		
		if (!m_isFirstLoaded)
		{
			sz.width = m_sprite->GetWidth();
			sz.height = m_sprite->GetHeight();
		}

		if (sz.width == 0) 
		{
			sz.width = m_sprite->GetWidth();
		}

		if (sz.height == 0) 
		{
			sz.height = m_sprite->GetHeight();
		}

		mpMLMedia->GetIVisible()->SetSize(sz);
		mlPoint pnt = mpMLMedia->GetIVisible()->GetAbsXY();
		m_sprite->SetCoords(pnt.x, pnt.y);
	}

	AbsDepthChanged();
	AbsVisibleChanged();
	PlayingChanged();

	if (isLoaded)
	{
		GetMLMedia()->GetILoadable()->onLoad( RMML_LOAD_STATE_SRC_LOADED);
	}

	return true;
}

void nrmAnim::OnChanged(int /*eventID*/)
{
	mlSize sz = mpMLMedia->GetIVisible()->GetSize();
	mlPoint pnt=mpMLMedia->GetIVisible()->GetAbsXY();

	if (m_sprite)
	{
		CTexture* texture = g->tm.GetObjectPointer(m_sprite->GetTextureID(0));

		if (!m_sprite->IsSizeDefined())
		{
			if ((2 == sz.width) && (2 == sz.height))
			{
				sz.width = texture->GetTextureWidth();
				sz.height = texture->GetTextureHeight();
			}
		}
		else
		{
			m_sprite->GetSize(sz.width, sz.height);
		}
		
		mpMLMedia->GetIVisible()->SetSize(sz);
		m_sprite->SetCoords(pnt.x, pnt.y);
	}
	
	AbsVisibleChanged();
	AbsEnabledChanged();
	AbsOpacityChanged();
	SizeChanged();
	AbsXYChanged();
	m_isFirstLoaded = false;

	GetMLMedia()->GetILoadable()->onLoad( RMML_LOAD_STATE_SRC_LOADED);
}

// изменилось абсолютное положение 
void nrmAnim::AbsXYChanged()
{
	if (!m_sprite)
	{
		return;
	}

	mlPoint pnt = mpMLMedia->GetIVisible()->GetAbsXY();
	m_sprite->SetCoords(pnt.x, pnt.y);
} 

void nrmAnim::SizeChanged()
{
	if (!m_sprite)	
	{
		return;
	}

	mlSize sz = mpMLMedia->GetIVisible()->GetSize();
	m_sprite->SetSize(sz.width, sz.height);
}

// изменилась абсолютная глубина
void nrmAnim::AbsDepthChanged()
{
	if (!m_sprite)	
	{
		return;
	}

	m_sprite->SetDepth(GetMLMedia()->GetIVisible()->GetAbsDepth());
}

// изменилась абсолютная видимость
void nrmAnim::AbsVisibleChanged()
{
	if (!m_sprite)
	{
		return;
	}

#ifndef USE_VISIBLE_DESTINATION
	m_sprite->SetVisible(GetMLMedia()->GetIVisible()->GetAbsVisible());
#else
	m_sprite->SetVisible(GetMLMedia()->GetIVisible()->GetAbsVisible(), 14);
#endif
}

void nrmAnim::AbsEnabledChanged()
{
	// do nothing
}

mlSize nrmAnim::GetMediaSize()
{
	mlSize size; ML_INIT_SIZE(size);

	if (m_sprite)
	{
		if (m_sprite->GetTextureID(0) < 0) 
		{
			return size;
		}

		int totalFrames = mpMLMedia->GetIContinuous()->GetLength();
		if (0 == totalFrames)
		{
			totalFrames = 1;
		}

		CTexture* texture = g->tm.GetObjectPointer(m_sprite->GetTextureID(0));
		size.width = (int)(texture->GetTextureWidth() / totalFrames);
		size.height = texture->GetTextureHeight();
	}

	return size;
}

void nrmAnim::PlayingChanged()
{
}

void nrmAnim::FrameChanged()
{
}

// переставить на новое место (ms)
void nrmAnim::PosChanged()
{
}

unsigned long nrmAnim::GetCurPos()
{
	return 0;
}

// запрашивает значение _duration (продолжительность в ms)
long nrmAnim::GetDuration()
{
	return m_sprite->GetFrameCount() * m_sprite->GetDeltaTime();
}

void nrmAnim::SetMask(int aiX, int aiY, int aiW, int aiH)
{
	if (!m_sprite)
	{
		return;
	}

	m_sprite->SetScissors(aiX, aiY, aiW, aiH);
}

int nrmAnim::IsPointIn(const mlPoint aPnt)
{
	if (!m_sprite)	
	{
		return 0;
	}

	return (!m_sprite->IsPixelTransparent(aPnt.x, aPnt.y)) ? 1 : 0;
}

// изменилась абсолютная видимость
void nrmAnim::AbsOpacityChanged()
{
	if (m_sprite)
	{
		float op = mpMLMedia->GetIVisible()->GetAbsOpacity();
		m_sprite->SetAlpha((unsigned char)(op * 255.0f));
	}
}

void nrmAnim::LengthChanged()
{ 
	// изменилось количество кадров в картиночной анимации
	int totalFrames = mpMLMedia->GetIContinuous()->GetLength();
	if (m_sprite)
	{
		m_sprite->SetFrameCount(totalFrames);
	}
}

void nrmAnim::DurationChanged()
{ 
	// изменилась продолжительность картиночной анимации (в ms)
	long duration = mpMLMedia->GetIContinuous()->GetLength();

	if (m_sprite)
	{
		m_sprite->SetDeltaTime(duration);
	}
}

void nrmAnim::FPSChanged()
{ 
	// изменился FPS картиночной анимации
//	int fps = mpMLMedia->GetIContinuous()->GetFPS();
}

nrmAnim::~nrmAnim()
{
}