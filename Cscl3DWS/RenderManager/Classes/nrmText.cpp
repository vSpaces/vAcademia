// nrmText.cpp: implementation of the nrmText class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "nrmText.h"
#include "nrmImage.h"

#include "GlobalSingletonStorage.h"
#include "RMContext.h"
#include "../../mdump/mdump.h"

nrmText::nrmText(mlMedia* apMLMedia): 
	nrmObject(apMLMedia)
{
	m_text = MP_NEW(CFormattedTextSprite);
	m_text->SetVerticalAlign(FONT_VALIGN_TOP);
	gRM->scene2d->AddTextSprite(m_text, 0, 0);
//	m_text->SetVisibilityProvider(this);
#ifndef USE_VISIBLE_DESTINATION
	m_text->SetVisible(false);
#else
	m_text->SetVisible(false, 6);
#endif
	m_text->SetAntialiasing(true);
}

void nrmText::BkgColorChanged()
{
	mlMedia*	apMLMedia = GetMLMedia();
	if (apMLMedia)
	{
		if (m_text)
		{
			mlColor color = GetMLMedia()->GetIText()->GetBkgColor();
			m_text->SetBkgColor(color.r, color.g, color.b, color.a);
		}
	}
}

bool	nrmText::AreWordBreaksExists()
{
	if (m_text)
	{
		return m_text->AreWordBreaksExists();
	}
	else
	{
		return false;
	}
}

void	nrmText::HtmlChanged()
{
	mlMedia*	apMLMedia = GetMLMedia();
	if (apMLMedia)
	{
		if (m_text)
		{
			bool isTagsPermitted = apMLMedia->GetIText()->IsHtml();
			if (m_text->IsTagsPermitted() != isTagsPermitted)
			{
				m_text->PermitTags(isTagsPermitted);

				ReformatText();
			}
		}
	}
}

void	nrmText::OnSetRenderManager()
{
	assert(m_pRenderManager);
	mlMedia*	apMLMedia = GetMLMedia();

	if (!apMLMedia)
	{
		return;
	}

	mlStyle*	pStyle = apMLMedia->GetIText()->GetStyle();
	if (!pStyle)
	{
		pStyle = apMLMedia->GetIText()->GetStyle();
	}
	else
	{
		switch (pStyle->textAlign)
		{
		case pStyle->TA_left:
			m_text->SetHorizontalAlign(ALIGN_LEFT);
			break;

		case pStyle->TA_center:
			m_text->SetHorizontalAlign(ALIGN_CENTER);
			break;

		case pStyle->TA_right:
			m_text->SetHorizontalAlign(ALIGN_RIGHT);
			break;
		}
		
		m_text->SetItalicStyle((pStyle->fontStyle == mlStyle::FS_italic));
		m_text->SetBoldStyle((pStyle->fontWeight == mlStyle::FW_bold));
		if (pStyle->fontFamily)
		{			
			m_text->SetFont(pStyle->fontFamily);
		}
		m_text->SetFontSize(pStyle->fontSize.val);
		if(pStyle->antialiasing == mlStyle::AA_smooth)
			m_text->SetAntialiasing(true);
		else if(pStyle->antialiasing == mlStyle::AA_none)
			m_text->SetAntialiasing(false);
		
		//mlStyle*	pStyle = apMLMedia->GetIText()->GetStyle();

		if( pStyle->lineHeight.val != -1)
		{

			//assert(FALSE);
			//pitext->SetLineHeight(pStyle->lineHeight.val);
			//added 10.10.2008 nikitin
			m_text->SetLineHeight(pStyle->lineHeight.val);
		}
		if( pStyle->color.def)
		{
			m_text->SetColor(pStyle->color.r, pStyle->color.g, pStyle->color.b, 255);
			AbsOpacityChanged();
		}
	}
	mlMedia* pMLMParent=apMLMedia->GetParentElement();
	if (pMLMParent && pMLMParent->mType==MLMT_CLOUD) 
	{
		return;
	}
	AbsDepthChanged();
	AbsXYChanged();
	SizeChanged();
	ValueChanged();
	BkgColorChanged();
	HtmlChanged();
}

// загрузка нового ресурса
bool nrmText::SrcChanged()
{
	CWComString name = L"";
	if (mpMLMedia)
	if (mpMLMedia->GetStringProp("name"))
	{
		name = mpMLMedia->GetStringProp("name");
	}
	m_text->SetName(name.GetBuffer(0));

	AbsDepthChanged();
	AbsVisibleChanged();
	AbsEnabledChanged();
	return true;
}

// изменилось абсолютное положение 
void nrmText::AbsXYChanged()
{
	if (mpMLMedia)
	{
		mlPoint pnt = mpMLMedia->GetIVisible()->GetAbsXY();
		m_text->SetCoords(pnt.x, pnt.y);
	}
} 

void nrmText::SizeChanged()
{
	assert(m_text);

	int w, h;
	m_text->GetSize(w, h);

	if (mpMLMedia)
	{
		mlSize sz = mpMLMedia->GetIText()->GetFormatSize();
		m_text->SetSize(sz.width, sz.height);

		if (w != sz.width || h != sz.height)
		{
			ReformatText();
		}
	}
}

// изменилась абсолютная глубина
void nrmText::AbsDepthChanged()
{
	if (GetMLMedia())
	{
		m_text->SetDepth(GetMLMedia()->GetIVisible()->GetAbsDepth());
	}
}

// изменилась абсолютная видимость
void nrmText::AbsVisibleChanged()
{
	if (GetMLMedia())
	{
#ifndef USE_VISIBLE_DESTINATION
		m_text->SetVisible(GetMLMedia()->GetIVisible()->GetAbsVisible());
#else
		m_text->SetVisible(GetMLMedia()->GetIVisible()->GetAbsVisible(), 7);
#endif
	}
}

void nrmText::AbsEnabledChanged()
{
}

mlSize nrmText::GetMediaSize()
{
	mlSize size; 
	ML_INIT_SIZE(size);
	size.width = m_text->GetRealWidth();
	size.height = m_text->GetRealHeight();
	return size;
}

// изменился текст
void nrmText::ValueChanged()
{
	ReformatText();
}

// переформатирование текста
void nrmText::ReformatText()
{
	if (!GetMLMedia())
	{
		return;
	}

	mlIText* pmoText = GetMLMedia()->GetIText();
	const wchar_t* pwcValue = pmoText->GetValue();
	
	const wchar_t* pwcTail = pmoText->GetSliceTail();
	if (!pwcValue) 
	{
		return;
	}

	/*if( pwcValue && wcscmp(pwcValue,L"testdump") == 0)
		wcscpy( (wchar_t*)pwcTail, (wchar_t*)pwcValue);*/
		//((wchar_t*)pwcTail)[0] = L'A';

	/*if( pwcValue && wcsstr(pwcValue,L"***") != NULL)
		int ii = 0;*/

	if (pwcTail)
	{
		m_text->SetDotsText(pwcTail);
	}
	else
	{
		m_text->SetDotsText(L"");
	}

	m_text->SetText(pwcValue);	
}

void nrmText::StylesChanged()
{
	mlStyle*	pStyle = GetMLMedia()->GetIText()->GetStyle();
	if (pStyle)
	{
		switch (pStyle->textAlign)
		{
		case pStyle->TA_left:
			m_text->SetHorizontalAlign(ALIGN_LEFT);
			break;

		case pStyle->TA_center:
			m_text->SetHorizontalAlign(ALIGN_CENTER);
			break;

		case pStyle->TA_right:
			m_text->SetHorizontalAlign(ALIGN_RIGHT);
			break;
		}

		if(pStyle->antialiasing == mlStyle::AA_smooth)
			m_text->SetAntialiasing(true);
		else if(pStyle->antialiasing == mlStyle::AA_none)
			m_text->SetAntialiasing(false);
		
		m_text->SetItalicStyle((pStyle->fontStyle == mlStyle::FS_italic));
		m_text->SetBoldStyle((pStyle->fontWeight == mlStyle::FW_bold));
		m_text->SetUnderlineStyle((pStyle->textDecoration == mlStyle::TD_underline));
		if( pStyle->lineHeight.val != -1)
		{
			//assert(false);
			m_text->SetLineHeight(pStyle->lineHeight.val);
		}
		if (pStyle->color.def)
		{
            m_text->SetColor(pStyle->color.r, pStyle->color.g, pStyle->color.b, 255);
			AbsOpacityChanged();
		}

		if (pStyle->fontFamily)
		{			
			m_text->SetFont(pStyle->fontFamily);
		}

		m_text->SetFontSize(pStyle->fontSize.val);		
	}

	ReformatText();
}

void nrmText::AbsOpacityChanged() // изменилась абсолютная видимость
{
	if ((m_text) && (mpMLMedia))
	{
		float op = mpMLMedia->GetIVisible()->GetAbsOpacity();		
		unsigned char r, g, b, a;
		m_text->GetColor(r, g, b, a);
		a = (unsigned char)(op * 255.0f);
		m_text->SetColor(r, g, b, a);
	}
}

void nrmText::UpdateMask( nrmImage* /*apMask*/)
{
	if( !m_text)
		return;

	if (!GetMask())
	{
		m_text->SetMaskSprite(NULL);
		return;
}

	if (GetMask()->GetType() != MLMT_IMAGE)
		return;

	nrmImage* img = (nrmImage *)GetMask()->GetSafeMO();
	m_text->SetMaskSprite(img->m_sprite);
}

void nrmText::SetMask(int aiX, int aiY, int aiW, int aiH)
{
	m_text->SetScissors(aiX, aiY, aiW, aiH);
}

bool nrmText::GetTextFormattingInfo(mlSynchData& /*aData*/)
{
	// ??
	return false;
}

void nrmText::AddText(wchar_t* str)
{
	//USES_CONVERSION;
	//m_text->AddText(W2A(str));
	m_text->AddText(str);
}

int nrmText::GetCursorX()
{
	return m_text->GetCursorX();
}

int nrmText::GetCursorY()
{
	return m_text->GetCursorY();
}

int nrmText::GetCursorPos()
{
	return m_text->GetCursorPos();
}

int nrmText::GetCursorHeight()
{
	return m_text->GetCursorHeight();
}

void nrmText::SetSelection(int from, int to)
{
	m_text->SetSelection(from, to);
}

void nrmText::SetCursorPos(int pos)
{
	m_text->SetCursorPos(pos);
}

void nrmText::IncreaseCursor(unsigned int delta)
{
	if (delta > 1)
	{
		m_text->IncreaseCursor(delta);
	}
	else
	{
		m_text->IncreaseCursor();
	}
}

void nrmText::DecreaseCursor(unsigned int delta)
{
	if (delta > 1)
	{
		m_text->DecreaseCursor(delta);
	}
	else
	{
		m_text->DecreaseCursor();
	}
}

void nrmText::GetVisibleBounds(int& _begin, int& _end)
{
	m_text->GetVisibleBounds(_begin,_end);
}

void nrmText::ToClipboard(wchar_t* str)
{	
	if (0 == wcslen(str))
	{
		return;
	}
	
	if(OpenClipboard(g->ne.ghWnd))
	{
		HGLOBAL hgBuffer;
		EmptyClipboard();
		hgBuffer = GlobalAlloc(GHND, wcslen(str)*2+2);
		if (hgBuffer)	
		{
			wcscpy((wchar_t*)GlobalLock(hgBuffer), str);
			GlobalUnlock(hgBuffer);
			SetClipboardData(CF_UNICODETEXT, hgBuffer);
		}
		CloseClipboard();
	}
}

std::wstring nrmText::FromClipboard()
{
	bool isMultiline = false;
	if (GetMLMedia())
	if (GetMLMedia()->GetBooleanProp("multiline") == true)
	{
		isMultiline = true;
	}

	std::wstring result;
	const int maxBufferSize = 8192;

	HGLOBAL hData;
	if ( IsClipboardFormatAvailable( CF_UNICODETEXT))
	{
		if ( OpenClipboard( g->ne.ghWnd))
		{
			hData = GetClipboardData( CF_UNICODETEXT);
			wchar_t* chBuffer = (wchar_t*)GlobalLock(hData);
			wchar_t* tmp = NULL;
			if (chBuffer)
			{			
				int stringSize = wcslen(chBuffer);
				if (stringSize > maxBufferSize)
				{
					stringSize = maxBufferSize;
				}
				tmp = MP_NEW_ARR(wchar_t, (stringSize + 1) * sizeof(wchar_t));
				memcpy(tmp, chBuffer, stringSize * sizeof(wchar_t));
				tmp[stringSize] = 0;

				result = tmp;

				MP_DELETE_ARR(tmp);
			}
			GlobalUnlock(hData);
			CloseClipboard();

			if (!isMultiline)
			for ( unsigned int i = 0; i < result.size(); i++)
			{
				if (( result[ i] == 9) || ( result[ i] == 13) || ( result[ i] == 10))
				{
					result[i] = L' ';
				}
			}

			return result;
		}
		else
		{
			return L"";
		}
	}
	else
	{
		return L"";
	}
}

bool nrmText::GetSliced()
{
	if ( m_text)
	{
		return m_text->GetSliced();
	}
	return false;
}

int nrmText::IsPointIn(const mlPoint aPnt) // -1 - not handled, 0 - point is out, 1 - point is in
{	
	if (!m_text)	
	{
		return 0;
	}

	return (!m_text->IsPixelTransparent(aPnt.x, aPnt.y)) ? 1 : 0;
}

int nrmText::GetSymbolIDByXY(int x, int y)
{
	if (!m_text)	
	{
		return -1;
	}

	return m_text->GetSymbolIDByXY(x, y);
}

nrmText::~nrmText()
{
	gRM->scene2d->DeleteTextSprite(m_text);

	if (m_text)	
	{
		MP_DELETE(m_text);
	}
}