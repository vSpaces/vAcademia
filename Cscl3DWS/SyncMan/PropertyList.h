#pragma once
#include "SyncProtocol.h"
#include "PropertyInfo.h"
#include "PropertyValue.h"

using namespace SyncManProtocol;

class CPropertyList : public syncIPropertyList
{
	friend class CPropertyInfo;
	friend class CPropertyValue;
public:
	CPropertyList(short aPropCount);
	virtual public ~CPropertyList();
	void AddProperty(wchar_t *aName, CPropertyValue *aVal);
private:
	void CreateList(short aPropCount);

private:
	CPropertyInfo *propertiesInfo;
	unsigned short propCount;
	unsigned short num;

public:
	virtual unsigned short GetCount();
	virtual syncIPropertyInfo *GetProperty(int auIndex);
};
