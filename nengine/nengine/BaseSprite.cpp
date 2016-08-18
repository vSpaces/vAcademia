
#include "StdAfx.h"
#include "BaseSprite.h"
#include "GlobalSingletonStorage.h"
#include "cal3d/memory_leak.h"

#define MAX_DEPTH	(256 * 256 * 256 - 1)

CBaseSprite::CBaseSprite(unsigned char type):
	m_x(0),
	m_y(0),	
	m_depth(0),
	m_width(0),
	m_height(0),
	m_type(type),
	m_scissorsX(0),
	m_scissorsY(0),
	m_sortValue(0),
	m_scene2d(NULL),	
	m_isLoaded(false),	
	m_isVisible(false),
	m_maskSprite(NULL),
	m_scissorsWidth(-1),
	m_scissorsHeight(-1),
	m_isSizeDefined(false),
	m_uselessAsMaskCount(0)	
#ifdef USE_VISIBLE_DESTINATION
	, m_visibilityProvider(NULL)
#endif
{
}

bool CBaseSprite::IsNeedToUpdate()
{
	return false;
}

void CBaseSprite::SetScene2D(C2DScene* const scene2d)
{
	m_scene2d = scene2d;
}

void CBaseSprite::SetCoords(const int x, const int y)
{
	if ((m_scene2d) && ((m_isVisible) || (m_uselessAsMaskCount > 0)) && ((m_x != x) || (m_y != y)))
	{
		m_scene2d->OnParamsChanged();
	}

	m_x = x;
	m_y = y;
}

C2DScene* CBaseSprite::GetScene2D()const
{
	return m_scene2d;
}

void CBaseSprite::SetSize(const int width, const int height)
{
	if ((m_scene2d) && ((m_isVisible) || (m_uselessAsMaskCount > 0)) && ((m_width != width) || (m_height != height)))
	{
		m_scene2d->OnParamsChanged();
	}

	m_isSizeDefined = true;
	m_width = width;
	m_height = height;
}

void CBaseSprite::SetDepth(int depth)
{
	if (depth > 0)
	{
		depth = 0;
	}

	if (depth < -MAX_DEPTH)
	{
		depth = -MAX_DEPTH;
	}

	if (m_depth != depth)
	{
		m_depth = depth;
		if ((m_scene2d) && (m_isVisible))
		{
			m_scene2d->OnChanged();
		}

		int dp = -m_depth;
		memcpy(((unsigned char *)&m_sortValue) + 1, &dp, 3);
	}
}


#ifndef USE_VISIBLE_DESTINATION
	void CBaseSprite::SetVisible(const bool isVisible)
#else
	void CBaseSprite::SetVisible(const bool isVisible, const int from)
#endif
{
	if (m_isVisible != isVisible)
	{
		m_isVisible = isVisible;
		if (m_scene2d)
		{
			m_scene2d->OnChanged();
		}
	}	

#ifdef USE_VISIBLE_DESTINATION
	if (from != 0)
	if (m_visibilityProvider)
	if (isVisible != m_visibilityProvider->IsVisible())
	{
		g->lw.WriteLn("Visibility error ", GetName(), " from ", from);
	}
#endif
}

#ifdef USE_VISIBLE_DESTINATION
void CBaseSprite::SetVisibilityProvider(IVisibilityProvider* visibilityProvider)
{
	m_visibilityProvider = visibilityProvider;
}

IVisibilityProvider* CBaseSprite::GetVisibilityProvider()const
{
	return m_visibilityProvider;
}
#endif

void CBaseSprite::SetLoadedStatus(const bool isLoaded)
{
	if (m_isLoaded != isLoaded)
	{
		m_isLoaded = isLoaded;
		if ((m_scene2d) && (m_isVisible))
		{
			m_scene2d->OnChanged();
		}
	}
}

void CBaseSprite::SetScissors(const int x, const int y, const int width, const int height)
{
	if ((m_scissorsX != x) || (m_scissorsY != y) || (m_scissorsWidth != width) || (m_scissorsHeight != height))
	{
		m_scissorsX = x;
		m_scissorsY = y;
		m_scissorsWidth = width;
		m_scissorsHeight = height;

		if (m_maskSprite)
		{
			m_maskSprite->DecrementUselessAsMask();
			m_maskSprite = NULL;
		}

		if ((m_scene2d) && (m_isVisible))
		{
			m_scene2d->OnChanged();
		}
	}
	else if (m_maskSprite)
	{
		m_maskSprite->DecrementUselessAsMask();
		m_maskSprite = NULL;

		if ((m_scene2d) && (m_isVisible))
		{
			m_scene2d->OnChanged();
		}
	}
}

void CBaseSprite::OnBeforeDraw(CPolygons2DAccum* const accum)
{
	if (m_maskSprite)
	{
		m_scissorsX = m_maskSprite->GetX();
		m_scissorsY = m_maskSprite->GetY();
		m_scissorsWidth = m_maskSprite->GetWidth();
		m_scissorsHeight = m_maskSprite->GetHeight();
	}

	if (m_scissorsWidth != -1)
	{
		if (accum)
		{
			accum->ChangeScissors(m_scissorsX, m_scissorsY, m_scissorsWidth, m_scissorsHeight);
			return;
		}

		if (g->rs.GetInt(SCISSORS_SCALE) != 1)
		{
			int scale = g->rs.GetInt(SCISSORS_SCALE);
			g->stp.EnableScissors(m_scissorsX / scale, (g->stp.GetCurrentHeight() - m_scissorsY - m_scissorsHeight) / scale + (g->stp.GetCurrentHeight() - g->stp.GetCurrentHeight() / scale), m_scissorsWidth / scale, m_scissorsHeight / scale);			
		}
		else
		{
			g->stp.EnableScissors(m_scissorsX, (g->stp.GetCurrentHeight() - m_scissorsY - m_scissorsHeight), m_scissorsWidth, m_scissorsHeight);
		}
	}
}

void CBaseSprite::OnAfterDraw(CPolygons2DAccum* const accum)
{
	if (m_scissorsWidth != -1)
	{
		if (accum)
		{
			accum->DisableScissors();
		}
		else
		{
			g->stp.DisableScissors();
		}
	}
}

void CBaseSprite::IncrementUselessAsMask()
{
	m_uselessAsMaskCount++;
}

void CBaseSprite::DecrementUselessAsMask()
{
	m_uselessAsMaskCount--;
}

CBaseSprite* CBaseSprite::GetMaskSprite()const
{
	return m_maskSprite;
}

void CBaseSprite::SetMaskSprite(CBaseSprite* const mask)
{
	if (m_maskSprite != mask)
	{
		if (mask)
		{
			mask->IncrementUselessAsMask();
		}

		if (m_maskSprite)
		{
			m_maskSprite->DecrementUselessAsMask();

			m_scissorsX = m_maskSprite->GetX();
			m_scissorsY = m_maskSprite->GetY();
			m_scissorsWidth = m_maskSprite->GetWidth();
			m_scissorsHeight = m_maskSprite->GetHeight();
		}

		m_maskSprite = mask;

		if ((m_scene2d) && (m_isVisible))
		{
			m_scene2d->OnParamsChanged();
		}
	}	
}

void CBaseSprite::SetType(const unsigned char type)
{
	m_type = type;
}

CBaseSprite::~CBaseSprite()
{
}