
#include "StdAfx.h"
#include "XMLList.h"
#include "CubeMapManager.h"
#include "PlatformDependent.h"
#include "cal3d/memory_leak.h"

CCubeMapManager::CCubeMapManager()
{
	ReserveMemory(10);
}

void CCubeMapManager::LoadAll()
{
	CXMLList cubemapsList(GetApplicationRootDirectory() + L"cubemaps.xml", true);

	std::string fileName;

	while (cubemapsList.GetNextValue(fileName))
	{
		AddObject(fileName);
	}
}

void CCubeMapManager::AddCubeMap(std::string name, int textureID)
{
	int id = GetObjectNumber(name);
	if (-1 == id)
	{
		id = AddObject(name);
	}
	CCubeMap* cubeMap = GetObjectPointer(id);
	cubeMap->SetCubeMapID(textureID);
}

void CCubeMapManager::AddCubeMap(std::wstring name, int textureID)
{
	int id = GetObjectNumber(name);
	if (-1 == id)
	{
		id = AddObject(name);
	}
	CCubeMap* cubeMap = GetObjectPointer(id);
	cubeMap->SetCubeMapID(textureID);
}

CCubeMapManager::~CCubeMapManager()
{

}