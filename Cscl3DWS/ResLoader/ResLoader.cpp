// ResLoader.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "ResLoader.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
#ifdef _DEBUG
//_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
  /*_CrtSetDbgFlag( _CRTDBG_REPORT_FLAG | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_DELAY_FREE_MEM_DF
		| _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_DELAY_FREE_MEM_DF);*/
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


// This is an example of an exported variable
RESLOADER_API int nResLoader=0;

// This is an example of an exported function.
RESLOADER_API int fnResLoader(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see ResLoader.h for the class definition
CResLoader::CResLoader()
{ 
	return; 
}

