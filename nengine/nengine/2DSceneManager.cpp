
#include "StdAfx.h"
#include <string>
#include <Assert.h>
#include "XMLList.h"
#include "2DSceneManager.h"
#include "PlatformDependent.h"
#include "cal3d/memory_leak.h"

C2DSceneManager::C2DSceneManager():
	m_activeSceneID(0)
{
	ReserveMemory(0);
}

void C2DSceneManager::LoadAll()
{
	CXMLList scenes2d(GetApplicationRootDirectory() + L"scenes2d.xml", false);

	std::string fileName; 

	while (scenes2d.GetNextValue(fileName))
	{
		int id = AddObject(fileName);
		C2DScene* scene2d = GetObjectPointer(id);
		scene2d->LoadFromXMLFile(fileName);
	}
}

void C2DSceneManager::Draw()
{
	C2DScene* scene2d = GetObjectPointer(m_activeSceneID);
	scene2d->Draw();
}

void C2DSceneManager::SetActive2DScene(std::string fileName)
{
	m_activeSceneID = GetObjectNumber(fileName);
	assert(m_activeSceneID != -1);
}

C2DScene* C2DSceneManager::GetActive2DScene()
{
	assert(m_activeSceneID != -1);
	return GetObjectPointer(m_activeSceneID);
}

C2DSceneManager::~C2DSceneManager()
{

}