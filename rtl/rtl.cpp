// rtl.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#ifdef RTL_STATIC_LIB

#else

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

#endif