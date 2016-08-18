
#include "StdAfx.h"
#include "XMLList.h"
#include "PlatformDependent.h"
#include "TexturesPreloader.h"
#include "GlobalSingletonStorage.h"

CTexturesPreloader::CTexturesPreloader()
{
}

void CTexturesPreloader::LoadAll()
{
	bool isKeepInMemory = !g->cc.IsTextureRenderingSupported();

	CXMLList texturesList(GetApplicationRootDirectory() + L"textures_preload.xml", true);
	std::string texturePath;
	while (texturesList.GetNextValue(texturePath))
	{
		CNTextureLoadingInfo info;
		info.SetCanBeCompressed(!isKeepInMemory);
		info.SetMipMapStatus(false);		
		info.SetKeepInSystemMemory(isKeepInMemory);		
		info.SetCanBeResized(isKeepInMemory);
		std::string path = texturePath;
		int textureID = g->tm.LoadAsynch(texturePath, path, NULL, true, &info);
		CTexture* texture = g->tm.GetObjectPointer(textureID);
		texture->AddRef();
		texture->SetHasNoMipmaps(true);

		if (isKeepInMemory)
		{
			texture->SetPreMultiplyStatus(true);
		}
	}
}

CTexturesPreloader::~CTexturesPreloader()
{}