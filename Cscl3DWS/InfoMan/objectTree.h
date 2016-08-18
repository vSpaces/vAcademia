#pragma once

#include "oms_context.h"
#include "rmml.h"
#include <map>

using namespace rmml;

char* WC2MB(const wchar_t* apwc);
typedef MP_MAP<mlString, HTREEITEM> CParentMap;

class CObjectTree : public CTreeViewCtrl
{
public:
	CObjectTree();
	void Build();
	int GetFilter();
	void SetFilter( int aFilter);

	void Find( CComString aFindFilter);
	void ShowObject(const rmml::mlString& aTarget);

	void SetContext( oms::omsContext *apContext)
	{
		pContext = apContext;
	}

private:
	int filter;
	oms::omsContext *pContext;
	MP_VECTOR<mlObjectInfo> vElements;
	int typeFind;
	typedef CParentMap::const_iterator CParentMapIterator;
	CParentMap parentMap;


	void AddObject( mlObjectInfo *apObjectInfo);
	bool CompareFindFilter( CComString &findFilter, CComString &itemText);
};
