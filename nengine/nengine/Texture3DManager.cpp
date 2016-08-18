
#include "StdAfx.h"
#include "XMLList.h"
#include "Texture3DManager.h"
#include "GlobalSingletonStorage.h"
#include "PlatformDependent.h"
#include "cal3d/memory_leak.h"

CTexture3DManager::CTexture3DManager()
{
	ReserveMemory(10);
}

void CTexture3DManager::LoadAll()
{
	if (!g->ei.IsExtensionSupported(GLEXT_ext_texture3d))
	{
		return;
	}

	if (!g->rs.IsNotOnlyLowQualitySupported())
	{
		return;
	}

	CXMLList texturesList(GetApplicationRootDirectory() + L"textures3d.xml", true);

	std::string fileName;

	while (texturesList.GetNextValue(fileName))
	{
		GetObjectNumber(fileName);
	}
}

int CTexture3DManager::GetObjectNumber(std::string name)
{
	int res = CManager<CTexture3D>::GetObjectNumber(name);

	if (res == ERR_OBJECT_NOT_FOUND)
	{
		res = AddObject(name);

		USES_CONVERSION;
		int textureNum = CreateTexture3D(false, (GetApplicationRootDirectory() + L"external\\textures3d\\" + A2W(name.c_str())).data());
		CTexture3D* texture3D = GetObjectPointer(res);
		texture3D->SetTextureNumber(textureNum);

		int res2 = AddObject(name + "_mipmap");

		textureNum = CreateTexture3D(true, (GetApplicationRootDirectory() + L"external\\textures3d\\" + A2W(name.c_str())).data());
		CTexture3D* _texture3D = GetObjectPointer(res2);
		_texture3D->SetTextureNumber(textureNum);
	}

	return res;
}

void CTexture3DManager::BindTexture3D(int num)
{
	if (num >= GetCount())
	{
		return;
	}

	CTexture3D* texture3D = GetObjectPointer(num);
	int num2 = texture3D->GetTextureNumber();

	GLFUNC(glBindTexture)(GL_TEXTURE_3D, num2);
}

CTexture3DManager::~CTexture3DManager()
{
}