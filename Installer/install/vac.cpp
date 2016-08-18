// vac.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <mmsystem.h>

// Addition libs
#pragma comment(lib, "winmm.lib")

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

bool IsVACInstalled()
{
	unsigned nbDevices = ::mixerGetNumDevs();
	MIXERCAPSW mixercaps;	ZeroMemory(&mixercaps, sizeof(mixercaps));

	OSVERSIONINFOEXW ovi;
	memset(&ovi, 0, sizeof(ovi));
	ovi.dwOSVersionInfoSize=sizeof(ovi);
	GetVersionExW(reinterpret_cast<OSVERSIONINFOW *>(&ovi));

	//For all the mixer devices
	for (unsigned deviceId = 0; deviceId < nbDevices; deviceId++) {

		//Gets the capacities of the mixer device
		if (MMSYSERR_NOERROR == ::mixerGetDevCapsW(deviceId, &mixercaps, sizeof(MIXERCAPSW))) 
		{
			if ((ovi.dwMajorVersion >= 6) && (ovi.dwBuildNumber >= 6001))
			{
				wchar_t* alpwcName = L"Virtual Audio Cable";
				if( wcsstr(mixercaps.szPname, alpwcName) != NULL)
				{
					return true;
				}
			}

			//For all the destinations available through the mixer device
			for (unsigned i = 0; i < mixercaps.cDestinations; i++) {
				MIXERLINEW mixerline;
				mixerline.cbStruct = sizeof(MIXERLINEW);
				mixerline.dwDestination = i;

				//Handle identifying the opened mixer device
				HMIXER hMixer;

				//Opens the mixer in order to get the handle identifying the opened mixer device
				if (MMSYSERR_NOERROR == ::mixerOpen(&hMixer, deviceId, NULL, NULL, MIXER_OBJECTF_MIXER)) {

					//Gets all information about the mixer line
					if (MMSYSERR_NOERROR == ::mixerGetLineInfoW((HMIXEROBJ) hMixer, &mixerline, MIXER_GETLINEINFOF_DESTINATION)) {

						//Checks if the mixer line is of type targetType
						if (mixerline.Target.dwType == MIXERLINE_TARGETTYPE_WAVEOUT)
						{
							wchar_t* alpwcName = L"Virtual Cable";
							if( wcsstr(mixercaps.szPname, alpwcName) != NULL)
							{
								::mixerClose(hMixer);
								return true;
							}
						}
					}
					::mixerClose(hMixer);
				}
			}
		}
	}
	return false;
}