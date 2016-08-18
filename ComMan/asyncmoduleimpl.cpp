// asyncmoduleimpl.cpp
//
////////////////////////////////////////////////////////////////////////0

#include "stdafx.h"
#include "comman.h"
#include "commanager.h"
#include "asyncmoduleimpl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAsyncModuleImpl::CAsyncModuleImpl( CComManager* apComManager)
{
	ATLASSERT( apComManager);
	mpComManager = apComManager;
	mpResIModuleNotify = NULL;
}

CAsyncModuleImpl::~CAsyncModuleImpl()
{
	
}

void CAsyncModuleImpl::SetResIModuleNotify( resIModuleNotify* apResIModuleNotify)
{
	mpResIModuleNotify = apResIModuleNotify;
}

void CAsyncModuleImpl::Destroy()
{
	MP_DELETE_THIS;
}

//DWORD CAsyncModuleImpl::Run()
//{
//	ATLASSERT( FALSE);
//	return 0;
//}

//////////////////////////////////////////////////////////////////////////
// CAsyncSetModuleImpl
//////////////////////////////////////////////////////////////////////////
CAsyncSetModuleImpl::CAsyncSetModuleImpl( CComManager* apComManager) : CAsyncModuleImpl( apComManager)
{
	iID = 0;
}

void CAsyncSetModuleImpl::SetModuleID( unsigned int aID, const wchar_t* apwcModuleID, const wchar_t* asScene)
{
	moduleID = apwcModuleID;
	sScene = asScene;
	iID = aID;
	Start();
}

DWORD CAsyncSetModuleImpl::Run()
{
	unsigned int errorCode = 0;
	int iRes = mpComManager->SetModuleID(moduleID.c_str(), false, NULL, &errorCode);	

	int triesCount = 3;
	while( (errorCode & RES_SERVER_ERROR_NOTCONNECT) == RES_SERVER_ERROR_NOTCONNECT && triesCount > 0)		
	{
		Sleep( 100);
		iRes = mpComManager->SetModuleID(moduleID.c_str(), false, NULL, &errorCode);
		triesCount--;
	}
	if(iRes > -1)
	{
		wchar_t* pwcEduMETA=L"\\META-INF\\manifest.xml";
		res::resIResource* pRes = NULL;
		DWORD dwOpenFlags = RES_MEM_CACHE;
		errorCode = mpComManager->OpenResource(pwcEduMETA, pRes, dwOpenFlags);

		triesCount = 3;
		while( errorCode == OMS_ERROR_RESOURCE_FOUND_TIMEOUT && triesCount > 0)
		{
			Sleep( 100);
			mpComManager->OpenResource(pwcEduMETA, pRes, dwOpenFlags);
			triesCount--;
		}
		if(pRes != NULL)
			pRes->Close();

		if( errorCode != OMS_OK)
		{
			mpComManager->OnLoadedModuleMeta(iID, 2);
		}
		else
		{
			mpComManager->LoadModuleMetaAndLoadScene( iID, (wchar_t*)moduleID.c_str(), (wchar_t*)sScene.c_str());
		}
	}
	else
	{
		mpComManager->OnLoadedModuleMeta(iID, 1);
	}
	return 0;
}


//////////////////////////////////////////////////////////////////////////
// CAsyncModuleOpenImpl
//////////////////////////////////////////////////////////////////////////
CAsyncModuleOpenImpl::CAsyncModuleOpenImpl( CComManager* apComManager) : CAsyncModuleImpl( apComManager)
{
}

void CAsyncModuleOpenImpl::OpenModule( const wchar_t* apwcModuleID, const wchar_t* apwcAlias, resIModuleNotify* apResIModuleNotify)
{
	moduleID = apwcModuleID;
	moduleAlias = apwcAlias;
	mpResIModuleNotify = apResIModuleNotify;
	if( mpResIModuleNotify != NULL)
		mpResIModuleNotify->SetAsyncModule( this);
	Start();
}

DWORD CAsyncModuleOpenImpl::Run()
{
	omsresult result = mpComManager->OpenModule( moduleID.c_str(), moduleAlias.c_str());
	if( mpResIModuleNotify != NULL)
	{
		if( result == OMS_OK)
			mpResIModuleNotify->OnModuleOpened( moduleID.c_str());
		else
			mpResIModuleNotify->OnModuleError( moduleID.c_str(), 0);
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// CAsyncModuleCreateImpl
//////////////////////////////////////////////////////////////////////////
CAsyncModuleCreationImpl::CAsyncModuleCreationImpl( CComManager* apComManager) : CAsyncModuleImpl( apComManager)
{
	remote = false;
}

void CAsyncModuleCreationImpl::CreateModule( const wchar_t* apwcModuleID, const wchar_t* apwcAlias, bool abRemote, resIModuleNotify* apResIModuleNotify)
{
	remote = abRemote;
	moduleID = apwcModuleID;
	moduleAlias = apwcAlias;
	mpResIModuleNotify = apResIModuleNotify;
	Start();
}

DWORD CAsyncModuleCreationImpl::Run()
{
	omsresult result = mpComManager->CreateModule( moduleID.c_str(), moduleAlias.c_str(), remote);
	if( mpResIModuleNotify != NULL)
	{
		if( result == OMS_OK)
			mpResIModuleNotify->OnModuleCreated( moduleID.c_str(), &remote);
		else
			mpResIModuleNotify->OnModuleError( moduleID.c_str(), 1);
	}
	return 0;
}