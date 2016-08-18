// AudioCapture.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "AudioCapture.h"

#define XP_VAC4_10_CAPTURE_DEVICE_NAME		L"Virtual Cable 1"
#define VISTA7_VAC4_10_CAPTURE_DEVICE_NAME	L"Line 1 (Virtual Audio Cable)"

VOIPAUDIOCAPTURE_API const wchar_t*	GetVirtualAudioCableName()
{
	OSVERSIONINFOEXW ovi;
	memset(&ovi, 0, sizeof(ovi));
	ovi.dwOSVersionInfoSize=sizeof(ovi);
	GetVersionExW(reinterpret_cast<OSVERSIONINFOW *>(&ovi));

	if ((ovi.dwMajorVersion < 6) || (ovi.dwBuildNumber < 6001))
		return XP_VAC4_10_CAPTURE_DEVICE_NAME;
	
	return VISTA7_VAC4_10_CAPTURE_DEVICE_NAME;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}