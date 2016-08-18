#pragma once

#define	ALLOW_TREES

//////////////////////////////////////////////////////////////////////////
#include "../MapManagerZone.h"

//////////////////////////////////////////////////////////////////////////
//using namespace natura3d;

//////////////////////////////////////////////////////////////////////////
class CMapManager;
class nrmPlugin;

//////////////////////////////////////////////////////////////////////////
typedef struct _TREE_DESC
{
	CComString	sType;
	CComString	sPresetPath;
	float		x, y, z;
} TREE_DESC;

//////////////////////////////////////////////////////////////////////////
class CMapTreeManager
{
public:
	CMapTreeManager(CMapManager* apmapmanager);
	~CMapTreeManager(void);

public:
	void	initialize_manager();
	void	add_tree(ZONE_OBJECT_INFO*	ainfo);
	void	plant_grass_on_named_object( LPCTSTR lpobjectname);
/*	ivisibleobject*		get_visible_trees_object();
	ivisibleobject*		get_visible_grass_object();*/

protected:
	void	update_pending_queue();
//	bool	create_plugin( const wchar_t* path, const CComString  uid, nrmPlugin** ppifakeplugin, iobject3d** ppifakeobject);

protected:
	CMapManager*	map_manager;
	// trees
	//iobject3d*		pifaketreesobject;
	nrmPlugin*		pifaketreeplugin;
	// trees
	//iobject3d*		pifakegrassobject;
	nrmPlugin*		pifakegrassplugin;
	// loading pending queue
	std::vector<TREE_DESC>	pending_trees;
	bool			m_bforest_preset_loaded;
};
