// ComMan.cpp : Implementation of DLL Exports.

#include "stdafx.h"
#include "comman.h"
#include "ComManager.h"


/*extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)*/
BOOL APIENTRY DllMain( HANDLE hModule, 
	DWORD  ul_reason_for_call, 
	LPVOID lpReserved
	)
{
#ifdef _DEBUG
/*	_CrtSetDbgFlag( _CRTDBG_REPORT_FLAG | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_DELAY_FREE_MEM_DF
		| _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_DELAY_FREE_MEM_DF);*/
//_CrtSetBreakAlloc(464);
#endif
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}	
    return TRUE;    // ok
}

namespace cm
{
	COMMAN_API unsigned int CreateComManager( oms::omsContext* aContext)
	{
		//aContext->mpComMan = (cm::cmICommunicationManager*)(new CComManager( aContext));
		//CComManager *pComManager = new CComManager( aContext);
		MP_NEW_P(pComManager, CComManager, aContext);
		aContext->mpComMan = pComManager;
		aContext->mpResM = pComManager;
		aContext->mpIResM = pComManager;
		return OMS_OK;
	}

	COMMAN_API void DestroyComManager( oms::omsContext* aContext)
	{
		if (aContext == NULL || aContext->mpComMan == NULL)
			return;
		aContext->mpComMan->DestroyComman();
		aContext->mpComMan = NULL;
		aContext->mpResM = NULL;
		aContext->mpIResM = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE