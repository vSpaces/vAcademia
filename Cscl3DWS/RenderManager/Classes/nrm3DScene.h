
#pragma once

#include "nrmObject.h"
#include "Scene.h"

class CMapManager;

class nrm3DScene : public nrmObject
{
public:
	nrm3DScene(mlMedia* apMLMedia);
	virtual ~nrm3DScene();

	void		update(DWORD delta);
	void		OnSetRenderManager();
	void		SetMapManager(CMapManager* aMapManager);

private:
	void PreloadClasses();

	// реализация moMedia
public:
	virtual	void PropIsSet();	

	CScene*			m_scene;
	CMapManager*	m_mapManager;
};
