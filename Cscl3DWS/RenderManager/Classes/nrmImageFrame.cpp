// nrmImageFrame.cpp: implementation of the nrmImageFrame class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "nrmImageFrame.h"
#include "nrmPlugin.h"
#include "nrmViewport.h"
#include "nrm3DObject.h"
#include "HelperFunctions.h"

#include "GlobalSingletonStorage.h"
#include "RMContext.h"

#define MIN_SYNCH_TIME	500

nrmImageFrame::nrmImageFrame(mlMedia* apMLMedia) : 
	MP_WSTRING_INIT(m_src),
	m_loadingPriority(255),
	m_isFirstLoaded(true),
	nrmObject(apMLMedia),
	m_paper(NULL)
{
}

// загрузка нового ресурса
bool nrmImageFrame::SrcChanged()
{
	USES_CONVERSION;
	const wchar_t* pwcSrc = mpMLMedia->GetILoadable()->GetSrc();
	if (!pwcSrc) 
	{
		return false;	
	}
	
	mlString sSRC = pwcSrc;
	if (sSRC.empty()) 
	{
		return false;
	}

	if (!wcscmp(m_src.c_str(), pwcSrc))
	{
		GetMLMedia()->GetILoadable()->onLoad( RMML_LOAD_STATE_SRC_LOADED);
		return true;
	}

	CWComString name;
	if (mpMLMedia->GetStringProp("name"))
	{
		name = mpMLMedia->GetStringProp("name");
	}
	m_src = pwcSrc;
	ILogWriter* log = GetRenderManager()->GetLogWriter();
	if (!m_paper)
	{
		name += L"_";
		name += pwcSrc;
		name += L"_";
		name += IntToWStr(g->sprm.GetCount()).c_str();

		m_paper = MP_NEW(CPaper);
		m_paper->SetName(name.GetBuffer(0));
		int spriteID = g->sprm.ManageObject(m_paper);
		m_paper->AddChangesListener(this);
		gRM->scene2d->AddSprite(spriteID, 0, 0);
//		m_paper->SetVisibilityProvider(this);
	}

	m_paper->LoadFromTexture( m_src);

	bool isLoaded = m_paper->IsLoaded();
		
	if (m_paper)
	{
		mlSize sz = mpMLMedia->GetIVisible()->GetSize();
		
		if (!m_isFirstLoaded)
		{
			sz.width = m_paper->GetWidth();
			sz.height = m_paper->GetHeight();
		}

		if (sz.width == 0) 
		{
			sz.width = m_paper->GetWidth();
		}

		if (sz.height == 0) 
		{
			sz.height = m_paper->GetHeight();
		}

		mpMLMedia->GetIVisible()->SetSize(sz);
		mlPoint pnt = mpMLMedia->GetIVisible()->GetAbsXY();
		m_paper->SetCoords(pnt.x, pnt.y);
	}

	ScanPlugins();
	InitializePlugins();
	MarginsChanged();
	AbsDepthChanged();
	AbsVisibleChanged();
	AbsEnabledChanged();
	AbsOpacityChanged();
	TileChanged();

	if (m_loadingPriority != 255)
	{
		SetLoadingPriority(m_loadingPriority);
	}

	if (isLoaded)
	{
		m_isFirstLoaded = false;
		GetMLMedia()->GetILoadable()->onLoad( RMML_LOAD_STATE_SRC_LOADED);
	}

	return true;
}

// смена приоритета загрузки ресурса
void nrmImageFrame::LoadingPriorityChanged( ELoadingPriority aPriority)
{
	SetLoadingPriority(aPriority);
}

void nrmImageFrame::SetLoadingPriority(unsigned char aPriority)
{
	m_loadingPriority = aPriority;

	if (m_src.size() > 0)
	{
		IAsynchResource* res = g->rl->GetAsynchResource();
		if (res)
		{
			res->SetScriptPriority(aPriority);
		}	
	}
}

// изменилось абсолютное положение 
void nrmImageFrame::AbsXYChanged()
{
	if (!m_paper)
	{
		return;
	}

	mlPoint pnt = mpMLMedia->GetIVisible()->GetAbsXY();
	m_paper->SetCoords(pnt.x, pnt.y);
} 

void nrmImageFrame::SizeChanged()
{
	if (!m_paper)	
	{
		return;
	}

	mlSize sz = mpMLMedia->GetIVisible()->GetSize();
	m_paper->SetSize(sz.width, sz.height);
}

// изменилась абсолютная глубина
void nrmImageFrame::AbsDepthChanged()
{
	if (!m_paper)	
	{
		return;
	}

	m_paper->SetDepth(GetMLMedia()->GetIVisible()->GetAbsDepth());
}

// изменилась абсолютная видимость
void nrmImageFrame::AbsVisibleChanged()
{
	if (!m_paper)
	{
		return;
	}

#ifndef USE_VISIBLE_DESTINATION
	m_paper->SetVisible(GetMLMedia()->GetIVisible()->GetAbsVisible());
#else
	m_paper->SetVisible(GetMLMedia()->GetIVisible()->GetAbsVisible(), 3);
#endif
}

mlSize nrmImageFrame::GetMediaSize()
{
	mlSize size; ML_INIT_SIZE(size);

	if (m_paper)
	{
		if (m_paper->GetTextureID(0) < 0) 
		{
			return size;
		}

		CTexture* texture = g->tm.GetObjectPointer(m_paper->GetTextureID(0));
		size.width = texture->GetTextureWidth();
		size.height = texture->GetTextureHeight();
	}

	return size;
}

void nrmImageFrame::AbsEnabledChanged()
{
	// do nothing
}

void nrmImageFrame::UpdateMask( nrmImage* apMask)
{
	if( !m_paper)
		return;

	if (!GetMask())
	{
		m_paper->SetMaskSprite(NULL);
		return;
}

	if (GetMask()->GetType() != MLMT_IMAGE)
		return;

	nrmImage* img = (nrmImage *)GetMask()->GetSafeMO();
	m_paper->SetMaskSprite(img->m_sprite);
}

void nrmImageFrame::SetMask(int aiX, int aiY, int aiW, int aiH)
{
	if( !m_paper)
		return;
	m_paper->SetScissors(aiX, aiY, aiW, aiH);
}

int nrmImageFrame::IsPointIn(const mlPoint aPnt) // -1 - not handled, 0 - point is out, 1 - point is in
{	
	if (!m_paper)	
	{
		return 0;
	}

	return (!m_paper->IsPixelTransparent(aPnt.x, aPnt.y)) ? 1 : 0;
}

void nrmImageFrame::Crop(short awX, short awY, MLWORD awWidth, MLWORD awHeight)
{
	if (!m_paper)
	{
		return;
	}

	if (awWidth > 65000)
	{
		m_paper->SetFullWorkRect();
		return;
	}

	m_paper->SetWorkRect(awX, awY, awX + awWidth, awY + awHeight);

	mlSize size;
	size.width = awWidth;
	size.height = awHeight;
	if (mpMLMedia)
	{
		mpMLMedia->GetIVisible()->SetSize(size);
	}
}


mlRect nrmImageFrame::GetCropRect()
{ 
	mlRect rc; 
	ML_INIT_RECT(rc); 

	return rc; 
}

void nrmImageFrame::AbsOpacityChanged() // изменилась абсолютная видимость
{
	if (m_paper)
	{
		float op = mpMLMedia->GetIVisible()->GetAbsOpacity();
		m_paper->SetAlpha((unsigned char)(op * 255.0f));
	}
}

void nrmImageFrame::OnChanged(int /*eventID*/)
{
}
	 
void nrmImageFrame::MarginsChanged()
{
	if (m_paper)
	{
		int marginTop;
		int marginBottom;
		int marginLeft;
		int marginRight;
		mpMLMedia->GetIImageFrame()->GetMargins( marginTop, marginBottom, marginLeft, marginRight);
		if (marginTop == ML_UNDEF_MARGIN_VALUE)
		{
			marginTop = marginBottom = marginLeft = marginRight = 1; // или какое-то другое значение по умолчанию
		}

		m_paper->SetMargins(marginTop, marginLeft, marginBottom, marginRight);
	}
}

void nrmImageFrame::TileChanged()
{
	mlIImageFrame* pmoImageFrame = GetMLMedia()->GetIImageFrame();
	if (pmoImageFrame)
	{
		bool tile = pmoImageFrame->GetTile();
		m_paper->SetTiling(tile);
	}
}

nrmImageFrame::~nrmImageFrame()
{
	if (m_paper)
	{
		m_paper->DeleteChangesListener(this);
		gRM->scene2d->DeleteSprite(m_paper->GetName());
		g->sprm.DeleteObject(m_paper->GetID());
	}
}