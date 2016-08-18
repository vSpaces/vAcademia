// asyncmoduleimpl.h
//
//////////////////////////////////////////////////////////////////////

#pragma once

//#include "ComMan.h"
#include "ViskoeThread.h"
#include "res.h"

using namespace res;

class CComManager;

class CAsyncModuleImpl : public ViskoeThread::CThreadImpl<CAsyncModuleImpl>, public IAsyncModuleImpl
{
public:
	CAsyncModuleImpl( CComManager* apComManager);
	virtual ~CAsyncModuleImpl();

public:
	void SetResIModuleNotify( resIModuleNotify* apResIModuleNotify);
	void Destroy();

protected:
	CComManager* mpComManager;
	std::wstring	moduleID;
	resIModuleNotify* mpResIModuleNotify;
};

// установка текущего модуля
class CAsyncSetModuleImpl : public CAsyncModuleImpl
{
public:
	CAsyncSetModuleImpl( CComManager* apComManager);

public:
	void SetModuleID( unsigned int aID, const wchar_t* apwcModuleID, const wchar_t* asScene);

public:
	virtual DWORD Run();

private:	
	std::wstring	sScene;
	unsigned int	iID;
};

// открытие модуля
class CAsyncModuleOpenImpl : public CAsyncModuleImpl
{
public:
	CAsyncModuleOpenImpl( CComManager* apComManager);

public:
	void OpenModule( const wchar_t* apwcModuleID, const wchar_t* apwcAlias, resIModuleNotify* apResIModuleNotify);

public:
	virtual DWORD Run();

private:
	std::wstring	moduleAlias;
};



// создание модуля
class CAsyncModuleCreationImpl : public CAsyncModuleImpl
{
public:
	CAsyncModuleCreationImpl( CComManager* apComManager);

public:
	void CreateModule( const wchar_t* apwcModuleID, const wchar_t* apwcAlias, bool abRemote, resIModuleNotify* apResIModuleNotify);

public:
	virtual DWORD Run();

private:
	bool			remote;
	std::wstring	moduleAlias;	
};
