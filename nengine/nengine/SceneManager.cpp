
#include "StdAfx.h"
#include "SceneManager.h"
#include "cal3d/memory_leak.h"

CSceneManager::CSceneManager():
	m_activeSceneID(-1)
{
	ReserveMemory(2);
}

void CSceneManager::SetActiveScene(int sceneID)
{
	assert(sceneID >= 0);
	assert(sceneID < GetCount());

	m_activeSceneID = sceneID;
}

int CSceneManager::GetActiveSceneID()
{
	return m_activeSceneID;
}

CScene* CSceneManager::GetActiveScene()
{
	if (m_activeSceneID >= 0)
	{
		CScene* scene = GetObjectPointer(m_activeSceneID);
		if (scene->IsDeleted())
		{
			return NULL;
		}

		return scene;
	}
	else
	{
		return NULL;
	}
}

void CSceneManager::RenderActiveScene()
{
	if (m_activeSceneID == -1)
	{
		return;
	}

	CScene* scene = GetObjectPointer(m_activeSceneID);
	assert(scene);

	scene->Render();
}

CSceneManager::~CSceneManager()
{

}
