#include "stdafx.h"
#include "MapTreeObject.h"

CMapTreeObject::CMapTreeObject(CMapTreeManager* apmapmanager)
{
	ATLASSERT( apmapmanager);
	map_manager = apmapmanager;
}

CMapTreeObject::~CMapTreeObject(void)
{
} 
