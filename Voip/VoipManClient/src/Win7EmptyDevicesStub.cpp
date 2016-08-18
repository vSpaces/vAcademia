#include "StdAfx.h"
#include "..\include\Win7EmptyDevicesStub.h"

#if _MSC_VER >= 1400
#include "..\src\VistaAudioEngine\VistaAudioEngine.h"
#endif

// 0 - XP
// 1 - Vista
// 2 - 7ка
// 3 - выше
unsigned char GetAudioEngineVersion()
{
	OSVERSIONINFOEXW ovi;
	memset(&ovi, 0, sizeof(ovi));
	ovi.dwOSVersionInfoSize=sizeof(ovi);
	GetVersionExW(reinterpret_cast<OSVERSIONINFOW *>(&ovi));

	if ( ovi.dwMajorVersion < 6)
		return 0;

	if ( ovi.dwMajorVersion == 6)
	{
		if( ovi.dwMinorVersion == 0)
			return 1;
		else if( ovi.dwMinorVersion == 1)
			return 2;
	}
	return 3;
}


bool CWin7EmptyDevicesStub::MakeExceptOneDeviceEnabled( ILogWriter* apLW)
{
#if _MSC_VER >= 1400
	OSVERSIONINFOEXW ovi;
	memset(&ovi, 0, sizeof(ovi));
	ovi.dwOSVersionInfoSize=sizeof(ovi);
	GetVersionExW(reinterpret_cast<OSVERSIONINFOW *>(&ovi));

	CComString foo;
	foo.Format("[VOIP] OS version: %u, %u, %u\n", ovi.dwMajorVersion, ovi.dwMinorVersion, ovi.dwBuildNumber);
	apLW->WriteLPCSTR( foo.GetBuffer());

	unsigned char audioEngineVersion = GetAudioEngineVersion();

	if( apLW != NULL)
	{
		CComString asLogText;
		asLogText.Format( "[VOIP] Activating VOIP input audio devices. audioEngineVersion = %i\n", (int)audioEngineVersion);
		apLW->WriteLPCSTR( asLogText.GetBuffer());
	}

	if ( audioEngineVersion > 0)
	{
		CVistaAudioEngine vistaAudioEngine;
		return vistaAudioEngine.MakeExceptOneDeviceEnabled( apLW);
	}
#endif
	return true;
}