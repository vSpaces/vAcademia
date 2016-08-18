// CaveSupport.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "CaveSupport.h"
#include "MainWindow.h"

HWND XVR_hWnd = 0;
char* XVR_path = NULL;

// deals with initialization
void startApp()
{
	try
	{
		EnableCaveSupport();
		DoInitializationAndLoading( XVR_hWnd);
		//EnableCaveSupport();
	}
	catch (...)
	{
		MessageBox(0, "Exception","Exception", MB_OK);
	}
}

// deals with onFrame and idle functions
void doFrame()
{
	DoFrame();
}

void doEvent(int e, int x, int y)
{
	int ii = 0;
	DoEvent(e,x,y);
}


// cleans up on exit
void quitApp()
{
	DoQuit();	
}

#define XVR_CONST_HWND 0 // Handle to the XVR window 
#define XVR_CONST_PATH 1 // Handle to the XVR current path 
#define XVR_CONST_HOSTHWND 3 // Handle to the XVR container window 

typedef void *(*XVR_CallBack)(int); 
char* __XVR_INIT(void *XVR_pointer) 
{ 
	//MessageBox(0, "__XVR_INIT", "", MB_OK);
	XVR_CallBack XVRGet = (XVR_CallBack) XVR_pointer; 
	XVR_hWnd = (HWND) XVRGet( XVR_CONST_HWND ); 
	XVR_path = (char*) XVRGet( XVR_CONST_PATH ); 
	return NULL; 
}

BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
	SetResourceInstance((HINSTANCE)hModule);
	return TRUE;
}

