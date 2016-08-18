#include "StdAfx.h"
#include <Assert.h>
#include "Sprite.h"
#include "HelperFunctions.h"
#include "XMLPropertiesList.h"
#include "PlatformDependent.h"
#include "GlobalSingletonStorage.h"

CSprite::CSprite():
	m_tx1(0),
	m_ty1(0),
	m_tx2(0),
	m_ty2(0),
	m_angle(0),
	m_maxDecreaseKoef(4),
	m_alpha(255),	
	m_deltaTime(40),
	m_frameCount(0),
	m_textureID(-1),
	m_isTiling(false),	
	m_isSmooth(false),
	m_isDynamic(false),
	m_lastFrameNum(-1),
	m_isSnapshot(false),	
	m_color(255, 255, 255),
	m_canBeCompressed(false),
	m_isWorkRectDefined(false),
	m_isUniqueTextureUsed(false),
	m_isPermitTransparent(false),
	m_isKeepInSystemMemory(false),
	CBaseSprite(SPRITE_TYPE_SPRITE)
{
}

void CSprite::SetPermitTransparent(bool isPermit)
{
	m_isPermitTransparent = isPermit;
}

void CSprite::SetAngle(int const angle)
{
	m_angle = ((angle % 360)+360) % 360;
}

void CSprite::SetMaxDecreaseKoef(unsigned char maxDecreaseKoef)
{
	m_maxDecreaseKoef = maxDecreaseKoef;
}

unsigned char CSprite::GetMaxDecreaseKoef()const
{
	return m_maxDecreaseKoef;
}

void CSprite::KeepInSystemMemory(bool isKeep)
{
	m_isKeepInSystemMemory = isKeep;
}

bool CSprite::IsKeepInSystemMemory()const
{
	return m_isKeepInSystemMemory;
}

void CSprite::SetCanBeCompressed(bool canBeCompressed)
{
	m_canBeCompressed = canBeCompressed;
}

bool CSprite::CanBeCompressed()const
{
	return m_canBeCompressed;
}

void CSprite::SetTiling(bool isTiling)
{
	m_isTiling = isTiling;
}

bool CSprite::IsTilingEnabled()const
{
	return m_isTiling;
}

void CSprite::SetSmooth(bool isSmooth)
{
	m_isSmooth = isSmooth;
}

bool CSprite::IsSmoothEnabled()const
{
	return m_isSmooth;
}

void CSprite::SetColor(unsigned char r, unsigned char g, unsigned char b)
{
	m_color.r = r;
	m_color.g = g;
	m_color.b = b;
}

void CSprite::GetColor(unsigned char& r, unsigned char& g, unsigned char& b)
{
	r = m_color.r;
	g = m_color.g;
	b = m_color.b;
}

void CSprite::LoadFromColorTexture(std::string fileName)
{
	unsigned int _color = HexStringToDec(fileName) + 0xFF000000;
	unsigned int color = 0;

	unsigned char* colorArr = (unsigned char*)&color;
	unsigned char* _colorArr = (unsigned char*)&_color;

	colorArr[0] = _colorArr[2];
	colorArr[1] = _colorArr[1];
	colorArr[2] = _colorArr[0];
	colorArr[3] = _colorArr[3];

	if (m_textureID == -1)
	{
		int textureID = g->tm.AddColorTexture(fileName, color);
		ChangeTexture(textureID);

		OnChanged(0);
	}
	else
	{
		g->tm.ReplaceColorTexture(m_textureID, color);
	}
}

void CSprite::ClearTextureList()
{
	if (m_textureID != -1)
	{
		CTexture* texture = g->tm.GetObjectPointer(m_textureID);
		texture->ReleaseRef();			
		texture->DeleteOwner(this);
	}

	m_textureID = -1;
}

bool CSprite::LoadFromTextureFile(std::string &fileName)
{
	m_tx1 = 0;
	m_ty1 = 0;
	m_tx2 = 0;
	m_ty2 = 0;

	int textureID = g->tm.AddObject(fileName);	
	CTexture* texture = g->tm.GetObjectPointer(textureID);
	g->tm.RegisterTexture(texture);
	texture->KeepInSystemMemory(m_isKeepInSystemMemory);
	texture->AddChangesListener(this);
	texture->AddRef();
	texture->GetLoadingInfo()->SetCanBeCompressed(m_canBeCompressed);
	g->tm.LoadTextureFromFile(fileName, fileName);

	ChangeTexture(textureID);	

	if (texture->GetLoadedStatus() == TEXTURE_LOADED)
	{
		SetLoadedStatus(true);
		OnAfterChanges();
	}

	if ((texture->GetTextureWidth() != 0) && (texture->GetTextureHeight() != 0))
	{
		if( (!IsSizeDefined()) || ((m_width == 0) && (m_height == 0)))
		{
			m_width = texture->GetTextureWidth();
			if (m_frameCount != 0)
			{
				m_width = (int)(m_width / m_frameCount);
			}
			m_height = texture->GetTextureHeight();
		}

		if (!m_isWorkRectDefined)
		{
			if ((0 == m_tx2 - m_tx1) && (0 == m_ty2 - m_ty1) &&
				(0 == m_tx1) && (0 == m_ty1))
			{
				m_tx2 = texture->GetTextureWidth();
				m_ty2 = texture->GetTextureHeight();
			}
		}
	}

	return true;
}

bool CSprite::LoadFromTexture(std::string &fileName, bool isOnlyLoad, bool isMipMap, SColorKey* colorKey)
{
	std::wstring wfileName = CharToWide( fileName.c_str());
	return LoadFromTexture( wfileName,  isOnlyLoad, isMipMap, colorKey);
}

bool CSprite::LoadFromTexture(std::wstring &fileName, bool isOnlyLoad, bool isMipMap, SColorKey* colorKey)
{
	if ((fileName.size() == 0) || (fileName.find(L"#blank#") != -1))
	{
		if (m_textureID != -1)
		{
			g->tm.DeleteObject(m_textureID);
			m_textureID = -1;
		}
	}

	m_tx1 = 0;
	m_ty1 = 0;
	m_tx2 = 0;
	m_ty2 = 0;

	SetLoadedStatus(false);

	if (isOnlyLoad)
	{
		ifile* file = g->rl->GetResMemFile(fileName.c_str());
	
		if (!file)
		{
			USES_CONVERSION;
			IAsynchResource* asynch = g->rl->GetAsynchResMan()->GetAsynchResource( W2A(fileName.c_str()), true, RES_TYPE_TEXTURE, this, g->rl, NULL, NULL);
		
			if (asynch)
			{
				// resource not found on local disk
			}
		}
		else
		{
			g->rl->DeleteResFile(file);
		}

		return true;
	}

	if (fileName.size() > 1)
	{
		if (fileName[1] == L'#')
		{
			USES_CONVERSION;
			LoadFromColorTexture( W2A(fileName.substr(2, fileName.size() - 2).c_str()));
			SetLoadedStatus(true);
			return true;
		}
	}

	int dogPos = fileName.find_first_of(L"@");
	if (dogPos != -1)
	{
		int dogPos2 = fileName.find_last_of(L"@");
		
		if (dogPos != dogPos2)
		{
			m_frameCount = _wtoi(fileName.substr(dogPos + 1, dogPos2 - dogPos - 1).c_str());
			m_deltaTime = _wtoi(fileName.substr(dogPos2 + 1, fileName.size() - dogPos2 - 1).c_str());
		}

		fileName = fileName.substr(0, dogPos);
	}

	ClearTextureList();

	int textureID = g->tm.GetObjectNumber(fileName);
	if (-1 == textureID)
	{
		ifile* file = g->rl->GetResMemFile(fileName.c_str());
		if (file)
		{
			textureID = g->tm.AddObject(fileName);			
			CTexture* texture = g->tm.GetObjectPointer(textureID);
			g->tm.RegisterTexture(texture);
			texture->KeepInSystemMemory(m_isKeepInSystemMemory);
			texture->AddChangesListener(this);
			texture->AddRef();
			texture->AddOwner(this);
			if (isMipMap)
			{
				texture->SetObligatoryMipmaps(true);
			}
			texture->GetLoadingInfo()->SetCanBeCompressed(m_canBeCompressed);
			texture->GetLoadingInfo()->SetMipMapStatus(isMipMap);
			texture->GetLoadingInfo()->SetFilteringMode(!isMipMap ? FILTERING_MODE_PIXEL : FILTERING_MODE_LINEAR);
			texture->SetMaxDecreaseKoef(m_maxDecreaseKoef);
			if ((colorKey) && (colorKey->type != KEY_COLOR_NOT_USED))
			{
				texture->SetTransparentStatus(true);				
				texture->SetKeyColor(colorKey->type);
				texture->SetColor(colorKey->r, colorKey->g, colorKey->b);
			}
			bool isLoaded = g->tm.LoadFromIFile(texture, file, isMipMap, true);
			if (!isLoaded)
			{
				SetLoadedStatus(false);
				OnAfterChanges();
			}
			g->rl->DeleteResFile(file);
		}
		else
		{
			CNTextureLoadingInfo info;
			if (!g->gi.IsNPOTSupport())
			{
				info.SetMipMapStatus(isMipMap);
			}
			info.SetCanBeCompressed(m_canBeCompressed);
			info.SetFilteringMode((isMipMap == false) ? FILTERING_MODE_PIXEL : FILTERING_MODE_LINEAR);

			if ((colorKey) && (colorKey->type != KEY_COLOR_NOT_USED))
			{
				info.SetColorKey(colorKey->type, colorKey->r, colorKey->g, colorKey->b);				
			}

			textureID = g->tm.LoadAsynch(fileName, fileName, NULL, false, &info, this->GetMaxDecreaseKoef());			
			SetLoadedStatus(false);
	
			if (textureID != -1)
			{
				CTexture* texture = g->tm.GetObjectPointer(textureID);	
				texture->AddOwner(this);				
				if (isMipMap)
				{
					texture->SetObligatoryMipmaps(true);
				}

				texture->KeepInSystemMemory(m_isKeepInSystemMemory);				
				texture->AddChangesListener(this);
				texture->AddRef();
			}
			else
			{
				OnAfterChanges();
				return false;
			}
		}
	}
	else
	{
		CTexture* texture = g->tm.GetObjectPointer(textureID);
		texture->AddOwner(this);
		texture->AddRef();

		switch (texture->GetLoadedStatus())
		{
		case TEXTURE_NOT_LOADED_YET:
			texture->AddChangesListener(this);
			break;

		case TEXTURE_LOAD_ERROR:
			SetLoadedStatus(false);
			OnAfterChanges();
			break;
		}
	}

	ChangeTexture(textureID);

	CTexture* texture = g->tm.GetObjectPointer(textureID);

	if (texture->GetLoadedStatus() == TEXTURE_LOADED)
	{
		SetLoadedStatus(true);
		OnAfterChanges();
	}

	if ((texture->GetTextureWidth() != 0) && (texture->GetTextureHeight() != 0))
	{
		if( (!IsSizeDefined()) || ((m_width == 0) && (m_height == 0)))
		{
			m_width = texture->GetTextureWidth();
			if (m_frameCount != 0)
			{
				m_width = (int)(m_width / m_frameCount);
			}
			m_height = texture->GetTextureHeight();
		}

		if (!m_isWorkRectDefined)
		{
			if ((0 == m_tx2 - m_tx1) && (0 == m_ty2 - m_ty1) &&
				(0 == m_tx1) && (0 == m_ty1))
			{
				m_tx2 = texture->GetTextureWidth();
				m_ty2 = texture->GetTextureHeight();
			}
		}
	}
	else
	{
		SetLoadingSpriteSize();
	}

	return true;
}

void CSprite::SetLoadingSpriteSize()
{
	m_width = EMPTY_TEXTURE_LINEAR_SIZE;
	m_height = EMPTY_TEXTURE_LINEAR_SIZE;

	m_tx1 = 0;
	m_ty1 = 0;
	m_tx2 = EMPTY_TEXTURE_LINEAR_SIZE;
	m_ty2 = EMPTY_TEXTURE_LINEAR_SIZE;
}

void CSprite::OnChanged(int eventID)
{
	if (-1 == m_textureID)
	{
		return;
	}

	CTexture* texture = g->tm.GetObjectPointer(m_textureID);
	if (texture->GetLoadedStatus() != TEXTURE_LOADED)
	{
		OnAfterChanges(eventID);
		return;
	}

	if (!IsSizeDefined())
	{
		if ((EMPTY_TEXTURE_LINEAR_SIZE == m_width) && (EMPTY_TEXTURE_LINEAR_SIZE == m_height))
		{
			m_width = texture->GetTextureWidth();
			m_height = texture->GetTextureHeight();
		}
	}

	if (!m_isWorkRectDefined)
	{
		if ((EMPTY_TEXTURE_LINEAR_SIZE == m_tx2 - m_tx1) && (EMPTY_TEXTURE_LINEAR_SIZE == m_ty2 - m_ty1) &&
			(0 == m_tx1) && (0 == m_ty1))
		{
			m_tx2 = texture->GetTextureWidth();
			m_ty2 = texture->GetTextureHeight();
		}
	}
	
	if (!IsDeleted())
	{
		OnAfterChanges(eventID);
	}

	SetLoadedStatus(true);
}

bool CSprite::LoadSpriteFromXML(std::string fileName)
{
	USES_CONVERSION;
	std::wstring path = GetApplicationRootDirectory() + L"external\\sprites\\" + A2W( fileName.c_str());
	CXMLPropertiesList props(path, ERROR_IF_NOT_FOUND);

	if (props.GetString("type") == "static")
	{
		m_isDynamic = false;
	} 
	else if (props.GetString("type") == "animated")
	{
		m_isDynamic = true;
	}
	else
	{
		g->lw.WriteLn("ERROR: invalid type of sprite in ", fileName);
		return false;
	}

	int textureID = g->tm.GetObjectNumber(props.GetString("texture"));
	ChangeTexture(textureID);
	
	m_x = props.GetInteger("x");
	m_y = props.GetInteger("y");

	m_tx1 = props.GetInteger("u1");
	m_ty1 = props.GetInteger("v1");

	m_tx2 = props.GetInteger("u2");
	m_ty2 = props.GetInteger("v2");

	m_alpha = (unsigned char)props.GetInteger("alpha");

	m_angle = props.GetInteger("angle");

	return true;
}

bool CSprite::IsNeedToUpdate()
{
	if (m_frameCount == 0)
	{
		return false;
	}

	int frameNum = (int)(g->ne.time / m_deltaTime)%m_frameCount;
	return (m_lastFrameNum != frameNum);
}

void CSprite::Draw(CPolygons2DAccum* const accum)
{
	Draw(m_x, m_y, accum);
}
           
void CSprite::Draw(const int x, const int y, CPolygons2DAccum* const accum)
{
	if (-1 == m_textureID)
	{
		return;
	}

	if ((m_width <= 0) || (m_height <= 0))
	{
		return;
	}

	if (m_scissorsWidth != -1)
	{
		if ((y > m_scissorsY + m_scissorsHeight) || (y + m_height < m_scissorsY))
		{
			return;
		}
	}

	if ((x > (int)g->stp.GetCurrentWidth()) || (y > (int)g->stp.GetCurrentHeight()) ||
		(x + m_width < 0) || (y + m_height < 0))
	{
		return;
	}

	stat_2dVisibleSpritesCount++;

	//int textureNum = (int)(g->ne.time / m_deltaTime)%(m_texturesID.size());

	CTexture* texture = g->tm.GetObjectPointer(/*m_texturesID[textureNum]*/m_textureID);
	texture->SetReallyUsedStatus(true);
	texture->SetBindedStatus(true);
	bool isAlpha = texture->IsTransparent() || (m_alpha != 255);
	if (m_isPermitTransparent)
	{
		isAlpha = false;
	}

	if (texture->GetDynamicTexture())
	{
		GetScene2D()->AddDynamicTexture(texture);
	}

	if (m_frameCount != 0)
	{
		int frameNum = (int)(g->ne.time / m_deltaTime)%m_frameCount;
		m_tx1 = (int)(texture->GetTextureWidth() / m_frameCount) * frameNum;
		m_tx2 = (int)(texture->GetTextureWidth() / m_frameCount) * (frameNum + 1);
		m_lastFrameNum = frameNum;
	}	

	OnBeforeDraw(accum);

	if (accum)
	{
		accum->ChangeSmooth(this->IsSmoothEnabled());
		if (!m_isTiling)
		{
			if (!isAlpha)
			{
				g->sp.PutSprite(x, y, x + m_width, y + m_height, m_tx1, m_ty1, m_tx2, m_ty2, /*m_texturesID[textureNum]*/m_textureID, m_color, accum, m_angle);
			}
			else
			{
				g->sp.PutSprite(x, y, x + m_width, y + m_height, m_tx1, m_ty1, m_tx2, m_ty2, /*m_texturesID[textureNum]*/m_textureID, m_color, accum, m_angle, false, true, m_alpha);
			}
		}
		else
		{
			if (!isAlpha)
			{
				g->sp.PutSprite(x, y, x + m_width, y + m_height, m_tx1, m_ty1, m_tx1 + m_width, m_ty1 + m_height, /*m_texturesID[textureNum]*/m_textureID, m_color, accum, m_angle, true);
			}
			else
			{
				g->sp.PutSprite(x, y, x + m_width, y + m_height, m_tx1, m_ty1, m_tx1 + m_width, m_ty1 + m_height, /*m_texturesID[textureNum]*/m_textureID, m_color, accum, m_angle, true, true, m_alpha);
			}
		}
		accum->ChangeSmooth(false);
	}
	else
	if (!m_isTiling)
	{
		if (!isAlpha)
		{
			g->sp.PutNormalSprite(x, y, x + m_width, y + m_height, m_tx1, m_ty1, m_tx2, m_ty2, /*m_texturesID[textureNum]*/m_textureID, m_color, m_angle);
		}
		else
		{
			g->sp.PutAlphaSprite(x, y, x + m_width, y + m_height, m_tx1, m_ty1, m_tx2, m_ty2, /*m_texturesID[textureNum]*/m_textureID, m_alpha, m_color, m_angle);
		}
	}
	else
	{
		if (!isAlpha)
		{
			
			g->sp.PutNormalSprite(x, y, m_tx1, m_ty1, m_tx1 + m_width, m_ty1 + m_height, /*m_texturesID[textureNum]*/m_textureID, m_color, m_angle, true);
		}
		else
		{
			
			g->sp.PutAlphaSprite(x, y, m_tx1, m_ty1, m_tx1 + m_width, m_ty1 + m_height, /*m_texturesID[textureNum]*/m_textureID, m_alpha, m_color, m_angle, true);
		}
	}	

	OnAfterDraw(accum);
}

void CSprite::SetFullWorkRect()
{
	if (-1 == m_textureID)
	{
		return;
	}

	CTexture* texture = g->tm.GetObjectPointer(m_textureID);

	m_width = texture->GetTextureWidth();
	m_height = texture->GetTextureHeight();
	m_tx1 = 0;
	m_ty1 = 0;
	m_tx2 = m_width;
	m_ty2 = m_height;

	m_isWorkRectDefined = true;
}

void CSprite::SetWorkRect(int u1, int v1, int u2, int v2)
{
	if (!IsLoaded())
	{
		return;
	}

	if ((GetScene2D()) && ((m_tx1 != u1) || (m_ty1 != v1) || (m_tx2 != u2) || (m_ty2 != v2)) && (m_isVisible))
	{
		GetScene2D()->OnParamsChanged();
	}

	m_tx1 = u1;
	m_ty1 = v1;
	m_tx2 = u2;
	m_ty2 = v2;

	assert(m_textureID != -1);
	if (m_textureID == -1)
	{
		return;
	}

	CTexture* texture = g->tm.GetObjectPointer(m_textureID);
	GLFUNC(glBindTexture)(GL_TEXTURE_2D, texture->GetNumber());
	GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	g->tm.RefreshTextureOptimization();

	m_isWorkRectDefined = true;
}

void CSprite::GetWorkRect(int* u1, int* v1, int* u2, int* v2)const
{
	*u1 = m_tx1;
	*v1 = m_ty1;
	*u2 = m_tx2;
	*v2 = m_ty2;
}

bool CSprite::IsFullWorkRect()const
{
	if (m_textureID == -1)
	{
		return true;
	}

	CTexture* texture = g->tm.GetObjectPointer(m_textureID);

	if ((m_tx1 == 0) && (m_tx2 - m_tx1 == texture->GetTextureWidth()))
	if ((m_ty1 == 0) && (m_ty2 - m_ty1 == texture->GetTextureHeight()))
	{
		return true;
	}

	return false;
}

void CSprite::SetAlpha(unsigned char alpha)
{
	m_alpha = alpha;
}

unsigned char CSprite::GetAlpha()const
{
	return m_alpha;
}

void CSprite::SetTextureID(int textureID)
{
	assert(textureID != -1);

	if (textureID != -1)
	{
		ClearTextureList();
		ChangeTexture(textureID);

		CTexture* texture = g->tm.GetObjectPointer(textureID);
		if (texture->GetLoadedStatus() == TEXTURE_LOADED)
		{
			SetLoadedStatus(true);
		}
	}
}

void CSprite::SetTexture(std::string name)
{
	int textureID = g->tm.GetObjectNumber(name);
	assert(textureID != -1);

	if (textureID != -1)
	{
		ClearTextureList();
		ChangeTexture(textureID);
	}
}

bool CSprite::IsDynamic()const
{
	return m_isDynamic;
}

bool CSprite::AddTexture(std::string fileName)
{
	int textureID = g->tm.GetObjectNumber(fileName);

	if (textureID != -1)
	{
		return AddTexture(textureID);
	}
	else
	{
		return false;
	}
}

bool CSprite::AddTexture(int textureID)
{
	ChangeTexture(textureID);	

	return true;
}

void CSprite::SetDeltaTime(int deltaTime)
{
	m_deltaTime = deltaTime;
}

int CSprite::GetDeltaTime()const
{
	return m_deltaTime;
}

bool CSprite::ReplaceTexture(unsigned int num, int textureID)
{
	assert(num != 0);
	assert(textureID != -1);

	if (-1 == textureID)
	{
		return false;
	}

	if (num == 0)
	{
		m_textureID = textureID;
	}
	else
	{
		return false;
	}

	return true;
}

bool CSprite::ReplaceTexture(unsigned int num, std::string fileName)
{
	int textureID = g->tm.GetObjectNumber(fileName);
	return ReplaceTexture(num, textureID);
}

int CSprite::GetTextureID(unsigned int num)const
{
	if (num == 0)
	{
		return m_textureID;
	}
	else
	{
		return -1;
	}
}

bool CSprite::IsPixelTransparent(const int tx, const int ty)const
{
	if (!IsLoaded())
	{
		return false;
	}

	if ((tx >= 0) && (tx < m_width) && (ty >= 0) && (ty < m_height) && (m_width != 0) && (m_height != 0))
	{
		int absX = GetX() + tx;
		int absY = GetY() + ty;
		int maskX, maskY, maskWidth, maskHeight;
		GetScissors(maskX, maskY, maskWidth, maskHeight); 
		if (maskWidth != -1)
		if ((absX < maskX) || (absX > maskX + maskWidth) ||
			(absY < maskY) || (absY > maskY + maskHeight))
		{
			return true;
		}

		unsigned int textureID = GetTextureID(0);
		if (textureID != -1)
		{
			CTexture* texture = g->tm.GetObjectPointer(textureID);
			int x = (int)((float)tx / (float)m_width * (float)(m_tx2 - m_tx1)) + m_tx1;
			int y = (int)((float)ty / (float)m_height * (float)(m_ty2 - m_ty1)) + m_ty1;
			return texture->IsPixelTransparent(x, y);
		}
	}

	return true;
}

bool CSprite::GetPixelColor(const int tx, const int ty, unsigned char& r, unsigned char& _g, unsigned char& b)const
{
	if (!IsLoaded())
	{
		return false;
	}

	if((tx >= 0) && (tx < m_width) && (ty >= 0) && (ty < m_height) && (m_width != 0) && (m_height != 0))
	{
		unsigned int textureID = GetTextureID(0);
		if (textureID != -1)
		{
			CTexture* texture = g->tm.GetObjectPointer(textureID);
			int x = (int)((float)tx / (float)m_width * (float)(m_tx2 - m_tx1)) + m_tx1;
			int y = (int)((float)ty / (float)m_height * (float)(m_ty2 - m_ty1)) + m_ty1;
			bool res = texture->GetPixelColor(x, y, r, _g, b);
			return res;
		}
	}

	return true;
}

bool CSprite::IsUniqueTextureUsed()const
{
	return m_isUniqueTextureUsed;
}

void CSprite::SetUniqueTextureStatus(bool isUniqueTextureUsed)
{
	m_isUniqueTextureUsed = isUniqueTextureUsed;
}

bool CSprite::LoadFromDynamicTexture(IDynamicTexture* dynamicTexture)
{
	if (!dynamicTexture)
	{
		return false;
	}

	int textureID = g->tm.AddDynamicTexture(dynamicTexture);

	ClearTextureList();
	m_textureID = textureID;
	SetLoadedStatus(true);

	CTexture* texture = g->tm.GetObjectPointer(textureID);
	m_width = texture->GetTextureWidth();
	m_height = texture->GetTextureHeight();

	m_tx1 = 0;
	m_ty1 = 0;
	m_tx2 = m_width;
	m_ty2 = m_height;	

	return true;
}

unsigned int CSprite::GetCorrectScreenWidth()
{
	unsigned int width = g->stp.GetCurrentWidth();

	while (width%4 != 0)
	{
		width--;
	}

	return width;
}

void CSprite::GetCurrentScreen(unsigned char*& data, unsigned int& width, unsigned int& height, unsigned char& channelCount)
{
	channelCount = 3;
	height = g->stp.GetCurrentHeight();
	width = GetCorrectScreenWidth();
	unsigned int size = width * height * channelCount;
	data = MP_NEW_ARR(unsigned char, size);
	GLFUNC(glReadPixels)(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);	
}

void CSprite::RemoveDynamicTextures()
{
	if (m_textureID != -1)
	{
		CTexture* texture = g->tm.GetObjectPointer(m_textureID);
		g->tm.RemoveDynamicTexture(texture);	
	}
}

unsigned int CSprite::MakeSnapshot(bool toSystemMemory)
{
	if (m_isSnapshot)
	if (m_textureID != -1)
	{
		CTexture* texture = g->tm.GetObjectPointer(m_textureID);
		texture->MarkDeleted();		
	}

	m_textureID = -1;

	unsigned int width = GetCorrectScreenWidth();
	unsigned int textureID;

	if ((g->gi.GetVendorID() == VENDOR_INTEL) && ((toSystemMemory) || (!IsIconic(g->ne.ghWnd))))
	{
		unsigned int size = width * g->stp.GetCurrentHeight() * 3;
		unsigned char* data = MP_NEW_ARR(unsigned char, size);		

		g->lw.WriteLn("GetForegroundWindow(): ", (int)GetForegroundWindow());
		g->lw.WriteLn("g->ne.ghWnd: ", (int)g->ne.ghWnd);

		if ((!IsIconic(g->ne.ghWnd)) && (GetForegroundWindow() == g->ne.ghWnd))
		{
			glFlush();
			glFinish();

			HDC hDC = GetDC(NULL);
			HDC hMemDC = CreateCompatibleDC(hDC);

			RECT rct;
			GetWindowRect(g->ne.ghWnd, &rct);
			RECT clientRct;
			GetClientRect(g->ne.ghWnd, &clientRct);

			int captionHeight = (rct.bottom - rct.top) - (clientRct.bottom - clientRct.top);
			int borderWidth = ((rct.right - rct.left) - (clientRct.right - clientRct.left)) / 2;

			int startX = rct.left + borderWidth;
			int startY = rct.top + captionHeight - borderWidth;
			
			BITMAPINFO bmi;
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = width;
			bmi.bmiHeader.biHeight = g->stp.GetCurrentHeight();
			bmi.bmiHeader.biPlanes = 1; 
			bmi.bmiHeader.biBitCount = 24; 
			bmi.bmiHeader.biCompression = 0; 
			bmi.bmiHeader.biSizeImage = g->stp.GetCurrentHeight() * width * 3; 
			bmi.bmiHeader.biXPelsPerMeter = 0; 
			bmi.bmiHeader.biYPelsPerMeter = 0; 
			bmi.bmiHeader.biClrUsed = 0; 
			bmi.bmiHeader.biClrImportant = 0; 
	
			HBITMAP hBmpNew = CreateCompatibleBitmap(hDC, width, g->stp.GetCurrentHeight());
			SelectObject(hMemDC, hBmpNew);
			BitBlt(hMemDC, 0, 0, width, g->stp.GetCurrentHeight(), hDC, startX, startY, SRCCOPY);

			if (GetDIBits(hMemDC, hBmpNew, 0, g->stp.GetCurrentHeight(), data, &bmi, DIB_RGB_COLORS) == 0)
			{
				memset(data, 0, size);
				g->lw.WriteLn("GetDIBits failed: ", GetLastError());				
			}
			else
			{
				g->lw.WriteLn("GetDIBits success");
			}

			ReleaseDC(g->ne.ghWnd, hMemDC);
			DeleteDC(hMemDC);
			DeleteObject(hBmpNew);
		}
		else
		{
			memset(data, 0, size);
		}

		if (toSystemMemory)
		{
			std::string snapshotName = "shapshot_" + IntToStr(g->tm.GetCount());
			textureID = g->tm.AddObject(snapshotName);
			CTexture* texture = g->tm.GetObjectPointer(textureID);
			texture->SetTextureWidth(width);
			texture->SetTextureHeight(g->stp.GetCurrentHeight());
			texture->KeepInSystemMemory(true);
			texture->SetRawData(data, size, false);	
		}
		else
		{
			GLFUNC(glGenTextures)(1, &textureID);
			GLFUNC(glBindTexture)(GL_TEXTURE_2D, textureID);
		
			if (!g->gi.IsNPOTSupport())
			{
				unsigned int bigWidth = GetMinBigger2st(width);
				unsigned int bigHeight = GetMinBigger2st(g->stp.GetCurrentHeight());
				unsigned char* data_ = MP_NEW_ARR(unsigned char, bigWidth * bigHeight * 3);
				GLFUNC(glTexImage2D)(GL_TEXTURE_2D, 0, GL_RGB8, bigWidth, bigHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data_);
				MP_DELETE_ARR(data_);
				GLFUNC(glTexSubImage2D)(GL_TEXTURE_2D, 0, 0, GetMinBigger2st(g->stp.GetCurrentHeight()) - g->stp.GetCurrentHeight(), width, g->stp.GetCurrentHeight(), GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
			}
			else
			{
				GLFUNC(glTexImage2D)(GL_TEXTURE_2D, 0, GL_RGB8, width, g->stp.GetCurrentHeight(), 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
			}

			textureID = g->tm.ManageGLTexture("avatar_snapshot", textureID, width, g->stp.GetCurrentHeight());
        
			CTexture* texture = g->tm.GetObjectPointer(textureID);
			texture->SetCurrentFiltration(GL_LINEAR, GL_LINEAR, 1, 0);

			if (!g->gi.IsNPOTSupport())
			{
				texture->SetTextureRealWidth(GetMinBigger2st(width));
				texture->SetTextureRealHeight(GetMinBigger2st(g->stp.GetCurrentHeight()));
			}

			MP_DELETE_ARR(data);
		}		
	}
	else if (toSystemMemory)
	{
		unsigned int size = width * g->stp.GetCurrentHeight() * 3;
		unsigned char* pixels = MP_NEW_ARR(unsigned char, size);
		GLFUNC(glReadPixels)(0, 0, width, g->stp.GetCurrentHeight(), GL_RGB, GL_UNSIGNED_BYTE, pixels);		

		std::string snapshotName = "shapshot_" + IntToStr(g->tm.GetCount());
		textureID = g->tm.AddObject(snapshotName);
		CTexture* texture = g->tm.GetObjectPointer(textureID);
		texture->SetTextureWidth(width);
		texture->SetTextureHeight(g->stp.GetCurrentHeight());
		texture->KeepInSystemMemory(true);
		texture->SetRawData(pixels, size, true);		
	}
	else
	{
		GLFUNC(glGenTextures)(1, &textureID);
		GLFUNC(glBindTexture)(GL_TEXTURE_2D, textureID);
		
		if (!g->gi.IsNPOTSupport())
		{
			while (width%4 != 0)
			{
				width--;
			}

			GLFUNC(glTexImage2D)(GL_TEXTURE_2D, 0, GL_RGBA, GetMinBigger2st(width), GetMinBigger2st(g->stp.GetCurrentHeight()), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			GLFUNC(glCopyTexSubImage2D)(GL_TEXTURE_2D, 0, 0, GetMinBigger2st(g->stp.GetCurrentHeight()) - g->stp.GetCurrentHeight(), 0, 0, width, g->stp.GetCurrentHeight());
		}
		else
		{	
			while (width%4 != 0)
			{
				width--;
			}

			GLFUNC(glCopyTexImage2D)(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, width, g->stp.GetCurrentHeight(), 0);
		}

		textureID = g->tm.ManageGLTexture("avatar_snapshot", textureID, width, g->stp.GetCurrentHeight());
        
		CTexture* texture = g->tm.GetObjectPointer(textureID);
		texture->SetCurrentFiltration(GL_LINEAR, GL_LINEAR, 1, 0);				

		if (!g->gi.IsNPOTSupport())
		{
			texture->SetTextureRealWidth(GetMinBigger2st(width));
			texture->SetTextureRealHeight(GetMinBigger2st(g->stp.GetCurrentHeight()));
		}
	}

	m_textureID = textureID;
	SetSize(width, g->stp.GetCurrentHeight());
	SetFullWorkRect();
	m_isSnapshot = true;
	SetLoadedStatus(true);

	return textureID;
}

void CSprite::SetFrameCount(int frameCount)
{
	m_frameCount = frameCount;
}

int CSprite::GetFrameCount()const
{
	return m_frameCount;
}

void CSprite::FreeResources()
{
	if (m_textureID != -1)
	if (m_isSnapshot)
	{
		CTexture* texture = g->tm.GetObjectPointer(m_textureID);
		texture->MarkDeleted();
		texture->DeleteOwner(this);
		
	}
	else
	{
		CTexture* texture = g->tm.GetObjectPointer(m_textureID);
		texture->ReleaseRef();		
		texture->DeleteOwner(this);
	}

	m_textureID = -1;
}

void CSprite::OnAsynchResourceLoaded(IAsynchResource* /*asynch*/)
{
	SetLoadedStatus(true);
	OnAfterChanges();
	SetLoadedStatus(false);
}

void CSprite::OnAsynchResourceError(IAsynchResource* /*asynch*/)
{
	SetLoadedStatus(false);
	OnAfterChanges();
}

void CSprite::OnAsynchResourceLoadedPersent(IAsynchResource* /*asynch*/, unsigned char/* percent*/)
{
	// do nothing
}

void CSprite::ChangeTexture(unsigned int textureID)
{
	m_textureID = textureID;
}

void CSprite::DetachTexture()
{
	m_textureID = -1;
}

CSprite::~CSprite()
{
	if (g->ne.GetWorkMode() != WORK_MODE_DESTROY)
	if (m_textureID != -1)
	{
		CTexture* texture = g->tm.GetObjectPointer(m_textureID);
		texture->DeleteChangesListener(this);
		texture->DeleteOwner(this);
	}	
}