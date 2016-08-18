#pragma once

#include "Manager.h"
#include "Scene.h"

class CSceneManager : public CManager<CScene>
{
friend class CGlobalSingletonStorage;

public:
	void SetActiveScene(int sceneID);
	int GetActiveSceneID();
	CScene* GetActiveScene();

	void RenderActiveScene();

private:
	CSceneManager();
	CSceneManager(const CSceneManager&);
	CSceneManager& operator=(const CSceneManager&);
	~CSceneManager();

	int m_activeSceneID;
};