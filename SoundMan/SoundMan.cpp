// SoundMan.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "SoundMan.h"
#include "SoundManager.h"

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

// This is an example of an exported variable
SOUNDMAN_API int nSoundMan=0;

// This is an example of an exported function.
SOUNDMAN_API int fnSoundMan(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see SoundMan.h for the class definition
CSoundMan::CSoundMan()
{ 
	return; 
}

namespace sm
{
	SOUNDMAN_API oms::omsresult CreateSoundManager( oms::omsContext* amcx)
	{	
		MP_NEW_P(pSoundManager, CSoundManager, amcx);
		amcx->mpSndM = pSoundManager;
		return OMS_OK;
	}

	SOUNDMAN_API void DestroySoundManager( oms::omsContext* amcx)
	{	
		if ( amcx)
		{
			CSoundManager*	pSoundManager = (CSoundManager*) amcx->mpSndM;
			MP_DELETE( pSoundManager);
		}
	}
}
