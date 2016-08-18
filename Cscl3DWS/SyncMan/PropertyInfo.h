#pragma once
#include "SyncProtocol.h"
#include "PropertyValue.h"

using namespace SyncManProtocol;

class CPropertyInfo : public syncIPropertyInfo
{
	friend class CPropertyValue;
public:
	CPropertyInfo();
	virtual public ~CPropertyInfo();
	void SetName(wchar_t *aName);
	void SetValue(CPropertyValue *aValue);

private:
	wchar_t* name;
	CPropertyValue *value;

private:
	//bool AnalyseProperties(int &idx);

public:
	virtual bool GetNameLength( unsigned short& aValue);
	virtual bool GetNameData( wchar_t* &aValue);
	virtual bool GetType( unsigned char &aValue);
	virtual bool GetValueBool(bool &aValue);
	virtual bool GetValueInt(int &aValue);
	virtual bool GetValueDouble(double &aValue);
	virtual bool GetValueStringLength(unsigned short &aValue);
	virtual bool GetValueString(const wchar_t* &aValue) ;
	virtual bool GetValueObject( syncIPropertyList* &aProperties);
};
