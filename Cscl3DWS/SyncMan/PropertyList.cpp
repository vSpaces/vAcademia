#include "StdAfx.h"
#include "propertyList.h"

CPropertyList::CPropertyList(short aPropCount)
{
	propertiesInfo = NULL;
	CreateList(aPropCount);
}

CPropertyList::~CPropertyList()
{
	if(propertiesInfo!=NULL)
		MP_DELETE_ARR( propertiesInfo);
}

void CPropertyList::CreateList(short aPropCount)
{
	propCount=aPropCount;
	num = 0;
	if(propertiesInfo!=NULL)
		MP_DELETE_ARR( propertiesInfo);
	propertiesInfo = MP_NEW_ARR( CPropertyInfo,propCount);
}

void CPropertyList::AddProperty(wchar_t *aName, CPropertyValue *aVal)
{
	if(num>=propCount)
		return;
	propertiesInfo[num].SetName(aName);
	propertiesInfo[num].SetValue(aVal);
	num++;
}

unsigned short CPropertyList::GetCount()
{
	return propCount;
}

syncIPropertyInfo *CPropertyList::GetProperty(int auIndex)
{
	if(auIndex>=propCount)
		return NULL;
	return &propertiesInfo[auIndex];
}
