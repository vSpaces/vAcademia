
#pragma once

#include "CommonEngineHeader.h"

#include "Polygons2DAccum.h"
#include "CommonObject.h"
#include "TextSprite.h"
#include "BaseSprite.h"
#include "Texture.h"
#include <string>
#include "Line.h"
#include <map>

#define MAX_SPRITE_COUNT	4096
#define MAX_HELPER_TEXTS	16

#pragma  warning (disable: 4786)

typedef struct _SHelperText
{
	MP_WSTRING text;
	int x, y;
	unsigned char r, g, b;

	_SHelperText();
	_SHelperText(const _SHelperText& txt);
} SHelperText;

typedef struct _SSpriteDesc
{	
	CBaseSprite* sprite;
	char isSavedVisible;	
	
	_SSpriteDesc()
	{
		isSavedVisible = -1;
		sprite = NULL;
	}
} SSpriteDesc;

class C2DScene : public CCommonObject
{
public:
	C2DScene();
	~C2DScene();

	void LoadFromXMLFile(std::string fileName);
	void Draw();
	void DrawImmediately();	

	bool AddSprite(int spriteID, short x, short y);
	bool DeleteSprite(int spriteID);

	bool AddSprite(std::string name, short x, short y);
	bool DeleteSprite(std::string name);
	bool DeleteSprite(std::wstring name);

	bool AddTextSprite(CTextSprite* text, short x, short y);
	bool DeleteTextSprite(CTextSprite* text);

	bool AddLine(CLine* line, short x, short y);
	bool DeleteLine(CLine* line);

	void SaveVisibility();
	void RestoreVisibility();
	void HideAll();

	void OnChanged();
	void OnParamsChanged();

	void SetVisible(bool isVisible);
	void SetVisibleNow(bool isVisible);
	bool IsVisible()const;

	__forceinline void AddDynamicTexture(CTexture* texture) 
	{
		m_dynamicTextures.push_back(texture);
	}

	void PrintDebugInfo();

	void AddHelperText(std::wstring text, int x, int y, unsigned char _r, unsigned char _g, unsigned char _b);	

private:
	void RenderHelperTexts();

	MP_VECTOR<CTexture *> m_dynamicTextures;
	MP_MAP<void *, SSpriteDesc*> m_spriteDescs;
	MP_VECTOR<SSpriteDesc*> m_sprites;

	bool m_isNeedToUpdateAnyway;
	bool m_isNeedToAccumAnyway;

	int m_renderSpriteCount;
	__int64 m_lastUpdateTime;
	SSpriteDesc* m_renderQueue[MAX_SPRITE_COUNT];

	bool m_isVisible;
	bool m_isVisibleNow;

	CPolygons2DAccum m_accum;

	SHelperText m_texts[MAX_HELPER_TEXTS];
	int m_helperTextCount;

	CFont* m_helperFont;
};