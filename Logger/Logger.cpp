// PathRefiner.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "LogSender.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
#ifdef _DEBUG
	//_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
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

namespace lgs
{
	LOGGER_API unsigned int CreateLogger( oms::omsContext* aContext)
	{
		MP_NEW_P(logger, CLogSender, aContext);
		//aContext->mpLogger->SetClientSession(apClientSession);
		return OMS_OK;
	}

	LOGGER_API void DestroyLogger( oms::omsContext* aContext)
	{
		if (aContext == NULL || aContext->mpLogger == NULL)
			return;
		aContext->mpLogger->Destroy();
		aContext->mpLogger = NULL;
	}
}


