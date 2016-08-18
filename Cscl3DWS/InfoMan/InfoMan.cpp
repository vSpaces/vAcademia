// InfoMan.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "InfoMan.h"
#include "InfoManager.h"

CAppModule _Module;


int Run()
{
	//CMessageLoop theLoop;
	//_Module.AddMessageLoop(&theLoop);

	//int nRet = theLoop.R
	int nRet = 0;
	return nRet;
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
//#ifdef _DEBUG
//	_CrtSetDbgFlag(
//		_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) |
//		/*_CRTDBG_CHECK_ALWAYS_DF |*/
//		_CRTDBG_DELAY_FREE_MEM_DF |
//		_CRTDBG_LEAK_CHECK_DF);
//	//_CrtSetBreakAlloc(2726186);
//#endif
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			HRESULT hRes = ::CoInitialize(NULL);
			ATLASSERT(SUCCEEDED(hRes));

			InitCommonControls();
			AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);

			hRes = _Module.Init(NULL, (HINSTANCE) hModule);
			ATLASSERT(SUCCEEDED(hRes));
			//CDialogTemplate::SetResourceInstance( (HINSTANCE)_Module.m_hInstResource);

			Run();
			break;
			
		}
	case DLL_THREAD_ATTACH:
		{
			break;
		}
	case DLL_THREAD_DETACH:
		{
			break;
		}
	case DLL_PROCESS_DETACH:
		{
			_Module.Term();
			::CoUninitialize();
			break;
		}
	}
    return TRUE;
}

namespace info
{

INFOMAN_API omsresult CreateInfoManager( oms::omsContext* aContext)
{
	MP_NEW_P( pInfoMan, CInfoManager, aContext);
	aContext->mpInfoMan = pInfoMan;
	return OMS_OK;
}

INFOMAN_API void DestroyInfoManager( oms::omsContext* aContext)
{
	if (aContext == NULL || aContext->mpInfoMan == NULL)
		return;
	aContext->mpInfoMan->Destroy();
	aContext->mpInfoMan = NULL;
}

}
