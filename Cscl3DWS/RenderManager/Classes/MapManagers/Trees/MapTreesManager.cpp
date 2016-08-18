#include "stdafx.h"
#include "MapTreesManager.h"
//#include "n3dpluginsupport.h"
#include "nrmPlugin.h"
#include "../mapmanager.h"
#include "../ZoneObjectsTypes.h"

#define PLUGIN_PATH_TREES	L"ui:resources\\Trees\\treesgen.dll"
#define PLUGIN_PATH_GRASS	L"ui:resources\\Trees\\treesgen.dll"

CMapTreeManager::CMapTreeManager(CMapManager* apmapmanager)
{
	ATLASSERT( apmapmanager);
	map_manager = apmapmanager;
//	pifaketreesobject = NULL;
	pifaketreeplugin = NULL;
//	pifakegrassobject = NULL;
	pifakegrassplugin = NULL;
	m_bforest_preset_loaded = false;
}

CMapTreeManager::~CMapTreeManager(void)
{
}

void	CMapTreeManager::initialize_manager()
{
#ifdef ALLOW_TREES
	ATLASSERT(map_manager);
/*	if (!pifaketreesobject)
	{
		create_plugin( PLUGIN_PATH_TREES, "mmlabTreesGenPlugin", &pifaketreeplugin, &pifaketreesobject);
	}
	if (!pifakegrassobject)
	{
		create_plugin( PLUGIN_PATH_GRASS, "mmlabGrassGenPlugin", &pifakegrassplugin, &pifakegrassobject);
	}*/
#endif
}

/*bool	CMapTreeManager::create_plugin( const wchar_t* path, const CComString  uid, nrmPlugin** ppifakeplugin, iobject3d** ppifakeobject)
{
#ifdef ALLOW_TREES
	ATLASSERT( path);
	if(!path)	return false;
	ATLASSERT( !uid.IsEmpty());
	if(uid.IsEmpty())	return false;
	ATLASSERT( ppifakeobject);
	ATLASSERT( ppifakeplugin);

	*ppifakeobject = map_manager->create_and_add_fake_object3d_to_scene();
	*ppifakeplugin = map_manager->create_plugin();

	nrmPlugin* pifakeplugin = *ppifakeplugin;
	//iobject3d* pifakeobject = *ppifakeobject;

	pifakeplugin->LoadLibrary( path);
	if( !pifakeplugin->IsLoaded())
	{
		//ATLASSERT( FALSE);
		return false;
	}
	pifakeplugin->SetUID(uid);
	pifakeplugin->InitializeDll();

/*	ipluginsupport*	pipluginsupport = NULL;
	pifakeobject->query_interface(iid_ipluginsupport, (void**)&pipluginsupport);

	ibaseplugin*	pibaseplugin=NULL;
	irmbaseplugin*  pirmbaseplugin=pifakeplugin->GetPluginInterface();
	ATLASSERT( pirmbaseplugin);
	pirmbaseplugin->query_interface(iid_ibaseplugin, (void**)&pibaseplugin);
	ATLASSERT( pibaseplugin);
	if( pipluginsupport && pibaseplugin)
	{
		pipluginsupport->register_plugin(pibaseplugin);
	}*/
/*#endif
	return true;
}*/

void	CMapTreeManager::add_tree( ZONE_OBJECT_INFO*	ainfo)
{
#ifdef ALLOW_TREES
	ATLASSERT( ainfo);
	ATLASSERT( ainfo->type == vespace::VE_TREE);
	ATLASSERT( pifaketreeplugin);
//	ATLASSERT( pifaketreesobject);
	//
	CComString	sTreesPresetName = "deftrees.ini";
	//CComString	sForestPresetName = "defforest.ini";
	CComString	sTreeType = ainfo->objectName;
	/*
	if( !ainfo->params.IsEmpty())
	{
		// ?? parse tree params
	}
	*/
	// pend tree to loading queue
	
	// тут нужно обеспечить загрузку файлов конфигурации

	//
	TREE_DESC	desc;
	desc.sPresetPath = "ui:resources\\Trees\\presets.ini";
	desc.sType = ainfo->objectName;
	desc.x = ainfo->translation.x;
	desc.y = ainfo->translation.y;
	desc.z = ainfo->translation.z;
	pending_trees.push_back( desc);
	update_pending_queue();
#endif
}

void	CMapTreeManager::plant_grass_on_named_object( LPCTSTR lpobjectname)
{
#ifdef ALLOW_TREES
	ATLASSERT(lpobjectname);
	if( !lpobjectname)	return;

	/*
	arguments: "named_object", object_name,
	"type", "billboard | random",
	"grass_height", height
	"grass_step", step
	*/
	int iArgC = 9;
	va_list	valist = new char[iArgC*_INTSIZEOF(double)+2];
	char* ap = valist;

	wchar_t*	wcProcName = L"named_object";

	wchar_t*	wcParamDef_type = L"type";
	wchar_t*	wcParamValue_type = L"random";

	wchar_t*	wcParamDef_grass_height = L"grass_height";
	wchar_t*	wcParamValue_grass_height = L"50";

	wchar_t*	wcParamDef_grass_width = L"grass_width";
	wchar_t*	wcParamValue_grass_width = L"40";

	wchar_t*	wcParamDef_grass_step = L"grass_step";
	wchar_t*	wcParamValue_grass_step = L"50";

	USES_CONVERSION;
	// name
	*(wchar_t**)ap=wcProcName;	ap+=_INTSIZEOF(wchar_t*);
	*(wchar_t**)ap=A2W(lpobjectname);	ap+=_INTSIZEOF(wchar_t*);
	// type
	*(wchar_t**)ap=wcParamDef_type;	ap+=_INTSIZEOF(wchar_t*);
	*(wchar_t**)ap=wcParamValue_type;	ap+=_INTSIZEOF(wchar_t*);
	// grass_height
	*(wchar_t**)ap=wcParamDef_grass_height;	ap+=_INTSIZEOF(wchar_t*);
	*(wchar_t**)ap=wcParamValue_grass_height;	ap+=_INTSIZEOF(wchar_t*);
	// width
	*(wchar_t**)ap=wcParamDef_grass_width;	ap+=_INTSIZEOF(wchar_t*);
	*(wchar_t**)ap=wcParamValue_grass_width;	ap+=_INTSIZEOF(wchar_t*);
	// grass_step
	*(wchar_t**)ap=wcParamDef_grass_step;	ap+=_INTSIZEOF(wchar_t*);
	*(wchar_t**)ap=wcParamValue_grass_step;	ap+=_INTSIZEOF(wchar_t*);

	unsigned long aRes;
	pifakegrassplugin->Call( "", iArgC, valist, aRes);

	delete[] valist;
#endif
}

void	CMapTreeManager::update_pending_queue()
{ 
#ifdef ALLOW_TREES
	if( !m_bforest_preset_loaded)
	{
		USES_CONVERSION;
		wchar_t*	wcProcName = L"loadSingleForestPreset";
		wchar_t*	wcPresetFileName = L"ui:resources/trees/forest.ini";
		int iArgC = 2;
		va_list	valist = new char[iArgC*_INTSIZEOF(double)+2];
		char* ap = valist;
		*(wchar_t**)ap=wcProcName;	ap+=_INTSIZEOF(wchar_t*);
		*(wchar_t**)ap=wcPresetFileName;

		unsigned long aRes;
		pifaketreeplugin->Call( "", iArgC, valist, aRes);
		delete[] valist;
		m_bforest_preset_loaded = true;
	}

	// тут надо проверить, а скачан ли сейчас нужный пресет на локальный диск
#pragma message("Check Trees Preset loading..")

	std::vector<TREE_DESC>::iterator	it = pending_trees.begin(),
										end = pending_trees.end();
	for ( ;it!=end; it++)
	{
		// arguments: "addTree", presetFileName, treeType, x, y, z
		USES_CONVERSION;
		wchar_t*	wcProcName = L"addSingleTree";
		wchar_t*	wcPresetFileName = A2W( it->sPresetPath);
		wchar_t*	wcType = A2W( it->sType);
		int iArgC = 6;
		va_list	valist = new char[iArgC*_INTSIZEOF(double)+2];
		char* ap = valist;
		*(wchar_t**)ap=wcProcName;	ap+=_INTSIZEOF(wchar_t*);
		*(wchar_t**)ap=wcPresetFileName;	ap+=_INTSIZEOF(wchar_t*);
		*(wchar_t**)ap=wcType;	ap+=_INTSIZEOF(wchar_t*);
		*(double*)ap=(double)it->x;	ap+=_INTSIZEOF(double);
		*(double*)ap=(double)it->y;	ap+=_INTSIZEOF(double);
		*(double*)ap=(double)it->z;

		unsigned long aRes;
		pifaketreeplugin->Call( "", iArgC, valist, aRes);
		pending_trees.erase( it);

		delete[] valist;
	}
#endif
}

/*ivisibleobject*		CMapTreeManager::get_visible_trees_object()
{
#ifdef ALLOW_TREES
	ATLASSERT( pifaketreesobject);
	ivisibleobject* apivisibleobject = NULL;
	if( GET_INTERFACE(ivisibleobject, pifaketreesobject, &apivisibleobject))
		return apivisibleobject;
	ATLASSERT( FALSE);
#endif
	return NULL;
}


ivisibleobject*		CMapTreeManager::get_visible_grass_object()
{
#ifdef ALLOW_TREES
	ATLASSERT( pifakegrassobject);
	ivisibleobject* apivisibleobject = NULL;
	if( GET_INTERFACE(ivisibleobject, pifakegrassobject, &apivisibleobject))
		return apivisibleobject;
	ATLASSERT( FALSE);
#endif
	return NULL;
}*/