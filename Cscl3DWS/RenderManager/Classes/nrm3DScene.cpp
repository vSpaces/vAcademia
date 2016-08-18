// nrm3DScene.cpp: implementation of the nrm3DScene class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XMLList.h"
#include "UserData.h"
#include "nrmCamera.h"
#include "nrm3DScene.h"
#include "HelperFunctions.h"
#include "PlatformDependent.h"
#include "MapManagers/MapManager.h"
#include "GlobalSingletonStorage.h"

nrm3DScene::nrm3DScene(mlMedia* apMLMedia): 
	nrmObject(apMLMedia),
	m_mapManager(NULL),
	m_scene(NULL)
{
}

void	nrm3DScene::OnSetRenderManager()
{
	if (!GetMLMedia())
	{
		return;
	}

	wchar_t* name = GetMLMedia()->GetStringProp("name");

	std::string _name;
	if (name)
	{
		USES_CONVERSION;
		_name = W2A(name);
	}
	else
	{
		_name = "scene_" + IntToStr(g->scm.GetCount());
	}

	int sceneID = g->scm.AddObject(_name);
	m_scene = g->scm.GetObjectPointer(sceneID);

	m_scene->SetUserData(USER_DATA_NRMOBJECT, this);

	g->scm.SetActiveScene(sceneID);
	g->pf.Init();

	gRM->nrMan->LoadAndApplyUserSettings();

	PreloadClasses();
}

void		nrm3DScene::PreloadClasses()
{
	if( GetRenderManager() && GetRenderManager()->GetContext() && GetRenderManager()->GetContext()->mpLogWriter)
		GetRenderManager()->GetContext()->mpLogWriter->WriteLn("Start preloading classes");

	std::string className;
	CXMLList classList(GetApplicationRootDirectory() + L"preload_classes.xml", true);
	while (classList.GetNextValue(className))
	{
		gRM->mapMan->LoadClassFile(className.c_str());
	}
}

void		nrm3DScene::SetMapManager( CMapManager* aMapManager)
{
	m_mapManager = aMapManager;
}

void nrm3DScene::update(DWORD delta)
{
	if (!m_scene) 
	{
		return;
	}

	if (!m_mapManager)
	{
		return;
	}

	m_mapManager->Update(delta);
}

void nrm3DScene::PropIsSet()
{
	if (m_mapManager)
	if ((IsSet("reload_objects")) && (GetIntProp("reload_objects") == 1))
	{
		m_mapManager->ReloadZonesInfo();
	}

	nrmObject::PropIsSet();
}

nrm3DScene::~nrm3DScene()
{
	m_scene->SetUserData(USER_DATA_NRMOBJECT, NULL);
	g->scm.DeleteObject(m_scene->GetID());
}