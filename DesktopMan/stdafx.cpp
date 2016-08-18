// stdafx.cpp : source file that includes just the standard includes
// desktopMan.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#if (_ATL_VER < 0x0700)
#include <atlimpl.cpp>
#endif //(_ATL_VER < 0x0700)

#include "include/DesktopMan.h"
#include "include/SharingMan.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
CAppModule _Module;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
#ifdef DESKTOP_DETECT_MEMORY_LEAKS
	_CrtSetDbgFlag(
		_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) |
		/*_CRTDBG_CHECK_ALWAYS_DF |*/
		_CRTDBG_DELAY_FREE_MEM_DF |
		_CRTDBG_LEAK_CHECK_DF);
#endif

	if( ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		HRESULT hRes = ::CoInitialize(NULL);
		AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);	// add flags to support other controls

		hRes = _Module.Init(NULL, (HINSTANCE)hModule);
		ATLASSERT(SUCCEEDED(hRes));
	}
	else	if( ul_reason_for_call == DLL_PROCESS_DETACH)
	{
		_Module.Term();
		::CoUninitialize();
	}
    return TRUE;
}

namespace desktop
{
	//! Desktop manager
	DESKTOPMAN_API omsresult CreateDesktopManager( oms::omsContext* aContext)
	{
		MP_NEW_P( pDesktopMan, CDesktopManager, aContext);
		aContext->mpDesktopMan = pDesktopMan;
		return OMS_OK;
	}

	DESKTOPMAN_API void DestroyDesktopManager( oms::omsContext* aContext)
	{
		if (aContext == NULL || aContext->mpDesktopMan == NULL)
			return;
		aContext->mpDesktopMan->Release();
		aContext->mpDesktopMan = NULL;
	}

	//! Sharing manager
	DESKTOPMAN_API omsresult CreateSharingManager( oms::omsContext* aContext)
	{
		MP_NEW_P( pSharingMan, CSharingManager, aContext);	
		aContext->mpSharingMan = pSharingMan;		
		return OMS_OK;
	}

	DESKTOPMAN_API void DestroySharingManager( oms::omsContext* aContext)
	{
		if (aContext == NULL || aContext->mpSharingMan == NULL)
			return;
		aContext->mpSharingMan->Release();
		aContext->mpSharingMan = NULL;
	}
}