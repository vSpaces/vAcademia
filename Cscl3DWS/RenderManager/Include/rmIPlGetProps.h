
#if !defined(AFX_rmIPlGetProps_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
#define AFX_rmIPlGetProps_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "rmIPlugin.h"

struct IAnyDataContainer
{
	virtual	void* GetMediaPointer() = 0;
	virtual	void* GetData(const char* lpcTag) = 0;
	virtual	long GetData(const char* lpcTag, void* buffer, long count) = 0;
};

struct IPropertiesContainer
{
	virtual bool IsSet(const char*) = 0;
	virtual int GetPropType(const char*) = 0;
	virtual int GetIntProp(const char*) = 0;
	virtual double GetDoubleProp(const char*) = 0;
	virtual wchar_t* GetStringProp(const char*) = 0;
	virtual IAnyDataContainer* GetRefProp(const char*) = 0;
	virtual void SetProp(const char*, int) = 0;
	virtual void SetProp(const char*, double) = 0;
	virtual void SetProp(const char*, const char*) = 0;
	virtual void SetProp(const char*, const wchar_t*) = 0;
};

#endif // !defined(AFX_rmIPlGetProps_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)