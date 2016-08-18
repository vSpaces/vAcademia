// RTSampleDevice.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
/*
#include "DevicesCtrl.h"
#include "CaptureVideo.h"
//#include ".\sampledevicedialog.h"
/*
CDevicesCtrl	controller;
//CSampleDeviceDialog	myDialog;
void CreateDeviceDialog( HWND hParent);

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}



void CALLBACK MyEntryPoint(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{
	controller.EnumerateDevices();

	while (true)
	{
		Sleep(10);
	}
}


/************************************************************************/
// IRTBaseInterface* GetRTInterface()
// {
// 	return (IRTBaseInterface*)&controller;
// }
/*
void CreateDeviceDialog( HWND hParent)
{
// 	RECT rect = {0,0,500,500};
// 	myDialog.Create( hParent, rect, NULL);
// 	myDialog.ShowWindow( SW_SHOW);
}
*/