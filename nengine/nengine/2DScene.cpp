
#include "StdAfx.h"
#include "GlobalSingletonStorage.h"
#include "FormattedTextSprite.h"
#include "PlatformDependent.h"
#include "HelperFunctions.h"
#include "2DScene.h"
#include "XMLList.h"
#include <vector>
#include "cal3d/memory_leak.h"

#define ACCUM_2D		1

_SHelperText::_SHelperText():
	MP_WSTRING_INIT(text)
{
	text = L"";
	x = 0;
	y = 0;
	r = 255;
	g = 255;
	b = 255;
}

_SHelperText::_SHelperText(const _SHelperText& txt):
	MP_WSTRING_INIT(text)
{
	text = txt.text;
	x = txt.x;
	y = txt.y;
	r = txt.r;
	g = txt.g;
	b = txt.b;
}

C2DScene::C2DScene():
	m_isVisible(true),
	m_helperFont(NULL),
	m_lastUpdateTime(0),		
	m_isVisibleNow(true),
	m_helperTextCount(0),
	m_renderSpriteCount(0),
	m_isNeedToAccumAnyway(false),
	m_isNeedToUpdateAnyway(false),
	MP_VECTOR_INIT(m_dynamicTextures),
	MP_MAP_INIT(m_spriteDescs),
	MP_VECTOR_INIT(m_sprites)
{
	int i;
	for (i = 0; i < MAX_SPRITE_COUNT; i++)
	{
		m_renderQueue[i] = NULL;
	}

}

void C2DScene::SetVisible(bool isVisible)
{
	m_isVisible = isVisible;
}

void C2DScene::SetVisibleNow(bool isVisible)
{
	m_isVisibleNow = isVisible;
}

bool C2DScene::IsVisible()const
{
	return m_isVisible && m_isVisibleNow;
}

void C2DScene::SaveVisibility()
{
	std::vector<SSpriteDesc*>::iterator it = m_sprites.begin();
	std::vector<SSpriteDesc*>::iterator itEnd = m_sprites.end();

	for (int count = 0; it != itEnd; it++, count++)
	{
		(*it)->isSavedVisible = (*it)->sprite->IsVisible();		
	}
}

void C2DScene::RestoreVisibility()
{
	std::vector<SSpriteDesc*>::iterator it = m_sprites.begin();
	std::vector<SSpriteDesc*>::iterator itEnd = m_sprites.end();

	for ( ; it != itEnd; it++)
	{
		if ((*it)->isSavedVisible == -1)
		{
			continue;
		}

#ifndef USE_VISIBLE_DESTINATION
		(*it)->sprite->SetVisible(((*it)->isSavedVisible == 1));
#else
		(*it)->sprite->SetVisible(((*it)->isSavedVisible == 1), 2);
#endif
	}
}

void C2DScene::HideAll()
{
	std::vector<SSpriteDesc*>::iterator it = m_sprites.begin();
	std::vector<SSpriteDesc*>::iterator itEnd = m_sprites.end();

	for ( ; it != itEnd; it++)
	{
#ifndef USE_VISIBLE_DESTINATION
		(*it)->sprite->SetVisible(false);
#else
		(*it)->sprite->SetVisible(false, 0);
#endif
	}
}
	
void C2DScene::LoadFromXMLFile(std::string fileName)
{
	/*std::vector<std::string> optionalFields;
	optionalFields.push_back("x");
	optionalFields.push_back("y");
	optionalFields.push_back("name");
	std::string path = GetCurrentDirectory() + "\\external\\scenes2d\\";
	CXMLList scene2d(path + fileName, optionalFields, false);

	int cnt = 0;
	
	while (scene2d.GetNextValue(fileName))
	{
		SSpriteDesc* spriteDesc = MP_NEW(SSpriteDesc);
		int spriteID = g->sprm.GetObjectNumber(fileName);
		spriteDesc->sprite = g->sprm.GetObjectPointer(spriteID);

		std::string name, strX, strY;
		if (!scene2d.GetOptionalFieldValue("name", name))
		{
			name = IntToStr(cnt);
			cnt++;
		}

		if ((scene2d.GetOptionalFieldValue("x", strX)) &&
			(scene2d.GetOptionalFieldValue("y", strY)))
		{
			spriteDesc->isOwnCoords = true;
			spriteDesc->x = (short)rtl_atoi(strX.c_str());
			spriteDesc->y = (short)rtl_atoi(strY.c_str());
		}
		else
		{
			spriteDesc->isOwnCoords = false;
		}

		m_spriteDescs.insert(MP_MAP<MP_STRING, SSpriteDesc*>::value_type(MAKE_MP_STRING(name), spriteDesc));
	}*/
}

bool C2DScene::AddTextSprite(CTextSprite* text, short /*x*/, short /*y*/)
{
	std::string name = "text_";
	name += IntToStr((int)text);
	MP_MAP<void *, SSpriteDesc*>::iterator it = m_spriteDescs.find((void *)text);

	if (it == m_spriteDescs.end())
	{
		SSpriteDesc* spriteDesc = MP_NEW(SSpriteDesc);
		spriteDesc->sprite = text;
		spriteDesc->sprite->SetScene2D(this);
		m_spriteDescs.insert(MP_MAP<void *, SSpriteDesc*>::value_type((void *)text, spriteDesc));
		m_sprites.push_back(spriteDesc);
	}

	return true;
}

bool C2DScene::AddLine(CLine* line, short /*x*/, short /*y*/)
{
	std::string name = "line_";
	name += IntToStr((int)line);
	MP_MAP<void *, SSpriteDesc*>::iterator it = m_spriteDescs.find((void *)line);

	if (it == m_spriteDescs.end())
	{
		SSpriteDesc* spriteDesc = MP_NEW(SSpriteDesc);
		spriteDesc->sprite = line;
		spriteDesc->sprite->SetScene2D(this);
		m_spriteDescs.insert(MP_MAP<void *, SSpriteDesc*>::value_type((void *)line, spriteDesc));
		m_sprites.push_back(spriteDesc);
	}

	return true;
}


bool C2DScene::DeleteTextSprite(CTextSprite* text)
{
	MP_MAP<void *, SSpriteDesc*>::iterator it = m_spriteDescs.find(text);

	if (it != m_spriteDescs.end())
	{
		SSpriteDesc* desc = (*it).second;

		std::vector<SSpriteDesc *>::iterator iter = m_sprites.begin();
		std::vector<SSpriteDesc *>::iterator iterEnd = m_sprites.end();

		for ( ; iter != iterEnd; iter++)
		if ((*iter) == desc)
		{
			MP_DELETE_UNSAFE(desc);
			m_sprites.erase(iter);
			break;
		}

		m_spriteDescs.erase(it);

		if (text->IsVisible())
		{
			m_isNeedToUpdateAnyway = true;
		}
		
		for (int i = 0; i < m_renderSpriteCount; i++)
		if (m_renderQueue[i] == desc)
		{
			m_renderQueue[i] = NULL;
			break;
		}
		
		return true;
	}

	return false;
}

bool C2DScene::DeleteLine(CLine* line)
{
	MP_MAP<void *, SSpriteDesc*>::iterator it = m_spriteDescs.find(line);

	if (it != m_spriteDescs.end())
	{
		SSpriteDesc* desc = (*it).second;

		std::vector<SSpriteDesc *>::iterator iter = m_sprites.begin();
		std::vector<SSpriteDesc *>::iterator iterEnd = m_sprites.end();

		for ( ; iter != iterEnd; iter++)
		if ((*iter) == desc)
		{
			MP_DELETE_UNSAFE(desc);
			m_sprites.erase(iter);
			break;
		}

		m_spriteDescs.erase(it);

		if (line->IsVisible())
		{
			m_isNeedToUpdateAnyway = true;
		}
		
		for (int i = 0; i < m_renderSpriteCount; i++)
		if (m_renderQueue[i] == desc)
		{
			m_renderQueue[i] = NULL;
			break;
		}
		
		return true;
	}

	return false;
}

bool C2DScene::AddSprite(int spriteID, short /*x*/, short /*y*/)
{
	if ((spriteID < 0) || (spriteID >= g->sprm.GetCount()))
	{
		return false;
	}

	CSprite* sprite = g->sprm.GetObjectPointer(spriteID);
	MP_MAP<void *, SSpriteDesc*>::iterator it = m_spriteDescs.find((void *)sprite);

	if (it == m_spriteDescs.end())
	{
		SSpriteDesc* spriteDesc = MP_NEW(SSpriteDesc);
		spriteDesc->sprite = g->sprm.GetObjectPointer(spriteID);
		spriteDesc->sprite->SetScene2D(this);		
		m_spriteDescs.insert(MP_MAP<void *, SSpriteDesc*>::value_type((void *)sprite, spriteDesc));
		m_sprites.push_back(spriteDesc);
	}

	return true;
}

bool C2DScene::AddSprite(std::string name, short x, short y)
{
	int spriteID = g->sprm.GetObjectNumber(name);
	if (-1 == spriteID)
	{
		return false;
	}

	return AddSprite(spriteID, x, y);
}

bool C2DScene::DeleteSprite(int spriteID)
{
	if ((spriteID < 0) || (spriteID >= g->sprm.GetCount()))
	{
		return false;
	}

	CSprite* sprite = g->sprm.GetObjectPointer(spriteID);
	MP_MAP<void *, SSpriteDesc*>::iterator it = m_spriteDescs.find((void *)sprite);

	if (it != m_spriteDescs.end())
	{
		SSpriteDesc* desc = (*it).second;

		std::vector<SSpriteDesc *>::iterator iter = m_sprites.begin();
		std::vector<SSpriteDesc *>::iterator iterEnd = m_sprites.end();

		for ( ; iter != iterEnd; iter++)
		if ((*iter) == desc)
		{
			MP_DELETE_UNSAFE(desc);
			m_sprites.erase(iter);
			break;
		}
		
		m_spriteDescs.erase(it);

		if (sprite->IsVisible())
		{
			m_isNeedToUpdateAnyway = true;
		}				
		for (int i = 0; i < m_renderSpriteCount; i++)
		if (m_renderQueue[i] == desc)
		{
			m_renderQueue[i] = NULL;
			break;
		}		

		return true;
	}

	return false;
}

bool C2DScene::DeleteSprite(std::string name)
{
	int spriteID = g->sprm.GetObjectNumber(name);

	return DeleteSprite(spriteID);
}

bool C2DScene::DeleteSprite(std::wstring name)
{
	int spriteID = g->sprm.GetObjectNumber(name);

	return DeleteSprite(spriteID);
}

void C2DScene::OnChanged()
{
	if (!IsVisible())
	{
		return;
	}

	m_isNeedToUpdateAnyway = true;
}

void C2DScene::OnParamsChanged()
{
	if (!IsVisible())
	{
		return;
	}

	if (g->ne.GetWorkMode() != WORK_MODE_SNAPSHOTRENDER)
	{
		m_isNeedToAccumAnyway = true;
	}
}

void C2DScene::DrawImmediately()
{
	for (int i = 0; i < m_renderSpriteCount; i++)
	if (m_renderQueue[i])
	{
		m_renderQueue[i]->sprite->Draw(NULL);
	}
}

void C2DScene::RenderHelperTexts()
{
	if (g->ne.GetWorkMode() == WORK_MODE_SNAPSHOTRENDER)
	{
		return;
	}

	if (0 == m_helperTextCount)
	{
		return;
	}

	if (!m_helperFont)
	{
		m_helperFont = g->fm.GetFont("Arial", 20, true, false, false, false);		
	}

	for (int i = 0; i < m_helperTextCount; i++)
	{
		m_helperFont->SetColor(m_texts[i].r, m_texts[i].g, m_texts[i].b, 255);
		m_helperFont->OutTextXY(m_texts[i].x, m_texts[i].y, m_texts[i].text);	
	}
}

void C2DScene::Draw()
{ 
	if (!IsVisible())
	{
		return;
	}

    g->stp.PushMatrix();

	g->cm.SetCamera2d();

	g->stp.SetDefaultStates();

	g->stp.SetColor(255, 255, 255);
	g->stp.SetState(ST_ZTEST, false);
	g->stp.SetState(ST_ZWRITE, false);
	g->stp.SetState(ST_CULL_FACE, false);

	g->stp.SetMaterial(-1);
	if (g->rs.GetBool(SHADERS_USING))
	{
		g->sm.UnbindShader();
	}
	g->stp.PrepareForRender();

	RenderHelperTexts();	

	int i;

	if (m_isNeedToUpdateAnyway)
	{
		m_renderSpriteCount = 0;

		stat_2dAllCount = m_sprites.size();
		stat_2dVisibleSpritesCount = 0;
		stat_2dVisibleTextCount = 0;

		std::vector<SSpriteDesc*>::iterator iter = m_sprites.begin();
		std::vector<SSpriteDesc*>::iterator iterEnd = m_sprites.end();

		for ( ; iter != iterEnd; iter++)
		{
			if (!(*iter)->sprite->IsVisible())
			{
				continue;
			}

			if (!(*iter)->sprite->IsLoaded())
			{
				continue;
			}
						
			if (m_renderSpriteCount + 1 < MAX_SPRITE_COUNT)
			{
				m_renderQueue[m_renderSpriteCount++] = (*iter);
			}
			else
			{
				break;
			}
		}

		for (i = 0; i < m_renderSpriteCount - 1; i++)
		{
			int l = i;

			for (int k = i + 1; k < m_renderSpriteCount; k++)
			if ((m_renderQueue[k]) && (m_renderQueue[l]))
			if (m_renderQueue[k]->sprite->GetSortValue() < m_renderQueue[l]->sprite->GetSortValue())
			{
				l = k;
			}

			SSpriteDesc* tmp = m_renderQueue[i];
			m_renderQueue[i] = m_renderQueue[l];
			m_renderQueue[l] = tmp;
		}

		m_lastUpdateTime = g->ne.time;
	}
	else if (!m_isNeedToAccumAnyway)
	{
		for (int i = 0; i < m_renderSpriteCount; i++)
		if (m_renderQueue[i])
		if (m_renderQueue[i]->sprite->IsNeedToUpdate())
		{
			m_isNeedToAccumAnyway = true;
		}
	}

#ifndef ACCUM_2D
	DrawImmediately();
#else
	if (g->ne.GetWorkMode() == WORK_MODE_SNAPSHOTRENDER)
	{
		DrawImmediately();
	}
	else
	{
		if ((m_isNeedToUpdateAnyway) || (m_isNeedToAccumAnyway))
		{
			m_dynamicTextures.clear();
			//glNewList(355, GL_COMPILE);
			m_accum.Start();
			for (i = 0; i < m_renderSpriteCount; i++)
			if (m_renderQueue[i])
			{
				m_renderQueue[i]->sprite->Draw(&m_accum);				
			}
			
			m_accum.End();
			/*m_accum.Draw();
			//glEndList();*/

			m_isNeedToUpdateAnyway = false;
			m_isNeedToAccumAnyway = false;
		}
		//glCallList(355);

		std::vector<CTexture *>::iterator it = m_dynamicTextures.begin();
		std::vector<CTexture *>::iterator itEnd = m_dynamicTextures.end();

		for ( ; it != itEnd; it++)
		{
			(*it)->SetBindedStatus(true);
			(*it)->SetReallyUsedStatus(true);
		}

		m_accum.Draw();
	}
#endif

	g->stp.SetState(ST_ZTEST, true);
	g->stp.SetState(ST_CULL_FACE, true);
	g->stp.SetState(ST_ZWRITE, true);
	g->stp.PopMatrix();
	g->stp.PrepareForRender();

	if (g->scm.GetActiveScene())
	{
		g->scm.GetActiveScene()->OnEndRendering2D();
	}

	m_helperTextCount = 0;
}

void C2DScene::AddHelperText(std::wstring text, int x, int y, unsigned char _r, unsigned char _g, unsigned char _b)
{
	if (m_helperTextCount >= MAX_HELPER_TEXTS)
	{
		return;
	}

	m_texts[m_helperTextCount].text = text;
	m_texts[m_helperTextCount].x = x;
	m_texts[m_helperTextCount].y = y;
	m_texts[m_helperTextCount].r = _r;
	m_texts[m_helperTextCount].g = _g;
	m_texts[m_helperTextCount].b = _b;	
	m_helperTextCount++;
}

void C2DScene::PrintDebugInfo()
{
	g->lw.WriteLn("visible items: ", m_renderSpriteCount);

	USES_CONVERSION;
	for (int k = 0; k < m_renderSpriteCount; k++)
	if (m_renderQueue[k])
	{
		CBaseSprite* sprite = m_renderQueue[k]->sprite;
		CLogValue part1("name [", sprite->GetName(), "] x, y [", sprite->GetX(), ", ", sprite->GetY(), "] depth [", sprite->GetDepth(), "] ");

		CLogValue part2("width [", sprite->GetWidth(), "] height [", sprite->GetHeight(), "] ");		

		int s1, s2, s3, s4;
		sprite->GetScissors(s1, s2, s3, s4);
		CLogValue part3("scissors [", s1, ", ", s2, ", ", s3, ", ", s4, "]");

		CBaseSprite* maskSprite = sprite->GetMaskSprite();
		std::string name = "";
		if (maskSprite)
		{
			name = WCharStringToString(maskSprite->GetName());
		}
		CLogValue part4("maskSprite [", (int)maskSprite, ", ", name, "] ");

		g->lw.WriteLn(part1, part2, part3, part4);

		if (sprite->GetType() == SPRITE_TYPE_FORMATTED_TEXT)
		{
			CFormattedTextSprite* ftSprite = (CFormattedTextSprite*)sprite;
			ftSprite->PrintDebugInfo();
		}
		else if ((sprite->GetType() == SPRITE_TYPE_SPRITE) || (sprite->GetType() == SPRITE_TYPE_PAPER))
		{
			CSprite* imgSprite = (CSprite*)sprite;
			int textureID = imgSprite->GetTextureID(0);
			CTexture* texture = g->tm.GetObjectPointer(textureID);
			USES_CONVERSION;
			std::string name = WCharStringToString(texture->GetName());
			bool isDeleted = texture->IsDeleted();
			int tx1, ty1, tx2, ty2;
			imgSprite->GetWorkRect(&tx1, &ty1, &tx2, &ty2);
			CLogValue part("texture [", (int)textureID, ", ", name, ", ", (isDeleted ? 1 : 0), "] ");
			CLogValue part2("workrect [", tx1, ", ", ty1, ", ", tx2, ", ", ty2, "] ");
			g->lw.WriteLn(part, part2);
		}
	}

	g->lw.WriteLn("visible determination: ");
	std::vector<SSpriteDesc*>::iterator iter = m_sprites.begin();
	std::vector<SSpriteDesc*>::iterator iterEnd = m_sprites.end();

	for ( ; iter != iterEnd; iter++)
	{
		if (!(*iter)->sprite->IsVisible())
		{
			g->lw.WriteLn("name [", (*iter)->sprite->GetName(), "] non-visible");
			continue;
		}

		if (!(*iter)->sprite->IsLoaded())
		{
			g->lw.WriteLn("name [", (*iter)->sprite->GetName(), "] non-loaded");
			continue;
		}

		g->lw.WriteLn("name [", (*iter)->sprite->GetName(), "] visible");
	}
}

C2DScene::~C2DScene()
{
	MP_MAP<void *, SSpriteDesc*>::iterator iter = m_spriteDescs.begin();
	MP_MAP<void *, SSpriteDesc*>::iterator iterEnd = m_spriteDescs.end();

	for ( ; iter != iterEnd; iter++)
	{
		MP_DELETE((*iter).second);
	}

	if (m_helperFont)
	{
		g->fm.DeleteFont(m_helperFont);
	}
}