//////////////////////////////////////////////////////////////////////
// Volume Controler
// by Whoo(whoo@isWhoo.com)
// Oct.1 2001

//////////////////////////////////////////////////////////////////////
// Mixer.cpp: implementation of the CMixer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Mixer.h"
#include "DSMixer.h"

#include <mmsystem.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

wchar_t* CMixer::predefinedExcludeNames[] = {L"virtual audio cable"
, L"virtual cable"};

wchar_t* CMixer::predefinedMixersNames[] = {L"stereo mix"
, L"stereo mixer"
, L"стерео микшер"
, L"микшер"
, L"mixer"
, L"mono out"
, L"mix"
, L"auxiliary"};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMixer::~CMixer()
{

}

#undef FAILED
#define FAILED(X) (MMSYSERR_NOERROR != X)

/////////////////////////////////////////////////
//  Component: value from dwComponentType member of MIXERLINE, eg://	MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE
//  Kind : 0 Wave ouput tunnle, 1 Wave input tunle, 2... other. 
CMixer::CMixer(DWORD ComponentType, DestKind dkKind): m_dwControlID(-1), m_bOK(false), m_dwChannels(0), MP_WSTRING_INIT(m_wsMicrophoneName)
{
	HMIXER hMixer;
	HRESULT hr;
	hr = mixerOpen(&hMixer, 0, 0, 0, 0);
	if (FAILED(hr)) return;

	MIXERLINEW mxl;
	MIXERCONTROLW mc;
	MIXERLINECONTROLSW mxlc;
	DWORD kind, count;

	if (dkKind == Play)
		kind = MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT;
	else 
		kind = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;

	mxl.cbStruct = sizeof(mxl);
	mxl.dwComponentType = kind;

	hr = mixerGetLineInfoW((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE);
	if (FAILED(hr))
	{
		mixerClose(hMixer);
		return;
	}

	count = mxl.dwSource;
	for(UINT i = 0; i < count; i++)
	{
		mxl.dwSource = i;
		mixerGetLineInfoW((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_SOURCE);
		if (mxl.dwComponentType == ComponentType)
		{
			m_dwChannels = mxl.cChannels;
			mc.cbStruct = sizeof(mc);
			mxlc.cbStruct = sizeof(mxlc);
			mxlc.dwLineID = mxl.dwLineID;
			mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
			mxlc.cControls = 1;
			mxlc.cbmxctrl = sizeof(MIXERCONTROLW);
			mxlc.pamxctrl = &mc;
			hr = mixerGetLineControlsW((HMIXEROBJ)hMixer, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE);
			m_dwControlID = mc.dwControlID;
			break;
		};
	}
	mixerClose(hMixer);
	m_bOK = true;
}

void	CMixer::LogAllMicrophoneNames()
{
	HMIXER hMixer;
	HRESULT hr;

	std::vector<std::wstring>	wsNames;

	CDSMixer::WriteLog("LogAllMicrophoneNames");

	unsigned int mixerID = 0xFFFFFFFF;
	int deviceCount = waveInGetNumDevs();
	for( int idevice=0; idevice<deviceCount; idevice++)
	{
		WAVEINCAPSW caps;
		::waveInGetDevCapsW(idevice, &caps, sizeof(caps));

		CDSMixer::WriteLog( " Device name:");
		CDSMixer::WriteLogNoNewLine("  "); CDSMixer::WriteLog( caps.szPname);
		MMRESULT err = mixerGetID((HMIXEROBJ)idevice, &mixerID, MIXER_OBJECTF_WAVEIN);
		CDSMixer::WriteLogNoNewLine("  "); CDSMixer::WriteLog( "Mixer id:");
		CDSMixer::WriteLog((double)mixerID);
		if( err != MMSYSERR_NOERROR)
			continue;
		if( mixerID == -1)
			continue;

		hr = mixerOpen(&hMixer, mixerID, 0, 0, 0);
		if (FAILED(hr))
		{
			CDSMixer::WriteLogNoNewLine("  "); CDSMixer::WriteLog( "Open mixer FAILED");
			continue;
		}

		DWORD kinds[] = {MIXERLINE_COMPONENTTYPE_DST_WAVEIN, MIXERLINE_COMPONENTTYPE_DST_VOICEIN, MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE};
		for( int idev=0; idev<sizeof(kinds)/sizeof(kinds[0]); idev++)
		{
			CDSMixer::WriteLogNoNewLine("   "); CDSMixer::WriteLog( "Component type");
			CDSMixer::WriteLogNoNewLine("    "); 
			switch(kinds[idev]) {
			case MIXERLINE_COMPONENTTYPE_DST_WAVEIN:
				CDSMixer::WriteLog( "MIXERLINE_COMPONENTTYPE_DST_WAVEIN");
				break;
			case MIXERLINE_COMPONENTTYPE_DST_VOICEIN:
				CDSMixer::WriteLog( "MIXERLINE_COMPONENTTYPE_DST_VOICEIN");
				break;
			case MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE:
				CDSMixer::WriteLog( "MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE");
				break;
			default:
				CDSMixer::WriteLog((double)idev);
			};
			MIXERLINEW mxl;
			MIXERCONTROLW mc[2];
			MIXERLINECONTROLSW mxlc;
			DWORD kind, count;

			kind = kinds[idev];

			mxl.cbStruct = sizeof(mxl);
			mxl.dwComponentType = kind;

			hr = mixerGetLineInfoW((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE);
			if (FAILED(hr))
			{
				CDSMixer::WriteLog( "   mixerGetLineInfoW FAILED");
				continue;
			}

			count = mxl.cConnections;
			CDSMixer::WriteLogNoNewLine("   "); CDSMixer::WriteLog("Connections count");
			CDSMixer::WriteLogNoNewLine("    "); CDSMixer::WriteLog((double)count);
			for(UINT i = 0; i < count; i++)
			{
				mxl.dwSource = i;
				mixerGetLineInfoW((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_SOURCE);

				CDSMixer::WriteLogNoNewLine("    "); CDSMixer::WriteLog("mxl.dwComponentType");
				CDSMixer::WriteLogNoNewLine("    "); CDSMixer::WriteLog((double)mxl.dwComponentType);
				mc[0].cbStruct = sizeof(mc);
				mc[1].cbStruct = sizeof(mc);
				mxlc.cbStruct = sizeof(mxlc);
				mxlc.dwLineID = mxl.dwLineID;
				mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
				mxlc.cControls = 1;
				mxlc.cbmxctrl = sizeof(MIXERCONTROLW);
				mxlc.pamxctrl = (LPMIXERCONTROLW)&mc[0];
				hr = mixerGetLineControlsW((HMIXEROBJ)hMixer, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE);

				CDSMixer::WriteLogNoNewLine("    "); CDSMixer::WriteLog("Component Name");
				CDSMixer::WriteLogNoNewLine("    "); 
				if( mxlc.pamxctrl->szShortName)
					CDSMixer::WriteLog(mxlc.pamxctrl->szShortName);
				else
					CDSMixer::WriteLog(NULL);
			}
		}
		mixerClose(hMixer);
	}
}

bool CMixer::IsExludeName( const wchar_t* aName)
{
	CWComString s = aName;
	s.MakeLower();
	for (UINT pn=0; pn<sizeof(predefinedExcludeNames)/sizeof(predefinedExcludeNames[0]); pn++)
	{
		if( s.Find( predefinedExcludeNames[pn]) > -1)
		{
			return true;
		}		
	}	
	return false;
};

bool CMixer::IsMixerName( const wchar_t* aName)
{
	CWComString s = aName;
	s.MakeLower();
	for (UINT pn=0; pn<sizeof(predefinedMixersNames)/sizeof(predefinedMixersNames[0]); pn++)
	{
		if( s.Find( predefinedMixersNames[pn]) > -1)
		{
			//CDSMixer::WriteLog( "Wow Mixer!!");
			// mixer found!!
			return true;
		}		
	}
	return false;
};

std::vector<int>	CMixer::GetMicrophoneIDs( const std::wstring& awsDeviceName)
{
	HMIXER hMixer;
	HRESULT hr;

	std::vector<int>	micIndexes;

	unsigned int mixerID = 0xFFFFFFFF;
	int deviceCount = waveInGetNumDevs();
	for( int i=0; i<deviceCount; i++)
	{
		WAVEINCAPSW caps;
		::waveInGetDevCapsW(i, &caps, sizeof(caps));

		CDSMixer::WriteLogNoNewLine(" "); CDSMixer::WriteLog( caps.szPname);
		if( wcscmp(caps.szPname, awsDeviceName.data()) == 0)
		{
			MMRESULT err = mixerGetID((HMIXEROBJ)i, &mixerID, MIXER_OBJECTF_WAVEIN);
			if( err != MMSYSERR_NOERROR)
				mixerID = -1;
			break;
		}
	}
	if( mixerID == -1)
	{
		return micIndexes;
	}

	hr = mixerOpen(&hMixer, mixerID, 0, 0, 0);
	if (FAILED(hr))
	{
		return micIndexes;
	}

	MIXERLINEW mxl;
//	MIXERCONTROLW mc[2];
//	MIXERLINECONTROLSW mxlc;
	DWORD kind, count;

	kind = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;

	mxl.cbStruct = sizeof(mxl);
	mxl.dwComponentType = kind;

	hr = mixerGetLineInfoW((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE);
	if (FAILED(hr))
	{
		mixerClose(hMixer);
		return micIndexes;
	}

	count = mxl.cConnections;
	for(UINT i = 0; i < count; i++)
	{
		mxl.dwSource = i;
		mixerGetLineInfoW((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_SOURCE);
		if (mxl.dwComponentType == MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE)
		{
			micIndexes.push_back( i);
		};
	}
	mixerClose(hMixer);

	return micIndexes;
}

std::vector<std::wstring>	CMixer::GetMicrophoneNames( const std::wstring& awsDeviceName)
{
	HMIXER hMixer;
	HRESULT hr;

	std::vector<std::wstring>	wsNames;

	CDSMixer::WriteLog("GetMicrophoneNames");

	unsigned int mixerID = 0xFFFFFFFF;
	int deviceCount = waveInGetNumDevs();
	for( int i=0; i<deviceCount; i++)
	{
		WAVEINCAPSW caps;
		::waveInGetDevCapsW(i, &caps, sizeof(caps));

		CDSMixer::WriteLogNoNewLine(" "); CDSMixer::WriteLog( caps.szPname);
			if( wcscmp(caps.szPname, awsDeviceName.data()) == 0)
			{
				MMRESULT err = mixerGetID((HMIXEROBJ)i, &mixerID, MIXER_OBJECTF_WAVEIN);
				if( err != MMSYSERR_NOERROR)
					mixerID = -1;
				break;
			}
	}

	CDSMixer::WriteLog("_ GetMicrophoneNames");
	CDSMixer::WriteLog((double)mixerID);

	if( mixerID == -1)
	{
		CDSMixer::WriteLogNoNewLine(" "); CDSMixer::WriteLog("0. GetMicrophoneNames");
		return wsNames;
	}

	hr = mixerOpen(&hMixer, mixerID, 0, 0, 0);
	if (FAILED(hr))
	{
		CDSMixer::WriteLogNoNewLine(" "); CDSMixer::WriteLog("1. GetMicrophoneNames");
		return wsNames;
	}

	MIXERLINEW mxl;
//	MIXERCONTROLW mc[2];
//	MIXERLINECONTROLSW mxlc;
	DWORD kind, count;

	kind = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;

	mxl.cbStruct = sizeof(mxl);
	mxl.dwComponentType = kind;

	hr = mixerGetLineInfoW((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE);
	if (FAILED(hr))
	{
		mixerClose(hMixer);
		return wsNames;
	}

	count = mxl.cConnections;
	CDSMixer::WriteLogNoNewLine(" "); CDSMixer::WriteLog("3. GetMicrophoneNames");
	CDSMixer::WriteLogNoNewLine("  "); CDSMixer::WriteLog((double)count);
	for(UINT i = 0; i < count; i++)
	{
		mxl.dwSource = i;
		mixerGetLineInfoW((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_SOURCE);
		if (mxl.dwComponentType == MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE)
		{
			/*mc[0].cbStruct = sizeof(mc);
			mc[1].cbStruct = sizeof(mc);
			mxlc.cbStruct = sizeof(mxlc);
			mxlc.dwLineID = mxl.dwLineID;
			mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
			mxlc.cControls = 1;
			mxlc.cbmxctrl = sizeof(MIXERCONTROLW);
			mxlc.pamxctrl = (LPMIXERCONTROLW)&mc[0];
			hr = mixerGetLineControlsW((HMIXEROBJ)hMixer, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE);*/
			if( IsExludeName(mxl.szName))
				continue;
			std::wstring wsMicrophoneName = mxl.szName;
			wsNames.push_back( wsMicrophoneName);
			/*if( mxlc.pamxctrl && wcslen(mxlc.pamxctrl->szShortName) > 0)
			{
				wsMicrophoneName.append(mxlc.pamxctrl->szShortName, mxlc.pamxctrl->szShortName + wcslen(mxlc.pamxctrl->szShortName)*sizeof(wchar_t) );
				wsNames.push_back( wsMicrophoneName);
				wsMicrophoneName.clear();
				wsMicrophoneName.append(mxlc.pamxctrl->szName, mxlc.pamxctrl->szName + wcslen(mxlc.pamxctrl->szName)*sizeof(wchar_t) );
				wsNames.push_back( wsMicrophoneName);
			}*/
		};
	}
	mixerClose(hMixer);

	return wsNames;
}

std::vector<std::wstring>	CMixer::GetOutputLineNames( const std::wstring& awsDeviceName, int adwComponentType)
{
	HMIXER hMixer;
	HRESULT hr;

	std::vector<std::wstring>	wsNames;

	CDSMixer::WriteLog("GetOutputLineNames");

	unsigned int mixerID = 0xFFFFFFFF;
	int deviceCount = waveOutGetNumDevs();
	for( int i=0; i<deviceCount; i++)
	{
		WAVEOUTCAPSW caps;
		::waveOutGetDevCapsW(i, &caps, sizeof(caps));

		CDSMixer::WriteLogNoNewLine(" "); CDSMixer::WriteLog( caps.szPname);
		if( wcscmp(caps.szPname, awsDeviceName.data()) == 0)
		{
			MMRESULT err = mixerGetID((HMIXEROBJ)i, &mixerID, MIXER_OBJECTF_WAVEOUT);
			if( err != MMSYSERR_NOERROR)
				mixerID = -1;
			break;
		}
	}

	CDSMixer::WriteLog("_ GetOutputLineNames");
	CDSMixer::WriteLog((double)mixerID);

	if( mixerID == -1)
	{
		CDSMixer::WriteLogNoNewLine(" "); CDSMixer::WriteLog("0. GetOutputLineNames");
		return wsNames;
	}

	hr = mixerOpen(&hMixer, mixerID, 0, 0, 0);
	if (FAILED(hr))
	{
		CDSMixer::WriteLogNoNewLine(" "); CDSMixer::WriteLog("1. GetOutputLineNames");
		return wsNames;
	}

	MIXERLINEW mxl;
//	MIXERCONTROLW mc[2];
	MIXERLINECONTROLS mxlc;
	DWORD kind, count;

	kind = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;

	mxl.cbStruct = sizeof(mxl);
	mxl.dwComponentType = kind;

	hr = mixerGetLineInfoW((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE);
	if (FAILED(hr))
	{
		mixerClose(hMixer);
		return wsNames;
	}

	count = mxl.cConnections;
	CDSMixer::WriteLogNoNewLine(" "); CDSMixer::WriteLog("3. GetOutputLineNames");
	CDSMixer::WriteLogNoNewLine("  "); CDSMixer::WriteLog((double)count);
	for(UINT i = 0; i < count; i++)
	{
		mxl.dwSource = i;
		mixerGetLineInfoW((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_SOURCE);
		if( IsExludeName(mxl.szName))
			continue;
		if (adwComponentType == -1 || mxl.dwComponentType == adwComponentType) //MIXERLINE_COMPONENTTYPE_SRC_PCSPEAKER
		{
			MIXERCONTROL mc;
//			MIXERCONTROLDETAILS mxcd;
//			MIXERCONTROLDETAILS_UNSIGNED mxdu;

			mc.cbStruct = sizeof( mc);
			mxlc.cbStruct = sizeof( mxlc);
			mxlc.dwLineID = mxl.dwLineID;
			mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;//MIXERCONTROL_CONTROLTYPE_VOLUME;
			mxlc.cControls = 1;
			mxlc.cbmxctrl = sizeof( MIXERCONTROL);
			mxlc.pamxctrl = &mc;
			hr = mixerGetLineControls(( HMIXEROBJ)hMixer, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE);
			
			if( hr == S_OK)
			{				
				// добавляем в спсико те линии, который можно отключать/включать
				std::wstring wsOutputLineName = mxl.szName;// szShortName;			
				wsNames.push_back( wsOutputLineName);				
			}			
		}
	}
	mixerClose(hMixer);

	return wsNames;
}


std::vector<std::wstring>	CMixer::GetMixersNames( const std::wstring& awsDeviceName)
{
	HMIXER hMixer;
	HRESULT hr;

	std::vector<std::wstring>	wsNames;

	CDSMixer::WriteLog("GetMixersNames");

	unsigned int mixerID = 0xFFFFFFFF;
	int deviceCount = waveInGetNumDevs();
	for( int i=0; i<deviceCount; i++)
	{
		WAVEINCAPSW caps;
		::waveInGetDevCapsW(i, &caps, sizeof(caps));

		CDSMixer::WriteLogNoNewLine(" "); CDSMixer::WriteLog( caps.szPname);
		if( wcscmp(caps.szPname, awsDeviceName.data()) == 0)
		{
			MMRESULT err = mixerGetID((HMIXEROBJ)i, &mixerID, MIXER_OBJECTF_WAVEIN);
			if( err != MMSYSERR_NOERROR)
				mixerID = -1;
			break;
		}
	}

	CDSMixer::WriteLog("_GetMixersNames");
	CDSMixer::WriteLog((double)mixerID);

	if( mixerID == -1)
	{
		CDSMixer::WriteLogNoNewLine(" "); CDSMixer::WriteLog("0. GetMixersNames");
		return wsNames;
	}

	hr = mixerOpen(&hMixer, mixerID, 0, 0, 0);
	if (FAILED(hr))
	{
		CDSMixer::WriteLogNoNewLine(" "); CDSMixer::WriteLog("1. GetMixersNames");
		return wsNames;
	}

	MIXERLINEW mxl;
//	MIXERCONTROLW mc[2];
//	MIXERLINECONTROLSW mxlc;
	DWORD kind, count;

	kind = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;

	mxl.cbStruct = sizeof(mxl);
	mxl.dwComponentType = kind;

	hr = mixerGetLineInfoW((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE);
	if (FAILED(hr))
	{
		mixerClose(hMixer);
		return wsNames;
	}

	count = mxl.cConnections;
	CDSMixer::WriteLogNoNewLine(" "); CDSMixer::WriteLog("3. GetMixersNames");
	CDSMixer::WriteLogNoNewLine("  "); CDSMixer::WriteLog((double)count);
	for(UINT i = 0; i < count; i++)
	{
		mxl.dwSource = i;
		mixerGetLineInfoW((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_SOURCE);
		if( IsExludeName(mxl.szName))
			continue;
		//if (mxl.dwComponentType == MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE)
		{			
			std::wstring wsMixerName = mxl.szName;		
			if( IsMixerName( mxl.szName))
				wsNames.push_back( wsMixerName);	
		}
	}
	mixerClose(hMixer);

	return wsNames;
}

bool CMixer::SetOutputDeviceLine( const std::wstring& awsDeviceName,  const std::wstring& awsLineName)
{
	HMIXER hMixer;
	HRESULT hr;

	std::vector<std::wstring>	wsNames;

	CDSMixer::WriteLog("SetOutputDeviceLine");

	unsigned int mixerID = 0xFFFFFFFF;
	int deviceCount = waveOutGetNumDevs();
	for( int i=0; i<deviceCount; i++)
	{
		WAVEOUTCAPSW caps;
		::waveOutGetDevCapsW(i, &caps, sizeof(caps));

		CDSMixer::WriteLogNoNewLine(" "); CDSMixer::WriteLog( caps.szPname);
		if( wcscmp(caps.szPname, awsDeviceName.data()) == 0)
		{
			MMRESULT err = mixerGetID((HMIXEROBJ)i, &mixerID, MIXER_OBJECTF_WAVEOUT);
			if( err != MMSYSERR_NOERROR)
				mixerID = -1;
			break;
		}
	}

	CDSMixer::WriteLog("_ GetOutputLineNames");
	CDSMixer::WriteLog((double)mixerID);

	if( mixerID == -1)
	{
		CDSMixer::WriteLogNoNewLine(" "); CDSMixer::WriteLog("0. SetOutputDeviceLine");
		return false;
	}

	hr = mixerOpen(&hMixer, mixerID, 0, 0, 0);
	if (FAILED(hr))
	{
		CDSMixer::WriteLogNoNewLine(" "); CDSMixer::WriteLog("1. SetOutputDeviceLine");
		return false;
	}

	MIXERLINEW mxl;	
	DWORD kind, count;

	kind = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;

	mxl.cbStruct = sizeof(mxl);
	mxl.dwComponentType = kind;

	hr = mixerGetLineInfoW((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE);
	if (FAILED(hr))
	{
		mixerClose(hMixer);
		return false;
	}

	MIXERCONTROL mc;
	MIXERLINECONTROLS mxlc;
	MIXERCONTROLDETAILS mxcd;	
#define CHANNELS_COUNT 10
	MIXERCONTROLDETAILS_UNSIGNED mxdu[CHANNELS_COUNT];
	ZeroMemory( mxdu, sizeof(MIXERCONTROLDETAILS_UNSIGNED)*CHANNELS_COUNT);
	DWORD armxdu[]={0L, 0L};

	count = mxl.cConnections;
	CDSMixer::WriteLogNoNewLine(" "); CDSMixer::WriteLog("3. SetOutputDeviceLine");
	CDSMixer::WriteLogNoNewLine("  "); CDSMixer::WriteLog((double)count);

	BOOL bLineFinded = FALSE;
	for(UINT i = 0; i < count; i++)
	{
		mxl.dwSource = i;
		mixerGetLineInfoW((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_SOURCE);
		if ( wcscmp( mxl.szName, awsLineName.c_str()) == 0)
		{
			bLineFinded = TRUE;
			break;
		}
	}

	if ( !bLineFinded)
	{
		mixerClose(hMixer);
		return false;
	}


	for(UINT i = 0; i < count; i++)
	{
		mxl.dwSource = i;
		mixerGetLineInfoW((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_SOURCE);
		//if (adwComponentType == -1 || mxl.dwComponentType == adwComponentType) //MIXERLINE_COMPONENTTYPE_SRC_PCSPEAKER
		//{
		//	std::wstring wsOutputLineName = mxl.szName;// szShortName;
		//	wsNames.push_back( wsOutputLineName);			
		//};

		mc.cbStruct = sizeof( mc);
		mxlc.cbStruct = sizeof( mxlc);
		mxlc.dwLineID = mxl.dwLineID;
		mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;//MIXERCONTROL_CONTROLTYPE_VOLUME;
		mxlc.cControls = 1;
		mxlc.cbmxctrl = sizeof( MIXERCONTROL);
		mxlc.pamxctrl = &mc;
		hr = mixerGetLineControls(( HMIXEROBJ)hMixer, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE);
		if( hr == S_OK)
		{
			DWORD dwVol =  0xFFFF;
			if ( wcscmp( mxl.szName, awsLineName.c_str()) == 0)
			{
				dwVol = 0;
			}			
			// setting value
			for( UINT i = 0; i < ( mxl.cChannels); i++)
				armxdu[ i] = dwVol;
			for( int i=0; i<CHANNELS_COUNT; i++)
				mxdu[i].dwValue = dwVol;
			mxcd.cMultipleItems = 0;
			mxcd.cChannels = mxl.cChannels;
			mxcd.cbStruct = sizeof( mxcd);
			mxcd.dwControlID = mc.dwControlID;
			mxcd.cbDetails = sizeof( armxdu);
			mxcd.paDetails = &armxdu;
			hr = mixerSetControlDetails(( HMIXEROBJ) hMixer, &mxcd, MIXER_SETCONTROLDETAILSF_VALUE);
		}

	}
	mixerClose(hMixer);

	return true;

}

void CMixer::SetVolume(DWORD dwVol)
{
	if (!m_bOK) return;
	HMIXER hMixer;
	HRESULT hr;
	hr = mixerOpen(&hMixer, 0, 0, 0, 0);
	if (FAILED(hr)) return;

	MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_UNSIGNED mxdu;

	mxdu.dwValue = dwVol;

	mxcd.cMultipleItems = 0;
	mxcd.cChannels = m_dwChannels;
	mxcd.cbStruct = sizeof(mxcd);
	mxcd.dwControlID = m_dwControlID;
	mxcd.cbDetails = sizeof(mxdu);
	mxcd.paDetails = &mxdu;
	hr = mixerSetControlDetails((HMIXEROBJ)hMixer, &mxcd, MIXER_SETCONTROLDETAILSF_VALUE);	
	
	mixerClose(hMixer);
}

DWORD CMixer::GetVolume()
{
	if (!m_bOK) return 0;
	HMIXER hMixer;
	HRESULT hr;
	hr = mixerOpen(&hMixer, 0, 0, 0, 0);
	if (FAILED(hr)) return 0;

	MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_UNSIGNED mxdu;


	mxcd.cMultipleItems = 0;
	mxcd.cChannels = m_dwChannels;
	mxcd.cbStruct = sizeof(mxcd);
	mxcd.dwControlID = m_dwControlID;
	mxcd.cbDetails = sizeof(mxdu);
	mxcd.paDetails = &mxdu;
	hr = mixerGetControlDetails((HMIXEROBJ)hMixer, &mxcd, MIXER_GETCONTROLDETAILSF_VALUE);	
	
	mixerClose(hMixer);
	return mxdu.dwValue;
}
