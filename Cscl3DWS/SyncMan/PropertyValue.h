#pragma once

#include "SyncProtocol.h"
#include "PropertyList.h"
#include <string>
#include <xstring>

using namespace SyncManProtocol;

class CPropertyValue
{
	friend class CPropertyList;
public:
	CPropertyValue();
	virtual ~CPropertyValue();

public:
	syncUpdateType type;
	union            
	{
		bool bValue;
		int iValue;
		double dValue;
		CPropertyList *propertyList;
	};
	MP_WSTRING wcValue;	// если вернуть обратно в union то не компилится
	unsigned short wcValueLength;
	//bool bwcMem;
	//unsigned short propCount;
	//CPropertyValue &operator=(const CPropertyValue &aPropertyValue);
	void operator=(const CPropertyValue &aPropertyValue);
};
