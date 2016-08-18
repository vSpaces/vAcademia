#include "stdAfx.h"
#include "PropertyValue.h"

CPropertyValue::CPropertyValue()
:MP_WSTRING_INIT(wcValue)
{
	propertyList = NULL;
}

CPropertyValue::~CPropertyValue()
{
	if(propertyList!=NULL)
		MP_DELETE( propertyList);
}

void CPropertyValue::operator=(const CPropertyValue &aPropertyValue)
{
	type = aPropertyValue.type;
	switch(type)
	{
	case UT_BOOL:
		{
			bValue = aPropertyValue.bValue;
			break;
		}
	case UT_INT:
		{
			iValue = aPropertyValue.iValue;
			break;
		}
	case UT_DOUBLE:
		{
			dValue = aPropertyValue.dValue;
			break;
		}
	case UT_STRING:
		{
			break;
		}
	}
}