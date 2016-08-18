
#pragma once

#include "Sprite.h"
#include "Manager.h"

class CSpriteManager : public CManager<CSprite>
{
public:
	CSpriteManager();
	~CSpriteManager();

	void LoadAll();
};