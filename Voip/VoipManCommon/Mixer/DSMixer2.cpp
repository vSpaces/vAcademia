#include "StdAfx.h"
#include ".\DSMixer2.h"
#include ".\Mixer.h"
#include <mmsystem.h>
#include <dsound.h>
#include "MumbleAPI.h"

#include "strmif.h"
#include "uuids.h"
#include "atlcomcli.h"
#include <vector>
#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// asDeviceName - имя устройства вывода. For example:
// asMicrophoneName
CDSMixer2::CDSMixer2()
{
}

CDSMixer2::~CDSMixer2()
{
}

BOOL CALLBACK DirectSoundCaptureEnumerationRoutine(LPGUID guid, LPCWSTR desc, LPCWSTR driver, LPVOID user)
{
	CDSMixer2::VecDeviceLines* vecNames = (CDSMixer2::VecDeviceLines*)user;
	if( desc && vecNames)
	{
		//vecNames->push_back( desc);

		wchar_t wsGUID[ MUMBLE_STRLEN];
		wsGUID[ StringFromGUID2( *guid, &wsGUID[0], MUMBLE_STRLEN)] = 0;
		//vecNames->push_back( (LPCWSTR)wsGUID);
		CWComString s1 = wsGUID;
		s1.MakeLower();
		vecNames->push_back( s1.GetBuffer());
	}
	return TRUE;
}

/************************************************************************/
/* Input devices
/************************************************************************/
CDSMixer2::VecDeviceLines	CDSMixer2::GetInDeviceLines(const std::wstring& awsDeviceName) const
{
	return GetDeviceLinesImpl(awsDeviceName, TRUE);
}

CDSMixer2::DeviceLineName	CDSMixer2::GetInDeviceCurrentLineName(const std::wstring& awsDeviceName) const
{
	std::vector<int>	indexes = CMixer::GetMicrophoneIDs( awsDeviceName);
	CDSMixer2::VecDeviceLines allDeviceLines = GetInDeviceLines(awsDeviceName);
	for(unsigned int i=0; i<allDeviceLines.size(); i++)
	{
		if( std::find(indexes.begin(), indexes.end(), i) == indexes.end())
			continue;

		CComQIPtr<IAMAudioInputMixer>	pLineControl = GetInLineVolumeControl(awsDeviceName, allDeviceLines[i]);
		if( pLineControl != NULL)
		{
			BOOL abEnabled = FALSE;
			if( SUCCEEDED(pLineControl->get_Enable( &abEnabled)))
			{
				if( abEnabled)
					return allDeviceLines[i];
			}
		}
	}

	// No mic found. Return first disabled mic line
	if( indexes.size()>0 && (int)allDeviceLines.size()>indexes[0])
		return allDeviceLines[ indexes[0]];
	return L"";
}

CDSMixer2::VecDeviceLines	CDSMixer2::GetInDevices() const
{
	CDSMixer2::VecDeviceLines devicesNames;
	if( SUCCEEDED( DirectSoundCaptureEnumerateW( &DirectSoundCaptureEnumerationRoutine, &devicesNames)))
		return devicesNames;

	return GetDevicesImpl( TRUE);
}

CDSMixer2::VecDeviceLines	CDSMixer2::GetOutDevices() const
{
	CDSMixer2::VecDeviceLines devicesNames;
	if( SUCCEEDED( DirectSoundEnumerateW( &DirectSoundCaptureEnumerationRoutine, &devicesNames)))
		return devicesNames;

	return GetDevicesImpl( FALSE);
}

WORD	CDSMixer2::GetInLineVolume(const std::wstring& awsDeviceName, const std::wstring& awsLineName) const
{
	double volumeLevel = 0;
	CComQIPtr<IAMAudioInputMixer>	deviceControl = GetInLineVolumeControl( awsDeviceName, awsLineName);
	if( deviceControl)
	{
		if( SUCCEEDED( deviceControl->get_MixLevel(&volumeLevel)))
		{
			return (WORD)(0xFFFF * volumeLevel);
		}
	}
	deviceControl = GetInDeviceVolumeControl(awsDeviceName);
	if( deviceControl)
	{
		if( SUCCEEDED( deviceControl->get_MixLevel(&volumeLevel)))
		{
			return (WORD)(0xFFFF * volumeLevel);
		}
	}
	return 0xFFFF;
}

BOOL	CDSMixer2::SetInLineVolume(const std::wstring& awsDeviceName, const std::wstring& awsLineName, WORD dwVol)
{
	double volumeLevel = (double)dwVol / 0xFFFF;
	CComQIPtr<IAMAudioInputMixer>	deviceControl = GetInLineVolumeControl( awsDeviceName, awsLineName);
	if( deviceControl)
	{
		if( SUCCEEDED( deviceControl->put_MixLevel(volumeLevel)))
		{
			return TRUE;
		}
	}
	deviceControl = GetInDeviceVolumeControl(awsDeviceName);
	if( deviceControl)
	{
		if( SUCCEEDED( deviceControl->put_MixLevel(volumeLevel)))
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL	CDSMixer2::SetEnabledInDeviceLine(const std::wstring& awsDeviceName, const std::wstring& awsLineName, BOOL abEnable)
{
	BOOL success = FALSE;
	CComQIPtr<IAMAudioInputMixer>	deviceControl = GetInLineVolumeControl( awsDeviceName, awsLineName);
	if( deviceControl)
		if( SUCCEEDED( deviceControl->put_Enable(abEnable)))
			success = TRUE;

	if( abEnable)
	{
		deviceControl = GetInDeviceVolumeControl(awsDeviceName);
		if( deviceControl)
			if( SUCCEEDED( deviceControl->put_Enable(abEnable)))
				success = TRUE;
	}

	return success;
}

BOOL	CDSMixer2::EnableOneInDeviceLine(const std::wstring& awsDeviceName, const std::wstring& awsLineName)
{
	BOOL found = FALSE;
	CDSMixer2::VecDeviceLines allDeviceLines = GetInDeviceLines(awsDeviceName);
	for(unsigned int i=0; i<allDeviceLines.size(); i++)
	{
		if( awsLineName == allDeviceLines[i])
		{
			found = TRUE;
			SetEnabledInDeviceLine( awsDeviceName, allDeviceLines[i], true);
		}
		else
		{
			SetEnabledInDeviceLine( awsDeviceName, allDeviceLines[i], false);
		}
	}
	return found;
}

/************************************************************************/
/* Output devices
/************************************************************************/
CDSMixer2::VecDeviceLines	CDSMixer2::GetOutDeviceLines(const std::wstring& awsDeviceName) const
{
	return GetDeviceLinesImpl(awsDeviceName, FALSE);
}

CDSMixer2::DeviceLineName	CDSMixer2::GetOutDeviceCurrentLineName(const std::wstring& awsDeviceName) const
{
	std::vector<int>	indexes;
	CDSMixer2::VecDeviceLines allDeviceLines = GetOutDeviceLines(awsDeviceName);
	for(unsigned int i=0; i<allDeviceLines.size(); i++)
	{
		if( std::find(indexes.begin(), indexes.end(), i) == indexes.end())
			continue;

		CComQIPtr<IAMAudioInputMixer>	pLineControl = GetLineVolumeControlImpl(awsDeviceName, allDeviceLines[i], FALSE);
		if( pLineControl != NULL)
		{
			BOOL abEnabled = FALSE;
			if( SUCCEEDED(pLineControl->get_Enable( &abEnabled)))
			{
				if( abEnabled)
					return allDeviceLines[i];
			}
		}
	}

	// No mic found. Return first disabled microphone line
	if( indexes.size()>0 && (int)allDeviceLines.size()>indexes[0])
		return allDeviceLines[ indexes[0]];
	return L"";
}

WORD	CDSMixer2::GetOutLineVolume(const std::wstring& awsDeviceName, const std::wstring& awsLineName) const
{
	double volumeLevel = 0;
	CComQIPtr<IAMAudioInputMixer>	deviceControl = GetLineVolumeControlImpl( awsDeviceName, awsLineName, FALSE);
	if( deviceControl)
	{
		if( SUCCEEDED( deviceControl->get_MixLevel(&volumeLevel)))
		{
			return (WORD)(0xFFFF * volumeLevel);
		}
	}
	deviceControl = GetDeviceVolumeControlImpl(awsDeviceName, FALSE);
	if( deviceControl)
	{
		if( SUCCEEDED( deviceControl->get_MixLevel(&volumeLevel)))
		{
			return (WORD)(0xFFFF * volumeLevel);
		}
	}
	return 0xFFFF;
}

BOOL	CDSMixer2::SetOutLineVolume(const std::wstring& awsDeviceName, const std::wstring& awsLineName, WORD dwVol)
{
	double volumeLevel = (double)dwVol / 0xFFFF;
	CComQIPtr<IAMAudioInputMixer>	deviceControl = GetLineVolumeControlImpl( awsDeviceName, awsLineName, FALSE);
	if( deviceControl)
	{
		if( SUCCEEDED( deviceControl->put_MixLevel(volumeLevel)))
		{
			return TRUE;
		}
	}
	deviceControl = GetDeviceVolumeControlImpl(awsDeviceName, FALSE);
	if( deviceControl)
	{
		if( SUCCEEDED( deviceControl->put_MixLevel(volumeLevel)))
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL	CDSMixer2::SetCurrentOutDeviceLine(const std::wstring& awsDeviceName, const std::wstring& awsLineName, BOOL abEnable)
{
	BOOL success = FALSE;
	CComQIPtr<IAMAudioInputMixer>	deviceControl = GetLineVolumeControlImpl( awsDeviceName, awsLineName, FALSE);
	if( deviceControl)
		if( SUCCEEDED( deviceControl->put_Enable(abEnable)))
			success = TRUE;

	deviceControl = GetDeviceVolumeControlImpl(awsDeviceName, FALSE);
	if( deviceControl)
		if( SUCCEEDED( deviceControl->put_Enable(abEnable)))
			success = TRUE;

	return success;
}