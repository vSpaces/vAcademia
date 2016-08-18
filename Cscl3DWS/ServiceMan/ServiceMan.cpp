// ServiceMan.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ServiceMan.h"
#include "ServiceManager.h"
#include "ServiceWorldManager.h"

#define MAX_LOADSTRING 100

BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
#ifdef _DEBUG
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(2726186);
#endif
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

namespace service
{

	SERVICEMAN_API omsresult CreateServiceManager( oms::omsContext* aContext)
	{
		if(aContext->mpServiceMan==NULL)
		{
			MP_NEW_P( mpServiceMan, CServiceManager, aContext);
		}
		return OMS_OK;
	}

	SERVICEMAN_API void DestroyServiceManager( oms::omsContext* aContext)
	{
		if (aContext == NULL || aContext->mpServiceMan == NULL)
			return;
		aContext->mpServiceMan->Destroy();
		aContext->mpServiceMan = NULL;
	}

	SERVICEMAN_API omsresult CreateServiceWorldManager( oms::omsContext* aContext)
	{
		if(aContext->mpServiceWorldMan==NULL)
		{
			MP_NEW_P( mpServiceWorldMan, CServiceWorldManager, aContext);
		}
		return OMS_OK;
	}

	SERVICEMAN_API void DestroyServiceWorldManager( oms::omsContext* aContext)
	{
		if (aContext == NULL || aContext->mpServiceWorldMan == NULL)
			return;
		aContext->mpServiceWorldMan->Destroy();
		aContext->mpServiceWorldMan = NULL;
	}
}
