// VoipMan.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "include\ConaitoSDK\VoipManagerConaito.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
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

namespace voip
{
	VOIPMAN_API omsresult CreateVoipManagerClient( oms::omsContext* aContext)
	{
		MP_NEW_P( pVoipMan, CVoipManagerConaito, aContext);
		aContext->mpVoipMan = pVoipMan;
		//aContext->mpSoundCaptureMan = new CVoipManagerConaitoCapture( aContext);	
		return OMS_OK;
	}

	VOIPMAN_API void DestroyVoipManagerClient( oms::omsContext* aContext)
	{
		if ( aContext == NULL)
			return;
		if ( aContext->mpVoipMan != NULL)
		{
			aContext->mpVoipMan->Release();
			aContext->mpVoipMan = NULL;
		}
		/*if ( aContext->mpSoundCaptureMan != NULL)
		{
			aContext->mpSoundCaptureMan->Release();
			aContext->mpSoundCaptureMan = NULL;
		}*/
	}
}