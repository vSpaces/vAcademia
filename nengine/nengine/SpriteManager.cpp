
#include "StdAfx.h"
#include "XMLList.h"
#include "SpriteManager.h"
#include "PlatformDependent.h"
#include "cal3d/memory_leak.h"

CSpriteManager::CSpriteManager()
{	
	ReserveMemory(16000);
}

void CSpriteManager::LoadAll()
{
	CXMLList sprites(GetApplicationRootDirectory() + L"sprites.xml", false);

	std::string fileName;

	while (sprites.GetNextValue(fileName))
	{
		int id = AddObject(fileName);
		CSprite* sprite = GetObjectPointer(id);
		sprite->LoadSpriteFromXML(fileName);
	}
}

CSpriteManager::~CSpriteManager()
{
}