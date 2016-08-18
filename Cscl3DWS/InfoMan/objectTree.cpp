
#include "StdAfx.h"
#include "objectTree.h"

CObjectTree::CObjectTree()
:MP_VECTOR_INIT(vElements), MP_MAP_INIT(parentMap)

{
	pContext = NULL;
	filter = 0;
}

char* WC2MB(const wchar_t* apwc){
	int iSLen=wcslen(apwc);
	char* pch = MP_NEW_ARR( char, iSLen*MB_CUR_MAX+1);
	wcstombs(pch, apwc, iSLen);
	pch[iSLen]=0;
	return pch;
}

void CObjectTree::AddObject( mlObjectInfo *apObjectInfo)
{
	wchar_t *lp = (wchar_t *)apObjectInfo->name.c_str();
	char* p = WC2MB(lp);
	TVINSERTSTRUCT tvis;
	CParentMapIterator parentIt = parentMap.find( apObjectInfo->parent);
	if (parentIt == parentMap.end())
		tvis.hParent = TVI_ROOT;
	else
		tvis.hParent = parentIt->second;
	tvis.hInsertAfter = TVI_LAST;
	tvis.item.mask = TVIF_TEXT;
	tvis.item.pszText = (LPTSTR) p;
	tvis.item.iImage = 0;
	tvis.item.iSelectedImage = 0;
	tvis.item.state = 0;
	tvis.item.stateMask = 0;
	tvis.item.lParam = 0;
	HTREEITEM hItem = InsertItem( &tvis);
	DWORD_PTR ptr = (unsigned long)apObjectInfo;
	SetItemData(hItem, ptr);
	parentMap[ apObjectInfo->src] = hItem;
}

void CObjectTree::Build()
{
	ATLASSERT(pContext!=NULL);
	DeleteAllItems();
	vElements.clear();
	parentMap.clear();
	pContext->mpSM->GetRMMLObjectTree( vElements, filter);
	for(int i=0;i<vElements.size();i++)
	{
		AddObject( &vElements[i]);
	}
}

int CObjectTree::GetFilter()
{
	return filter;
}

void CObjectTree::SetFilter( int aFilter)
{
	filter = aFilter;
	Build();
}

bool CObjectTree::CompareFindFilter( CComString &findFilter, CComString &itemText)
{
	switch( typeFind)
	{
		case 3:
		{
			if(itemText.Find(findFilter, 0)>-1)
				return true;
			break;
		}
		case 2:
		{
			if(itemText.Find(findFilter, 0) == 0)
				return true;
			break;
		}
		case 1:
		{
			if(itemText.Find(findFilter, 0) >-1 
				&& findFilter[findFilter.GetLength() - 1] == itemText[itemText.GetLength() - 1])
				return true;
			break;
		}
		default:
		{
			if(findFilter == itemText)
				return true;
		}
	}
	return false;
}

void CObjectTree::Find( CComString aFindFilter)
{
	if( aFindFilter[0]=='*' && aFindFilter[aFindFilter.GetLength() - 1]=='*')
	{
		typeFind = 3;
		aFindFilter = aFindFilter.Mid(1, aFindFilter.GetLength() - 2);
	}
	else if( aFindFilter[aFindFilter.GetLength() - 1]=='*')
	{
		typeFind = 2;
		aFindFilter = aFindFilter.Left(aFindFilter.GetLength() - 2);
	}
	else if( aFindFilter[0]=='*')
	{
		aFindFilter = aFindFilter.Right(aFindFilter.GetLength() - 1);
		typeFind = 1;
	}
	else
		typeFind = 0;
	HTREEITEM item = NULL;
	HTREEITEM itemSel = item = GetSelectedItem();
	bool bSelected = true;
	bool bFind = false;
	if( item != NULL)
		item = GetNextSiblingItem( item);

	if(item == NULL)
	{
		item = GetChildItem( TVI_ROOT);
		bSelected = false;
	}
		
	while( item != NULL)
	{
		CComString s;
		GetItemText( item, s.GetBufferSetLength(1024), 1023);
		if(CompareFindFilter(aFindFilter, s))
		{
			SelectItem( item);
			bFind = true;
			break;
		}
		item = GetNextSiblingItem( item);
	}
	if( bSelected && !bFind)
	{
		item = GetChildItem( TVI_ROOT);
		while( item != NULL && item != itemSel)
		{
			CComString s;
			GetItemText( item, s.GetBufferSetLength(1024), 1023);
			if(CompareFindFilter(aFindFilter, s))
			{
				SelectItem( item);
				bFind = true;
				break;
			}
			item = GetNextSiblingItem( item);
		}
	}
	if( !bFind)
	{
		CComString s = "Такого элемента: \"" + aFindFilter + "\" нет";
		::MessageBox(m_hWnd, s, "Не найден", MB_APPLMODAL | MB_ICONWARNING);
	}
}

void CObjectTree::ShowObject(const rmml::mlString& aTarget)
{
	HTREEITEM item = NULL;
	item = GetChildItem( TVI_ROOT);
	while (item != NULL)
	{
		mlObjectInfo* pObjectInfo = (mlObjectInfo*) GetItemData( item);
		if (pObjectInfo != NULL && aTarget == pObjectInfo->src)
			break;
		item = GetNextSiblingItem( item);
	}
	if (item != NULL)
		SelectItem( item);
}