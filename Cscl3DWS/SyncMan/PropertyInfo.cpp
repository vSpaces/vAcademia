#include "StdAfx.h"
#include "propertyinfo.h"

CPropertyInfo::CPropertyInfo()
{
	name = NULL;
	value = NULL;
}

CPropertyInfo::~CPropertyInfo()
{
	if(name!=NULL)
		MP_DELETE_ARR( name);
	if(value!=NULL)
		MP_DELETE( value);
}

void CPropertyInfo::SetName(wchar_t *aName)
{
	name = aName;
}

void CPropertyInfo::SetValue(CPropertyValue *aValue)
{
	value = aValue;
}

bool CPropertyInfo::GetNameLength( unsigned short& aValue)
{
	if(name==NULL)
		return false;
	aValue = wcslen(name);
	return true;
}

bool CPropertyInfo::GetNameData( wchar_t* &aValue)
{
	aValue = name;
	return true;
}

bool CPropertyInfo::GetType( unsigned char& aValue)
{
	aValue = (unsigned char)value->type;
	return true;
}

bool CPropertyInfo::GetValueBool(bool &aValue)
{
	aValue = value->bValue;
	return true;
}

bool CPropertyInfo::GetValueInt(int &aValue)
{
	aValue = value->iValue;
	return true;
}

bool CPropertyInfo::GetValueDouble(double &aValue)
{
	aValue = value->dValue;
	return true;
}

bool CPropertyInfo::GetValueStringLength(unsigned short &aValue)
{
	if(value->wcValue.empty())
		return false;
	aValue = wcslen(value->wcValue.c_str());
	return true;
}

bool CPropertyInfo::GetValueString(const wchar_t* &aValue)
{
	aValue = value->wcValue.c_str();
	return true;
}

bool CPropertyInfo::GetValueObject(syncIPropertyList* &aProperties)
{
	aProperties = value->propertyList;
	return true;
}
