
#if !defined(AFX_CLEF_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
#define AFX_CLEF_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "rmIPlGetProps.h"

class CAnyDataContainerImpl : public clef::ianydatacontainer
{
public:
	CAnyDataContainerImpl(){}
	~CAnyDataContainerImpl(){}

public:
	virtual	void*	get_data(const char* lpcTag)
					{	return NULL;}
	virtual	long	get_data(const char* lpcTag, void* buffer, long count)
					{	return 0;}
};

#endif // !defined(AFX_CLEF_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)