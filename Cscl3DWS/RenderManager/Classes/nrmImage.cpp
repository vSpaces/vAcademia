// nrmImage.cpp: implementation of the nrmImage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "nrmText.h"
#include "nrmAnim.h"
#include <algorithm>
#include "nrmVideo.h"
#include "nrmImage.h"
#include "nrmPlugin.h"
#include "LastScreen.h"
#include "nrmViewport.h"
#include "nrm3DObject.h"
#include "RenderTarget.h"
#include "nrmImageFrame.h"
#include "HelperFunctions.h"
#include "MouseController.h"
#include "PlatformDependent.h"
#include "WhiteboardManager.h"

#include "GlobalSingletonStorage.h"
#include "RMContext.h"
#include "SnapshotMaker.h"

#define MIN_SYNCH_TIME	500
#define RESIZE			64

nrmImage::nrmImage(mlMedia* apMLMedia) : 
	m_isBinaryStateReceived(false),
	m_loadingPriority(255),
	m_isOnLoadSended(false),
	m_isNeedToClear(false),
	m_isFirstLoaded(true),
	MP_WSTRING_INIT(m_src),
	nrmObject(apMLMedia),
	m_editableNow(false),
	m_lastUpdateTime(0),
	m_editable(false),
	m_palette(NULL),
	m_sharingController(NULL),	
	m_sprite(NULL),
	m_version(0),	
	MP_VECTOR_INIT(v_IPadImage)
{
}

void nrmImage::CreateSpriteIfNeeded(std::wstring& src)
{
	if (!m_sprite)
	{
		std::wstring name;
		if (mpMLMedia)
		if (mpMLMedia->GetStringProp("name"))
		{
			USES_CONVERSION;
			name = mpMLMedia->GetStringProp("name");
		}

		name += L"_";
		name += src;
		name += L"_";
		name += IntToWStr(g->sprm.GetCount()).c_str();

		int spriteID = g->sprm.AddObject(name);
		m_sprite = g->sprm.GetObjectPointer(spriteID);
		m_sprite->AddChangesListener(this);
		gRM->scene2d->AddSprite(spriteID, 0, 0);
//		m_sprite->SetVisibilityProvider(this);
	}
}

bool nrmImage::IsLoaded()
{
	bool isLoaded = false;

	CTexture* texture = NULL;
	if (-1 == m_sprite->GetTextureID(0))
	{
		isLoaded = m_sprite->IsLoaded();
	}
	else
	{
		texture = g->tm.GetObjectPointer(m_sprite->GetTextureID(0));
		isLoaded = (texture->GetLoadedStatus() == TEXTURE_LOADED);
	}

	return isLoaded;
}

void nrmImage::OnAfterLoad()
{
	ScanPlugins();
	InitializePlugins();
	AbsDepthChanged();
	AbsVisibleChanged();
	AbsEnabledChanged();
	AbsOpacityChanged();
	ColorChanged();
	TileChanged(); 
	SmoothChanged();
	CropChanged();
	AbsXYChanged();

	if (GetMLMedia()->GetBooleanProp("permitBlend"))
	{
		m_sprite->SetPermitTransparent(true);
	}

	if (m_loadingPriority != 255)
	{
		LoadingPriorityChanged((rmml::ELoadingPriority)m_loadingPriority);
	}
}

// загрузка нового ресурса
bool nrmImage::SrcChanged()
{
	USES_CONVERSION;

	if (!mpMLMedia)
	{
		return false;
	}

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
	
	std::wstring src = CharLowerW( (LPWSTR)pwcSrc);

	if (m_src == src)
	{
		GetMLMedia()->GetILoadable()->onLoad( RMML_LOAD_STATE_SRC_LOADED);
		return true;
	}

	const wchar_t* pwcName = mpMLMedia->GetStringProp("name");
	std::string name;
	if (pwcName)
	{
		name = CharLower(W2A(pwcName));
	}

	bool isMipMap = false;
	
	CreateSpriteIfNeeded(src);

	if (src != m_src)
	{		
		m_src = src;
	}

	m_isOnLoadSended = false;

	CompressedChanged(); 
	AngleChanged();
	MaxDecreaseKoefChanged();
	
	bool isOnlyLoad = false;
	SColorKey colorKey;
	colorKey.type = KEY_COLOR_NOT_USED;

	if (GetMLMedia()->GetBooleanProp("enableKeyColor"))
	{
		colorKey.type = KEY_COLOR_FROM_OPTIONS;
		colorKey.r = GetMLMedia()->GetIntProp("colorR");
		colorKey.g = GetMLMedia()->GetIntProp("colorG");
		colorKey.b = GetMLMedia()->GetIntProp("colorB");
	}

	if (GetMLMedia()->GetBooleanProp("enableKeyColorFromImage"))
	{
		colorKey.type = KEY_COLOR_FROM_IMAGE;
	}

	if (GetMLMedia()->GetBooleanProp("onlyload"))
	{
		isOnlyLoad = true;
	}

	if (GetMLMedia()->GetBooleanProp("for3d"))
	{
		isMipMap = true;
	}

	if (src == L":last_screen")
	{
		std::wstring path = gRM->lastScreen->GetLastScreenPathForLoading();
		USES_CONVERSION;
		m_sprite->LoadFromTexture( path);
	}
	else if (src == L":kinect")
	{
		if (g->phom.GetControlledObject())
		{
			if (g->phom.GetControlledObject()->GetObject3D())
			{
				if (g->phom.GetControlledObject()->GetObject3D()->GetAnimation())
				{
					CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)g->phom.GetControlledObject()->GetObject3D()->GetAnimation();					
					m_sprite->LoadFromDynamicTexture(sao->GetVideoDynamicTexture());
					int textureID = m_sprite->GetTextureID(0);
					CTexture* texture = g->tm.GetObjectPointer(textureID);
					texture->SetYInvertStatus(false);
					m_sprite->SetWorkRect(0, 0, 640, 480);
				}
			}
		}
	}
	else if (src != L":scripts\\blank")
	{
		std::wstring path = CorrectPath(src);
		m_sprite->LoadFromTexture(path, isOnlyLoad, isMipMap, &colorKey);	
	}

	bool isLoaded = IsLoaded();
		
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
		// затычка для простановки размеров по завершению загрузки картинки по хттп
		if (!(!isLoaded && sz.height == 2 && sz.width == 2))
			mpMLMedia->GetIVisible()->SetSize(sz);
		mlPoint pnt = mpMLMedia->GetIVisible()->GetAbsXY();
		m_sprite->SetCoords(pnt.x, pnt.y);
	}

	OnAfterLoad();

	if (isLoaded)
	{
		m_isFirstLoaded = false;
		if (!m_isOnLoadSended)
		{
			GetMLMedia()->GetILoadable()->onLoad( RMML_LOAD_STATE_SRC_LOADED);
			m_isOnLoadSended = true;
		}
	}

	return true;
}

bool nrmImage::Reload()
{
	USES_CONVERSION;

	if (!mpMLMedia)
	{
		return false;
	}

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

	std::string src = CharLower(W2A(pwcSrc));

	int textureID = m_sprite->GetTextureID(0);
	g->tm.DeleteObject(textureID);
	bool res = SrcChanged();

	CTexture* texture = g->tm.GetObjectPointer(m_sprite->GetTextureID(0));
	texture->AddRef();

	std::vector<CSprite *>::iterator it = g->sprm.GetLiveObjectsBegin();
	std::vector<CSprite *>::iterator itEnd = g->sprm.GetLiveObjectsEnd();

	for ( ; it != itEnd; it++) 
	{
		if ((*it)->GetTextureID(0) == textureID)
		{
			(*it)->SetTextureID(m_sprite->GetTextureID(0));
			texture->AddRef();
		}
	}
	return res;
}

// смена приоритета загрузки ресурса
void nrmImage::LoadingPriorityChanged( ELoadingPriority aPriority)
{
	SetLoadingPriority(aPriority);
}

void nrmImage::SetLoadingPriority(unsigned char aPriority)
{
	m_loadingPriority = aPriority;
	
	if (m_src.size() > 0)
	{
		USES_CONVERSION;
		IAsynchResource* res = g->rl->GetAsynchResMan()->FindAsynchResource( W2A(m_src.c_str()));
		if (res)
		{
			res->SetScriptPriority(aPriority);
		}	 
	}
}

// изменилось абсолютное положение 
void nrmImage::AbsXYChanged()
{
	if (!m_sprite)
	{
		return;
	}

	if (mpMLMedia)
	{
		mlPoint pnt = mpMLMedia->GetIVisible()->GetAbsXY();
		m_sprite->SetCoords(pnt.x, pnt.y);
	}
} 

void nrmImage::SizeChanged()
{
	if (!m_sprite)	
	{
		return;
	}

	if (mpMLMedia)
	{
		mlSize sz = mpMLMedia->GetIVisible()->GetSize();
		m_sprite->SetSize(sz.width, sz.height);
	}
}

// изменилась абсолютная глубина
void nrmImage::AbsDepthChanged()
{
	if (!m_sprite)	
	{
		return;
	}

	m_sprite->SetDepth(GetMLMedia()->GetIVisible()->GetAbsDepth());
}

// изменилась абсолютная видимость
void nrmImage::AbsVisibleChanged()
{
	if (!m_sprite)
	{
		return;
	}

	if (GetMLMedia())
	{
		bool isVisible = GetMLMedia()->GetIVisible()->GetAbsVisible();
#ifndef USE_VISIBLE_DESTINATION
		m_sprite->SetVisible(isVisible);
#else
		m_sprite->SetVisible(isVisible, 8);
#endif		
	}
}

mlSize nrmImage::GetMediaSize()
{
	mlSize size; ML_INIT_SIZE(size);

	if (m_sprite)
	{
		if (m_sprite->GetTextureID(0) < 0) 
		{
			return size;
		}

		CTexture* texture = g->tm.GetObjectPointer(m_sprite->GetTextureID(0));
		size.width = texture->GetTextureWidth();
		size.height = texture->GetTextureHeight();

		if (m_sprite->GetFrameCount() > 0)
		{
			size.width /= m_sprite->GetFrameCount();
		}
	}

	return size;
}

void nrmImage::AbsEnabledChanged()
{
	// do nothing
}

void nrmImage::UpdateMask( nrmImage* apMask)
{
	if( !m_sprite)
		return;

	if (!GetMask())
	{
		m_sprite->SetMaskSprite(NULL);
		return;
	}

	if (GetMask()->GetType() != MLMT_IMAGE)
		return;

	nrmImage* img = (nrmImage *)GetMask()->GetSafeMO();
	m_sprite->SetMaskSprite(img->m_sprite);
}

void nrmImage::SetMask(int aiX, int aiY, int aiW, int aiH)
{
	if (!m_sprite)
	{
		return;
	}

	m_sprite->SetScissors(aiX, aiY, aiW, aiH);
}

int nrmImage::IsPointIn(const mlPoint aPnt) // -1 - not handled, 0 - point is out, 1 - point is in
{	
	if (!m_sprite)	
	{
		return 0;
	}

	return (!m_sprite->IsPixelTransparent(aPnt.x, aPnt.y)) ? 1 : 0;	
}

static int  count;

bool nrmImage::MakeSnapshot(mlMedia** appVisibles, const mlRect /*aCropRect*/)
{
	if (!mpMLMedia)
	{
		return false;
	}

	SSnapshotParams params;
	USES_CONVERSION;
	if (mpMLMedia->GetStringProp("name"))
	{
		params.imageName = W2A(mpMLMedia->GetStringProp("name"));
	}

	if (GetMLMedia()->GetStringProp("fileName"))
	{
		USES_CONVERSION;
		std::string fileName = W2A(GetMLMedia()->GetStringProp("fileName"));
		int pos = fileName.find("{tmp}");
		if (pos != -1)
		{
			char szTempDir[ MAX_PATH * 2];
			char tmpName[ MAX_PATH * 2];
			::GetTempPath( sizeof(szTempDir)/sizeof(szTempDir[0]), szTempDir);
			::GetTempFileName( szTempDir, "tmp", 0, tmpName);
			fileName = tmpName + fileName.substr(pos + 5, fileName.length() - 5 - pos);
			GetMLMedia()->SetProp("fileName", fileName.c_str());
		}
		params.fileName = fileName;
	}

	params.toSystemMemory = GetMLMedia()->GetBooleanProp("toSystemMemory");
	params.isPostEffectEnabled = GetMLMedia()->GetBooleanProp("posteffect");
	params.isMiniMap = GetMLMedia()->GetBooleanProp("minimap");
	params.isNoDynamicObjects = GetMLMedia()->GetBooleanProp("noDynamic");
	params.isNo2D = GetMLMedia()->GetBooleanProp("no2d");
	params.isOnly2D = GetMLMedia()->GetBooleanProp("only2d");
	params.useBackgroundColor = GetMLMedia()->GetBooleanProp("useBackgroundColor");
	params.colorR = (float)GetMLMedia()->GetIntProp("backgroundR") / 255.0f;
	params.colorG = (float)GetMLMedia()->GetIntProp("backgroundG") / 255.0f;
	params.colorB = (float)GetMLMedia()->GetIntProp("backgroundB") / 255.0f;
	params.isDisableTransparent = GetMLMedia()->GetBooleanProp("disable_transparent");
	params.isShadowsDisabled = GetMLMedia()->GetBooleanProp("shadowsDisabled");

	bool res = MakeSnapshotImpl(appVisibles, m_sprite, params);
	if (res)
	{
		OnAfterLoad();
	}	
	g->lw.WriteLn("Make snapshot res = ", res ? 1 : 0, " isIconicAndIntel ", g->ne.isIconicAndIntel);
	return res;
}

void nrmImage::Crop(short awX, short awY, MLWORD awWidth, MLWORD awHeight)
{
	if (!m_sprite)
	{
		return;
	}

	if (awWidth > 65000)
	{
		m_sprite->SetFullWorkRect();
		return;
	}

	m_sprite->SetWorkRect(awX, awY, awX + awWidth, awY + awHeight);
	mlSize	size;
	size.width = awWidth;
	size.height = awHeight;
	if (mpMLMedia)
	{
		mpMLMedia->GetIVisible()->SetSize(size);
	}
}

mlRect nrmImage::GetCropRect()
{ 
	mlRect rc; 
	ML_INIT_RECT(rc); 

	if (!m_sprite)
	{
		return rc;
	}

	m_sprite->GetWorkRect(&rc.left, &rc.top, &rc.right, &rc.bottom);

	return rc; 
}

BOOL	nrmImage::SaveMedia(const wchar_t* apwcPath)
{
	USES_CONVERSION;
	if (m_sprite)
	{
		int textureID = m_sprite->GetTextureID(0);
		if (textureID != -1)
		{
			CTexture* texture = g->tm.GetObjectPointer(textureID);

			if ((texture->IsEnabled()) && (!texture->IsDeleted()))
			{
				std::wstring path = apwcPath;
				g->tm.SaveTexture(texture, path);
				return TRUE;
			}
		}
	}

	return FALSE;
}

void nrmImage::AbsOpacityChanged() // изменилась абсолютная видимость
{
	if ((m_sprite) && (mpMLMedia))
	{
		float op = mpMLMedia->GetIVisible()->GetAbsOpacity();
		m_sprite->SetAlpha((unsigned char)(op * 255.0f));
	}
}

int	nrmImage::GetTextureID()
{
	if (!m_sprite)	
	{
		return -1;
	}

	return m_sprite->GetTextureID(0);
}

void nrmImage::OnChanged(int /*eventID*/)
{
	if (!mpMLMedia)
	{
		return;
	}
	
	mlSize sz = mpMLMedia->GetIVisible()->GetSize();
	mlPoint pnt = mpMLMedia->GetIVisible()->GetAbsXY();

	bool isLoaded = false;

	if (m_sprite)
	{
		isLoaded = IsLoaded();

		if (isLoaded)
		{
			CTexture* texture = NULL;
			int textureID = m_sprite->GetTextureID(0);
			if (textureID != -1)
			{
				texture = g->tm.GetObjectPointer(textureID);
			}

			if (!m_sprite->IsSizeDefined())
			{
				if ((2 == sz.width) && (2 == sz.height) || (0 == sz.width) && (0 == sz.height))
				{
					if (texture)
					{
						sz.width = texture->GetTextureWidth();
						sz.height = texture->GetTextureHeight();
					}
				}
			}
			else
			{
				m_sprite->GetSize(sz.width, sz.height);
			}
				
			mpMLMedia->GetIVisible()->SetSize(sz);
			m_sprite->SetCoords(pnt.x, pnt.y);
		}
	}
	
	if (isLoaded)
	{
		AbsVisibleChanged();
		AbsEnabledChanged();
		AbsOpacityChanged();
		AbsDepthChanged();
		SizeChanged();
		AbsXYChanged();
		m_isFirstLoaded = false;

		if (!m_isOnLoadSended)
		{
			GetMLMedia()->GetILoadable()->onLoad(RMML_LOAD_STATE_SRC_LOADED);
			m_isOnLoadSended = true;
		}
	}
	else
	{
		if (!m_isOnLoadSended)
		{
const wchar_t* src = GetMLMedia()->GetILoadable()->GetSrc();
ATLTRACE("RM::load error for %S \n", src);


			GetMLMedia()->GetILoadable()->onLoad(RMML_LOAD_STATE_LOADERROR);
			m_isOnLoadSended = true;
		}
	}
}

// setCanvasSize(w,h) - устанавливает размер текстуры (свойства width и height, т.е. отображаемый размер картинки не меняет)
void	nrmImage::SetCanvasSize(int /*w*/, int /*h*/)
{
}

void nrmImage::DrawLine(int /*aiX1*/, int /*aiY1*/, int /*aiX2*/, int /*aiY2*/, const mlColor /*color*/, int /*aiWeight*/)
{
}

void nrmImage::FillRectangle(int /*aiX1*/, int /*aiY1*/, int /*aiX2*/, int /*aiY2*/, const mlColor /*color*/)
{
}

void nrmImage::DrawEllipse(int /*aiX1*/, int /*aiY1*/, int /*aiX2*/, int /*aiY2*/, const mlColor /*aColor*/, int /*aiWeight*/, const mlColor /*aFillColor*/)
{
}

void nrmImage::DrawRectangle(int /*aiX1*/, int /*aiY1*/, int /*aiX2*/, int /*aiY2*/, const mlColor /*aColor*/, int /*aiWeight*/, const mlColor /*aFillColor*/)
{
}

// получить цвет пиксела
mlColor nrmImage::GetPixel(int aiX, int aiY)
{
	mlColor color; ML_INIT_COLOR(color);
	m_sprite->GetPixelColor(aiX, aiY, color.r, color.g, color.b);
	color.a = 1;
	return color;
}

// сделать себе копию изображения
void nrmImage::DuplicateFrom(mlMedia* apSourceImage)
{
	std::wstring src = L"duplicate";
	CreateSpriteIfNeeded(src);

	nrmImage* img = (nrmImage*)apSourceImage->GetSafeMO();

	if (!img->m_sprite)
	{
		return;
	}

	int textureID = img->m_sprite->GetTextureID(0);
	if (-1 == textureID)
	{
		return;
	}

	CTexture* texture = g->tm.GetObjectPointer(textureID);
	texture->AddOwner(m_sprite);
	texture->AddRef();	

	m_sprite->SetTextureID(textureID);
	
	m_sprite->SetLoadingSpriteSize();
	m_sprite->OnChanged(0);
}

void nrmImage::SetActiveTool(int aToolNumber)
{
	if (m_palette)
	{
		aToolNumber = aToolNumber % 256;
		m_palette->SetActiveToolID((unsigned char)aToolNumber);
	}
}

void nrmImage::SetLineWidth(int aLineWidth)
{
	if (m_palette)
	{
		unsigned char lineWidth = (unsigned char)aLineWidth;
		m_palette->SetCommonParameter(COMMON_WIDTH, (void*)lineWidth);
	}
}

void nrmImage::SetLineDirection(int aDirection)
{
	if (m_palette)
	{
		unsigned char activeTool = m_palette->GetActiveToolID();
		int theChoice = aDirection; // 0 - noArrows 1 - upArrow 2-downArrow
		int theTrue = 1;
		int theFalse = 0;
		if (activeTool == TOOL_LINE_ID)
		{
			switch (theChoice)
			{
			case 0:
				m_palette->SetParameter(LINE_ARROW1, (void*)theFalse);
				m_palette->SetParameter(LINE_ARROW2, (void*)theFalse);
				break;
			case 1:
				m_palette->SetParameter(LINE_ARROW1, (void*)theTrue);
				break;
			case 2:
				m_palette->SetParameter(LINE_ARROW2, (void*)theTrue);
				break;
			case 3:
				m_palette->SetParameter(LINE_ARROW1, (void*)theFalse);
				break;
			case 4:
				m_palette->SetParameter(LINE_ARROW2, (void*)theFalse);
				break;
			}
		}
	}
}

void nrmImage::SetLineColor(const mlColor aColor)
{
	if (m_palette)
	{
		CColor4 color = CColor4(aColor.r, aColor.g, aColor.b, aColor.a);
		unsigned char activeTool = m_palette->GetActiveToolID();
		switch (activeTool)
		{
		case TOOL_PEN_ID:
			m_palette->SetParameter(PEN_COLOR, (void*)(*(unsigned int*)&color));
			break;
		case TOOL_LINE_ID:
			m_palette->SetParameter(LINE_COLOR, (void*)(*(unsigned int*)&color));
			break;
		case TOOL_CIRCLE_ID:
			m_palette->SetParameter(CIRCLE_BACK_COLOR, (void*)(*(unsigned int*)&color));
			break;
		case TOOL_RECTANGLE_ID:
			m_palette->SetParameter(RECTANGLE_BACK_COLOR, (void*)(*(unsigned int*)&color));
			break;
		case TOOL_TEXT_ID:
			m_palette->SetParameter(TEXT_COLOR, (void*)(*(unsigned int*)&color));
			break;
		}
	}
}

void nrmImage::SetBgColor(const mlColor aColor)
{
	if (m_palette)
	{
		CColor4 color = CColor4(aColor.r, aColor.g, aColor.b, aColor.a);
		unsigned char activeTool = m_palette->GetActiveToolID();
		switch (activeTool)
		{
		case TOOL_CIRCLE_ID:
			m_palette->SetParameter(CIRCLE_COLOR, (void*)(*(unsigned int*)&color));
			break;
		case TOOL_RECTANGLE_ID:
			m_palette->SetParameter(RECTANGLE_COLOR, (void*)(*(unsigned int*)&color));
			break;
		}
	}
}

void nrmImage::SetToolOpacity(int aOpacity)
{
	if (m_palette)
	{
		//unsigned char activeTool = m_palette->GetActiveToolID();
		unsigned char opacity = (unsigned char)aOpacity;
		m_palette->SetCommonParameter(COMMON_OPACITY, (void*)opacity);
	}
}

void nrmImage::SetTextFontName(wchar_t* aFontName)
{
	USES_CONVERSION;
	std::string text = W2A(aFontName);
	if (m_palette)
	{
		unsigned char activeTool = m_palette->GetActiveToolID();
		if (activeTool == TOOL_TEXT_ID)
		{
			m_palette->SetParameter(TEXT_FONT_NAME, &text);
		}
	}
}

void nrmImage::SetTextBold(bool isBold)
{
	if (m_palette)
	{
		unsigned char activeTool = m_palette->GetActiveToolID();
		if (activeTool == TOOL_TEXT_ID)
		{
			m_palette->SetParameter(TEXT_BOLD, (void*)isBold);
		}
	}
}

void nrmImage::SetTextItalic(bool isItalic)
{
	if (m_palette)
	{
		unsigned char activeTool = m_palette->GetActiveToolID();
		if (activeTool == TOOL_TEXT_ID)
		{
			m_palette->SetParameter(TEXT_ITALIC, (void*)isItalic);
		}
	}
}

void nrmImage::SetTextUnderline(bool isUnderline)
{
	if (m_palette)
	{
		unsigned char activeTool = m_palette->GetActiveToolID();
		if (activeTool == TOOL_TEXT_ID)
		{
			m_palette->SetParameter(TEXT_UNDERLINE, (void*)isUnderline);
		}
	}
}

void nrmImage::SetTextAntialiasing(bool isAntialiasing)
{
	if (m_palette)
	{
		unsigned char activeTool = m_palette->GetActiveToolID();
		if (activeTool == TOOL_TEXT_ID)
		{
			m_palette->SetParameter(TEXT_ANTIALIASING, (void*)isAntialiasing);
		}
	}
}

void nrmImage::SetToolMode(int aMode)
{
	if (m_palette)
	{
		unsigned char activeTool = m_palette->GetActiveToolID();
		unsigned char mode = (unsigned char)aMode;
		if (activeTool == TOOL_CIRCLE_ID)
		{
			m_palette->SetParameter(CIRCLE_MODE, (void*)mode);
		}
		else if (activeTool == TOOL_RECTANGLE_ID)
		{
			m_palette->SetParameter(RECTANGLE_MODE, (void*)mode);
		}
	}
}

void nrmImage::UndoAction()
{
	if (m_palette)
	{
		m_palette->UndoAction();
	}
}

void nrmImage::PasteText(bool isPasteOnIconClick)
{
	if (m_palette)
	{
		m_palette->PasteText(isPasteOnIconClick);
	}
}

void nrmImage::Resample(int aiWidth, int aiHeight)
{
	if (m_sprite)
	{
		int textureID = m_sprite->GetTextureID(0);
		textureID = g->tm.AddResampledTexture(textureID, aiWidth, aiHeight);
		m_sprite->SetTextureID(textureID);
		m_sprite->OnChanged(0);
	}
}

void nrmImage::ClearOldScreen()
{
}

int nrmImage::GetBinarySize()
{
	if (m_palette)
	{
		return m_palette->GetSynch()->GetAllSynchSize();
	}

	return 0;
}

void nrmImage::EditableChanged()
{
	mlIImage* pmoImage = GetMLMedia()->GetIImage();
	if (pmoImage)
	{
		if ((m_palette) && (!pmoImage->GetEditable()))
		{
			gRM->wbMan->OnDeleteWhiteboard(this);
			m_palette->SetActiveToolID(0);
			Update(1);
			gRM->nrMan->UnregisterController(this);
			gRM->mouseCtrl->RemoveMouseListenerFromLastUsed(m_palette);
			MP_DELETE(m_palette);
			m_palette = NULL;
		}

		m_editable = pmoImage->GetEditable();
	}
}

void nrmImage::EditableNowChanged()
{
	mlIImage* pmoImage = GetMLMedia()->GetIImage();
	if (pmoImage)
	{
		bool isEditableNow = pmoImage->GetEditableNow();

		if (m_palette)
		{
			if( !isEditableNow)
				m_palette->SetIPadDrawing( false);
			m_palette->OnActivate();
			
			if ((!m_editableNow) && (isEditableNow))
			{
				gRM->nrMan->RegisterController(this);
			}

			if ((m_editableNow) && (!isEditableNow))
			{
				m_palette->SetActiveToolID(0);
				m_lastUpdateTime = 0;
				Update(1);
				gRM->nrMan->UnregisterController(this);
			}

			m_palette->GetSynch()->SetMode(isEditableNow ? SYNCH_RECORD : SYNCH_PLAY);
		}
		else if (m_sharingController)
		{
			int textureID = m_sprite->GetTextureID(0);
			CTexture* texture = g->tm.GetObjectPointer(textureID);
			m_sharingController->SetDinamicTexture(texture->GetDynamicTexture());
			//m_sharingController->UpdateActiveTarget();

			m_sharingController->SetNeedEvent(isEditableNow ? true : false);
			if ((!m_editableNow) && (isEditableNow))
			{
				gRM->nrMan->RegisterController(this);
			}

			if ((m_editableNow) && (!isEditableNow))
			{				
				gRM->nrMan->UnregisterController(this);
			}
		}
		
		m_editableNow = isEditableNow;
	}
}

void nrmImage::ColorChanged()
{
	mlIImage* pmoImage = GetMLMedia()->GetIImage();
	if (pmoImage)
	{
		mlColor color = pmoImage->GetColor();

		if (m_sprite)
		{
			m_sprite->SetColor(color.r, color.g, color.b);
		}
	}
}

void nrmImage::TileChanged()
{
	mlIImage* pmoImage = GetMLMedia()->GetIImage();
	if (pmoImage)
	{
		bool tile = pmoImage->GetTile();
		if (m_sprite)
			m_sprite->SetTiling(tile);
	}
}

void nrmImage::SmoothChanged()
{
	mlIImage* pmoImage = GetMLMedia()->GetIImage();
	if (pmoImage)
	{
		bool smooth = pmoImage->GetSmooth();
		if (m_sprite)
			m_sprite->SetSmooth(smooth);
	}
}

void nrmImage::CropChanged()
{
	mlIImage* pmoImage = GetMLMedia()->GetIImage();
	if (pmoImage)
	{
		mlBounds bnd = pmoImage->GetCrop();
		if ((bnd.width != 0) && (bnd.height != 0))
		{
			Crop((unsigned short)bnd.x, (unsigned short)bnd.y, (short)bnd.width, (short)bnd.height);
		}
	}
}

void nrmImage::CompressedChanged()
{
	mlIImage* pmoImage = GetMLMedia()->GetIImage();
	if (pmoImage)
	{
		bool compressed = pmoImage->GetCompressed();
		if (m_sprite)
		{
			m_sprite->SetCanBeCompressed(compressed);
		}
	}
}

void nrmImage::AngleChanged()
{
	mlIImage* pmoImage = GetMLMedia()->GetIImage();
	if (pmoImage)
	{
		int angle = pmoImage->GetAngle();
		if (m_sprite)
		{
			m_sprite->SetAngle(angle);
		}
	}
}

void nrmImage::MaxDecreaseKoefChanged()
{
	mlIImage* pmoImage = GetMLMedia()->GetIImage();
	if (pmoImage)
	{
		unsigned char maxDecreaseKoef = pmoImage->GetMaxDecreaseKoef();
		if (m_sprite)
		{
			int textureID = m_sprite->GetTextureID(0);
			if (textureID != -1)
			{
				CTexture* texture = g->tm.GetObjectPointer(textureID);
				texture->SetMaxDecreaseKoef(maxDecreaseKoef);
			}
			m_sprite->SetMaxDecreaseKoef(maxDecreaseKoef);
		}
	}
}

void nrmImage::SysmemChanged()
{
	mlIImage* pmoImage = GetMLMedia()->GetIImage();
	if (pmoImage)
	{
		bool sysmem = pmoImage->GetSysmem();
		if (m_sprite)
		{
			m_sprite->KeepInSystemMemory(sysmem);
		}
	}
}

void* nrmImage::GetMediaPointer()
{
	return m_sprite;
}

void nrmImage::SetPalette(CToolPalette* palette)
{
	if ((m_palette) && (m_palette != palette))
	{
		gRM->mouseCtrl->RemoveMouseListenerFromLastUsed(m_palette);
		gRM->wbMan->OnDeleteWhiteboard(this);
		MP_DELETE(m_palette);
		m_palette = NULL;	
	}

	m_palette = palette;
	m_version = -1;
	if (mpMLMedia != NULL)
	{
		mpMLMedia->ReadyForBinState();
	}
}

void nrmImage::Update(unsigned int /*deltaTime*/)
{
	if (!m_editableNow)
	{
		if (m_isNeedToClear)
		{
			mlIBinSynch* binSynch = GetMLMedia()->GetIBinSynch();
			if (!binSynch)
			{
				return;
			}

			if (!m_palette)
			{
				return;
			}

			m_isNeedToClear = false;
			IncreaseVersion();
			binSynch->StateChanged(m_version, BSSCC_CLEAR, NULL, 0);
			m_palette->ClearScreen();
			gRM->nrMan->UnregisterController(this);
		}

		return;
	}

	if (g->ne.time - m_lastUpdateTime < MIN_SYNCH_TIME)
	{
		return;
	}

	if (!m_palette)
	{
		return;
	}

	mlIBinSynch* binSynch = GetMLMedia()->GetIBinSynch();
	if (!binSynch)
	{
		return;
	}

	unsigned char* data = NULL;
	unsigned int dataSize = 0;
	unsigned char mode = MODE_ADD;
	m_palette->GetSynch()->GetNewSynchActions(&data, &dataSize, &mode);
	if (MODE_CLEAR == mode)
	{
		IncreaseVersion();
		binSynch->StateChanged(m_version, BSSCC_CLEAR, NULL, 0);
	}

	if (dataSize > 0)
	{
		IncreaseVersion();
		binSynch->StateChanged(m_version, BSSCC_APPEND, data, dataSize);
	}	

	m_lastUpdateTime = g->ne.time;
}

bool nrmImage::ChangeState( unsigned char type, unsigned char *aData, unsigned int aDataSize)
{	
	if (!m_palette)
	{
		return false;
	}
	MP_NEW_P3(pIPadBinState, IPadBinState, type, aData, aDataSize);	
	mutexIPad.lock();
	v_IPadImage.push_back( pIPadBinState);
	mutexIPad.unlock();
	//
	
	//int mode = type == BSSCC_REPLACE_ALL ? PLAY_MODE_RESTORE_STATE : (type == BSSCC_CLEAR ? MODE_CLEAR : PLAY_MODE_ADD);
	//int mode = type == BSSCC_REPLACE_ALL ? PLAY_MODE_RESTORE_STATE : (type == BSSCC_CLEAR ? MODE_CLEAR : MODE_ADD);
	//m_palette->GetSynch()->SaveSynchAction(ACTION_ID_MOUSEMOVE, aData, aDataSize);
	
	/*bool bIsSet =  true;
	if (type == BSSCC_REPLACE_ALL)
		bIsSet = SetFullState( m_version + 1, aData, aDataSize);
	else if (type == BSSCC_APPEND)
		bIsSet = UpdateState( m_version + 1, BSSCC_APPEND, aData, aDataSize);
	else if (type == BSSCC_CLEAR)
		bIsSet = UpdateState( m_version + 1, BSSCC_CLEAR, NULL, 0);*/

	return true;
}

bool nrmImage::GetBinState( BYTE*& aBinState, int& aBinStateSize)
{	
	mlIBinSynch* binSynch = GetMLMedia()->GetIBinSynch();
	if (!binSynch)
	{
		return false;
	}
	binSynch->GetBinState( aBinState, aBinStateSize);
	return true;
}

bool nrmImage::SetIPadDrawing( bool abIPadDrawing)
{
	if( m_palette != NULL)
		m_palette->SetIPadDrawing( abIPadDrawing);
	return true;
}

bool nrmImage::ApplyIPadState()
{
	if (!m_palette)
	{
		return false;
	}

	mlIBinSynch* binSynch = GetMLMedia()->GetIBinSynch();
	if (!binSynch)
	{
		return false;
	}
	mutexIPad.lock();
	if( v_IPadImage.size()>0)
	{
		CComString s = "";				
		IPadBinState *pIPadBinState = (IPadBinState*)v_IPadImage[0];		
		//mode = pIPadBinState->mode == BSSCC_REPLACE_ALL ? PLAY_MODE_RESTORE_STATE : (pIPadBinState->mode == BSSCC_CLEAR ? MODE_CLEAR : PLAY_MODE_ADD);		
		//if (MODE_CLEAR == mode)
		if (pIPadBinState->mode == BSSCC_CLEAR)
		{
			//m_palette->ClearScreen();
			IncreaseVersion();
			binSynch->StateChanged(m_version, BSSCC_CLEAR, NULL, 0);
			/*int toolID = m_palette->GetActiveToolID();
			m_palette->SetActiveToolID( TOOL_ERASER_ID);			
			m_palette->SetActiveToolID( TOOL_CLEAR_SCREEN_ID);
			m_palette->SetActiveToolID( toolID);*/
		}
		else if (pIPadBinState->size > 0)
		{	
			bool bIsSet =  true;
			if (pIPadBinState->mode == BSSCC_APPEND)
			{
				IncreaseVersion();	
				bIsSet = UpdateState( m_version, BSSCC_APPEND, pIPadBinState->data, pIPadBinState->size);
				binSynch->StateChanged(m_version, BSSCC_APPEND, pIPadBinState->data, pIPadBinState->size);
			}
			else if (pIPadBinState->mode == BSSCC_REPLACE_ALL) 
			{
				bIsSet = SetFullState( m_version+1, pIPadBinState->data, pIPadBinState->size, true);
				IncreaseVersion();
			}
		}		
		MP_DELETE( pIPadBinState);
		v_IPadImage.erase( v_IPadImage.begin());
	}
	mutexIPad.unlock();
	return true;
}

void nrmImage::IncreaseVersion()
{
	m_version++;
	if (m_version == 0)
		m_version = 1;
}

moIBinSynch* nrmImage::GetIBinSynch()
{
	return this;
}

bool nrmImage::SetFullState(unsigned int auStateVersion, const unsigned char* apState, int aiSize)
{
	return SetFullState(auStateVersion, apState, aiSize, false);
}

bool nrmImage::SetFullState(unsigned int auStateVersion, const unsigned char* apState, int aiSize, bool anIpadState)
{
	if (g->ne.isIconicAndIntel)
	{
		return false;
	}

	if (!m_palette)
	{
		return false;
	}

	m_isBinaryStateReceived = true;

	int t1 = g->tp.GetTickCount();	
	
	if ((auStateVersion != m_version) || (mpMLMedia != NULL && mpMLMedia->IsForcedUpdateBinState()))
	{
		m_version = auStateVersion;
		m_palette->ClearScreen();


		m_palette->GetSynch()->PlaySynchActions((unsigned char*)apState, aiSize, PLAY_MODE_RESTORE_STATE, anIpadState);		

		int	t2 = g->tp.GetTickCount();
		if (t2 - t1 > 20)
		{
			g->lw.WriteLn("PlaySynchActions: ", m_palette->GetName(), " in ", t2 - t1, " ms");		
		}	

		if  (mpMLMedia != NULL && mpMLMedia->IsForcedUpdateBinState())
		{
			g->lw.WriteLn("[[WB]TRANSPOSE BINSTATE]");
			m_palette->GetSynch()->LogData((unsigned char*)apState, aiSize);
		}	
	}

	return true;
}

bool nrmImage::UpdateState(unsigned int auStateVersion, BSSCCode aCode, const unsigned char* apData, int aulSize, 
		unsigned long aulOffset, unsigned long aulBlockSize)
{
	if (g->ne.isIconicAndIntel)
	{
		return false;
	}

	if (!m_palette)
	{
		return false;
	}		

	// обновление со смещением не реализовано
	if (aulOffset == 0) 
		assert(aulBlockSize == 0);

	m_version = auStateVersion;

	if (BSSCC_APPEND == aCode)
	{
		m_palette->GetSynch()->PlaySynchActions((unsigned char*)apData, aulSize);

		if  (mpMLMedia != NULL && mpMLMedia->IsForcedUpdateBinState())
		{
			g->lw.WriteLn("[[WB]TRANSPOSE BINSTATE]");
			m_palette->GetSynch()->LogData((unsigned char*)apData, aulSize);
		}	

	}
	else if (BSSCC_CLEAR == aCode)
	{
		m_palette->ClearScreen();
	}

	return true;
}

unsigned long nrmImage::GetFullState(unsigned int& auStateVersion, const unsigned char* apState, int aiSize)
{
	if (m_version == 0)
		m_version = 1;

	auStateVersion = m_version;
	if (m_palette == NULL)
		return 0;

	if (m_palette->GetSynch() == NULL)
		return 0;

	if (apState == NULL)
		aiSize = GetBinarySize();
	else
	{
	aiSize = 0;
	m_palette->GetSynch()->GetSynchActions((unsigned char **)&apState, (unsigned int*)&aiSize);
	}

	return aiSize;
}

void nrmImage::Reset()
{
	if (g->ne.isIconicAndIntel)
	{
		return;
	}

	m_version = 0;
	if (m_palette != NULL)
		m_palette->ClearScreen();
}

bool nrmImage::SetDynamicTexture(IDynamicTexture* apIDynamicTexture)
{
	if (!m_sprite)
	{
		return false;
	}

	unsigned int textureID = g->tm.AddDynamicTexture(apIDynamicTexture);
	m_sprite->SetTextureID(textureID);

	return true;
}

bool nrmImage::DynamicTextureDeleting(IDynamicTexture* apIDynamicTexture)
{
	if (!m_sprite)
	{
		return false;
	}

	int textureID = m_sprite->GetTextureID(0);
	CTexture* texture = g->tm.GetObjectPointer(textureID);
	if (texture->GetDynamicTexture() == apIDynamicTexture && m_sharingController)
		m_sharingController->SetDinamicTexture(NULL);

	return true;
}

bool nrmImage::ResizeImage()
{
	if (m_sprite)
	{
		int textureID = m_sprite->GetTextureID(0);
		CTexture* texture = g->tm.GetObjectPointer(textureID);
		m_textureWidthBeforeResize = texture->GetTextureRealWidth();
		m_textureHeightBeforeResize = texture->GetTextureRealHeight();
		g->tm.ResizeTexture(textureID, RESIZE, RESIZE);
		return true;
	}

	return false;
}

void nrmImage::ResetResize()
{
	g->tm.ResetResizeTexture(m_sprite->GetTextureID(0), m_textureWidthBeforeResize, m_textureHeightBeforeResize);
}

void nrmImage::SetSharingController(CSharingMouseController* contrl)
{
	if ((m_sharingController) && (m_sharingController != contrl))
	{
		gRM->mouseCtrl->RemoveMouseListenerFromLastUsed(m_sharingController);
		MP_DELETE(m_sharingController);
		m_sharingController = NULL;
	}

	m_sharingController = contrl;
}

bool nrmImage::isPaintToolUsed()
{
	return m_palette ? m_palette->IsToolsUsed() : false;
}

bool nrmImage::isFirstSharingFrameDrawed()
{
	if (m_sprite == NULL)
		return false;

	int textureID = m_sprite->GetTextureID(0);
	CTexture* texture = g->tm.GetObjectPointer(textureID);

	if (texture == NULL)
		return false;

	IDynamicTexture * dynamicTexture = NULL;
	if ( (dynamicTexture = texture->GetDynamicTexture()) == NULL)
		return false;

	return dynamicTexture->IsFirstFrameSetted();
}

void nrmImage::CopyToClipboard()
{
	if (!m_sprite)
	{
		return;
	}

	int textureID = m_sprite->GetTextureID(0);
	if (-1 == textureID)
	{
		return;
	}

	CTexture* texture = g->tm.GetObjectPointer(textureID);
	unsigned char* data = (unsigned char*)texture->GetRawData();
	if (!data)
	{
		return;
	}

	int bpp = texture->IsTransparent() ? 32 : 24;
	int size = texture->GetTextureWidth() * texture->GetTextureHeight() * bpp / 8;
	if (24 == bpp)
	{
		unsigned char* _data = MP_NEW_ARR(unsigned char, texture->GetTextureWidth() * texture->GetTextureHeight() * 4);
		for (int i = 0, k = 0; i < size; i += 3, k += 4)
		{
			_data[k] = data[i + 2];
			_data[k + 1] = data[i + 1];
			_data[k + 2] = data[i];
			_data[k + 3] = 255;			
		}

		MP_DELETE_ARR(data);
		data = _data;

		bpp = 32;
	}
	else
	{
		for (int i = 0; i < size; i += 4)
		{
			unsigned char tmp = data[i];
			data[i] = data[i + 2];
			data[i + 2] = tmp;
			data[i + 3] = 255;
		}
	}
	HBITMAP bitmap = CreateBitmap(texture->GetTextureWidth(), texture->GetTextureHeight(), 1, bpp, data);
  	
	if (OpenClipboard(g->ne.ghWnd))
	{
		EmptyClipboard();
		SetClipboardData(CF_BITMAP, bitmap) ;
		CloseClipboard();
	}	
	
	DeleteObject(bitmap);

	MP_DELETE_ARR(data);
}

bool nrmImage::IsBinaryStateReceived()
{
	return m_isBinaryStateReceived;
}

void nrmImage::Save()
{
	if (!m_sprite)
	{
		return;
	}

	int textureID = m_sprite->GetTextureID(0);
	if (-1 == textureID)
	{
		return;
	}

	CTexture* texture = g->tm.GetObjectPointer(textureID);

	std::wstring path = L"";
	
	wchar_t pwcPath[3000];
	if (gRM->nrMan->GetContext()->mpApp->GetLocalFilePath(L"*.jpg, *.png", L"", false, pwcPath, 2995, L"image.png"))
	{	
		path = pwcPath;
		g->tm.SaveTexture(texture, path, NULL);
	}
}

nrmImage::~nrmImage()
{
	if (m_palette)
	{
		gRM->wbMan->OnDeleteWhiteboard(this);
		gRM->nrMan->UnregisterController(this);
		gRM->mouseCtrl->RemoveMouseListenerFromLastUsed(m_palette);
		MP_DELETE(m_palette);
	}
		
	if (m_sharingController)
	{
		gRM->nrMan->UnregisterController(this);
		gRM->mouseCtrl->RemoveMouseListenerFromLastUsed(m_sharingController);
		MP_DELETE(m_sharingController);
	}
		
	if (m_sprite)
	{
		m_sprite->DeleteChangesListener(this);
		gRM->scene2d->DeleteSprite(m_sprite->GetID());
		g->sprm.DeleteObject(m_sprite->GetID());	
	}

	if (m_src == L":kinect")
	{
		if (g->phom.GetControlledObject())
		{
			if (g->phom.GetControlledObject()->GetObject3D())
			{
				if (g->phom.GetControlledObject()->GetObject3D()->GetAnimation())
				{
					CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)g->phom.GetControlledObject()->GetObject3D()->GetAnimation();					
					//sao->DeleteDynamicVideoTexture();
				}
			}
		}
	}
}