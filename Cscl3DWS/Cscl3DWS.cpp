// Cscl3DWS.cpp : Implementation of DLL Exports.
	
#include "stdafx.h"
#include "resource.h"
#include "Cscl3DWS.h"
#include "WS3D.h"

//#include <initguid.h>

HINSTANCE ghInstance = NULL;

BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
	ghInstance = (HINSTANCE)hModule;
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

namespace ws3d
{
	WS3DMAN_API oms::omsContext *Create3DWSManager(HWND aParent, LPRECT pRect)
	{
		CWS3D *p3DWSManager = MP_NEW(CWS3D);
		p3DWSManager->SetDllInstance(ghInstance);
		p3DWSManager->initOMSContext();
		DWORD dwStyles=WS_CHILD | WS_VISIBLE; 
		return p3DWSManager->getOMSContext();
	}

	WS3DMAN_API void Destroy3DWSManager(oms::omsContext *pContex)
	{
		if(pContex->mp3DWS!=NULL)
		{
			CWS3D *p3DWSManager = (CWS3D *)pContex->mp3DWS;
			pContex->mp3DWS->Destroy();
			MP_DELETE( p3DWSManager);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Code is written by Vladimir (2011.11.24)
	WS3DMAN_API HWND ghwndLauncherProcess = NULL;
	WS3DMAN_API int launchProcessID = 0;	

	BOOL CALLBACK EnumLauncherProc(HWND hwnd, LPARAM lParam){
		DWORD dwWinProcID = 0;
		GetWindowThreadProcessId(hwnd, &dwWinProcID);
		if(dwWinProcID == (DWORD)lParam){
			TCHAR   className[MAX_PATH+1];
			if( GetClassName( hwnd, className, MAX_PATH) > 0)
			{
				if( _tcsicmp( className, "LAUNCHER") == 0)
				{
					ghwndLauncherProcess = hwnd;
					return FALSE; // нашли нужное окно. ћожно не продолжать поиск
				}
			}
		}
		return TRUE;
	}

	WS3DMAN_API BOOL SendSignal2Launcher( int processId, unsigned int msgCode, unsigned int message)
	{
		unsigned int msgCode_ = (msgCode << 16) | message;
		if ( ghwndLauncherProcess == NULL && processId > 0)
		{
			EnumWindows(EnumLauncherProc, (LPARAM)processId);
			if ( ghwndLauncherProcess != NULL)
				return PostMessage( ghwndLauncherProcess, WM_LAUNCHER_WM_MESSAGE, GetCurrentProcessId(), msgCode_);
		}
		else if ( ghwndLauncherProcess != NULL)
			return PostMessage( ghwndLauncherProcess, WM_LAUNCHER_WM_MESSAGE, GetCurrentProcessId(), msgCode_);

		return FALSE;
	}


	WS3DMAN_API BOOL SendData2Launcher( int processId, int msgCode, COPYDATASTRUCT* message)
	{

		/*if ( ghwndLauncherProcess == NULL && processId > 0)
		{
			EnumWindows(EnumLauncherProc, (LPARAM)processId);
			if ( ghwndLauncherProcess != NULL)
			{				
				return SendMessage( ghwndLauncherProcess, WM_COPYDATA, msgCode, (LPARAM)(LPVOID) message);
			}
		}
		else if ( ghwndLauncherProcess != NULL)
		{
			//MessageBoxA(NULL, "8", "2", MB_OK);
			return SendMessage( ghwndLauncherProcess, WM_COPYDATA, msgCode, (LPARAM)(LPVOID) message);
		}

		return FALSE;*/

		if ( ghwndLauncherProcess == NULL && processId > 0)
		{
			EnumWindows(EnumLauncherProc, (LPARAM)processId);
		}

		if (ghwndLauncherProcess!=NULL)
		{
			unsigned char* data = MP_NEW_ARR(unsigned char, message->cbData + 1);
			memcpy(data, message->lpData, message->cbData);
			COPYDATASTRUCT* newMessage = MP_NEW(COPYDATASTRUCT);
			newMessage->lpData = data;
			newMessage->cbData = message->cbData;
			newMessage->dwData = message->dwData;
			MP_NEW_P3(infoSend, ThreadData, ghwndLauncherProcess, msgCode, (LPARAM)(LPVOID)newMessage);
			CCopyDataPoster::GetInstance()->AddData(infoSend);

			return TRUE;
		}

		return FALSE;
	}
	// end Vladimir (2011.02.10)
	//////////////////////////////////////////////////////////////////////////
}


/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE


