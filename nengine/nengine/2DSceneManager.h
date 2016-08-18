
#pragma once

#include "2DScene.h"
#include "Manager.h"

class C2DSceneManager : public CManager<C2DScene>
{
public:
	C2DSceneManager();
	~C2DSceneManager();

	void LoadAll();
	void Draw();

	void SetActive2DScene(std::string fileName);
	C2DScene* GetActive2DScene();

private:
	int m_activeSceneID;
};