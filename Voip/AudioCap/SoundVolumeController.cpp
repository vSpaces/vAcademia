#include "StdAfx.h"
#include <MMSystem.h>
#include "SoundVolumeController.h"

CSoundVolumeController::CSoundVolumeController()
{
	ATLASSERT( _CrtCheckMemory( ));
	micVolumeCoefSaver = getMicVolume();
	micVolumeCoef = micVolumeCoefSaver;	
	recordVolumeCoefSaver = getRecordVolume();	
	recordVolumeCoef = recordVolumeCoefSaver;	
	headPhoneVolumeCoefSaver = getHeadPhoneVolume();	
	headPhoneVolumeCoef = headPhoneVolumeCoefSaver;	
	playVolumeCoefSaver = getPlayVolume();	
	playVolumeCoef = playVolumeCoefSaver;
	/*setMicVolume( micVolumeCoef);	
	setRecordVolume( recordVolumeCoef);	
	setPlayVolume( playVolumeCoef);	
	setHeadPhoneVolume( headPhoneVolumeCoef);*/
}

CSoundVolumeController::~CSoundVolumeController()
{
	restore();
}

//////////////////////////////////////////////////////////////////////////

BOOL CSoundVolumeController::setVolume( int type, DWORD inout, DWORD ComponentType, DWORD dwVol)
{
	HMIXER hMixer;
	HRESULT hr = S_FALSE;

	if ( type == MIC_VOLUME)
	{
		int iwaveInGetNumDevs = waveInGetNumDevs();
		if ( iwaveInGetNumDevs < 1)
			return FALSE;
		hr = mixerOpen( &hMixer, 0, 0, 0, MIXER_OBJECTF_WAVEIN);
	}
	else if ( type == PLAY_VOLUME)
	{
		int iwaveOutGetNumDevs = waveOutGetNumDevs();
		if ( iwaveOutGetNumDevs < 1)
			return FALSE;
		hr = mixerOpen( &hMixer, 0, 0, 0, MIXER_OBJECTF_WAVEOUT);
	}

	if ( FAILED( hr))
	{
		return FALSE;
	}

	MIXERLINE mxl;
	MIXERCONTROL mc;
	MIXERLINECONTROLS mxlc;
	MIXERCONTROLDETAILS mxcd;
	//MIXERCONTROLDETAILS_UNSIGNED *mxdu = new MIXERCONTROLDETAILS_UNSIGNED();
#define CHANNELS_COUNT 10
	MIXERCONTROLDETAILS_UNSIGNED mxdu[CHANNELS_COUNT];
	ZeroMemory( mxdu, sizeof(MIXERCONTROLDETAILS_UNSIGNED)*CHANNELS_COUNT);
	
	DWORD count, armxdu[]={0L, 0L};

	memset( &mxl, 0, sizeof( mxl));
	mxl.cbStruct = sizeof( mxl);
	mxl.dwComponentType = inout /*ComponentType*/;
	for( int i=0; i<CHANNELS_COUNT; i++)
		mxdu[i].dwValue = -1;	// default
	hr = mixerGetLineInfo(( HMIXEROBJ) hMixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE);
	if ( FAILED( hr) || mxl.cControls == 0)
	{
		//delete mxdu;
		mixerClose( hMixer);
		return FALSE;
	}

	count = mxl.cConnections/* dwSource*/;
	if( count == ( UINT)-1)
	{
		//delete mxdu;
		mixerClose( hMixer);
		return FALSE;
	}

	mc.cbStruct = sizeof( mc);
	mxlc.cbStruct = sizeof( mxlc);
	mxlc.dwLineID = mxl.dwLineID;
	mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
	mxlc.cControls = 1;
	mxlc.cbmxctrl = sizeof( MIXERCONTROL);
	mxlc.pamxctrl = &mc;

	hr = mixerGetLineControls(( HMIXEROBJ) hMixer, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE);
	if( hr == S_OK)
	{		
		// getting value
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
	else
	{
		for( UINT i = 0; i < count; i++)
		{
			mxl.dwSource = i;
			mixerGetLineInfo(( HMIXEROBJ) hMixer, &mxl, MIXER_GETLINEINFOF_SOURCE);
			if ( mxl.dwComponentType == ComponentType)
			{
				mc.cbStruct = sizeof( mc);
				mxlc.cbStruct = sizeof( mxlc);
				mxlc.dwLineID = mxl.dwLineID;
				mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
				mxlc.cControls = 1;
				mxlc.cbmxctrl = sizeof( MIXERCONTROL);
				mxlc.pamxctrl = &mc;
				hr = mixerGetLineControls(( HMIXEROBJ)hMixer, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE);

				if ( hr != S_OK)
					continue;
				// setting value
				for( UINT i = 0; i < ( mxl.cChannels); i++)
					armxdu[ i] = dwVol;
				mxdu->dwValue = dwVol;
				mxcd.cMultipleItems = 0;
				mxcd.cChannels = mxl.cChannels;
				mxcd.cbStruct = sizeof( mxcd);
				mxcd.dwControlID = mc.dwControlID;
				mxcd.cbDetails = sizeof( armxdu);
				mxcd.paDetails = &armxdu;
				hr = mixerSetControlDetails(( HMIXEROBJ) hMixer, &mxcd, MIXER_SETCONTROLDETAILSF_VALUE);				
			}
		}
	}
	//delete mxdu;
	//mxdu = NULL;
	mixerClose( hMixer);
	return TRUE;
}

BOOL CSoundVolumeController::getVolume( int type, DWORD inout, DWORD ComponentType, DWORD &dwVol)
{
	dwVol = -1;
	HMIXER hMixer;
	HRESULT hr = S_FALSE;
	ATLASSERT( _CrtCheckMemory( ));

/*	for (int id = 0; id < iwaveInGetNumDevs; id++) 
	{
		WAVEINCAPS caps;
		if ( waveInGetDevCaps(id, &caps, sizeof(caps)) == 0)
		{
			char *s = caps.szPname;
		}
	}*/

	if ( type == MIC_VOLUME)
	{
		int iwaveInGetNumDevs = waveInGetNumDevs();
		if ( iwaveInGetNumDevs < 1)
			return FALSE;
		hr = mixerOpen( &hMixer, 0, 0, 0, MIXER_OBJECTF_WAVEIN);
	}
	else if ( type == PLAY_VOLUME)
	{
		int iwaveOutGetNumDevs = waveOutGetNumDevs();
		if ( iwaveOutGetNumDevs < 1)
			return FALSE;
		hr = mixerOpen( &hMixer, 0, 0, 0, MIXER_OBJECTF_WAVEOUT);
	}
	ATLASSERT( _CrtCheckMemory( ));
	if ( FAILED( hr))
	{
		return FALSE;
	}

	MIXERLINE mxl;
	MIXERCONTROL mc;
	MIXERLINECONTROLS mxlc;
	MIXERCONTROLDETAILS mxcd;
	#define CHANNELS_COUNT 10
	MIXERCONTROLDETAILS_UNSIGNED mxdu[CHANNELS_COUNT];
	ZeroMemory( mxdu, sizeof(MIXERCONTROLDETAILS_UNSIGNED)*CHANNELS_COUNT);
	DWORD count, armxdu[]={0L, 0L};

	memset( &mxl, 0, sizeof( mxl));
	mxl.cbStruct = sizeof( mxl);
	mxl.dwComponentType = inout /*ComponentType*/;
	for( int i=0; i<CHANNELS_COUNT; i++)
		mxdu[i].dwValue = -1;	// default
	hr = mixerGetLineInfo(( HMIXEROBJ) hMixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE);
	ATLASSERT( _CrtCheckMemory( ));
	if ( FAILED( hr) || mxl.cControls == 0)
	{
		int error = ::GetLastError();
		//delete mxdu;
		mixerClose( hMixer);
		return FALSE;
	}

	count = mxl.cConnections/* dwSource*/;
	if( count == ( UINT)-1)
	{
		delete mxdu;
		mixerClose( hMixer);
		return FALSE;
	}

	mc.cbStruct = sizeof( mc);
	mxlc.cbStruct = sizeof( mxlc);
	mxlc.dwLineID = mxl.dwLineID;
	mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
	mxlc.cControls = 1;
	mxlc.cbmxctrl = sizeof( MIXERCONTROL);
	mxlc.pamxctrl = &mc;

	BOOL bSuccess = TRUE;
	hr = mixerGetLineControls(( HMIXEROBJ) hMixer, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE);
	ATLASSERT( _CrtCheckMemory( ));
	if( hr == S_OK)
	{		
		// getting value
		mxcd.cMultipleItems = 0;
		mxcd.cChannels = mxl.cChannels;
		mxcd.cbStruct = sizeof( mxcd);
		mxcd.dwControlID = mc.dwControlID;
		mxcd.cbDetails = sizeof( mxdu);
		mxcd.paDetails = &mxdu[0];
		hr = mixerGetControlDetails(( HMIXEROBJ) hMixer, &mxcd, MIXER_GETCONTROLDETAILSF_VALUE);
		ATLASSERT( _CrtCheckMemory( ));
		if( FAILED( hr))
		{
			bSuccess = FALSE;			
		}
	}
	else
	{		
		for( UINT i = 0; i < count; i++)
		{
			mxl.dwSource = i;

			hr = mixerGetLineInfo((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_SOURCE);
			ATLASSERT( _CrtCheckMemory( ));
			if (( !FAILED( hr)) && ( mxl.dwComponentType == ComponentType))
			{
				mc.cbStruct = sizeof( mc);
				mxlc.cbStruct = sizeof( mxlc);
				mxlc.dwLineID = mxl.dwLineID;
				mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
				mxlc.cControls = 1;
				mxlc.cbmxctrl = sizeof(MIXERCONTROL);
				mxlc.pamxctrl = &mc;

				hr = mixerGetLineControls(( HMIXEROBJ) hMixer, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE);
				ATLASSERT( _CrtCheckMemory( ));
				if( FAILED( hr) || hr != S_OK)
				{
					continue;
				}
				// getting value
				mxcd.cMultipleItems = 0;
				mxcd.cChannels = mxl.cChannels;
				mxcd.cbStruct = sizeof( mxcd);
				mxcd.dwControlID = mc.dwControlID;
				mxcd.cbDetails = sizeof( mxdu);
				mxcd.paDetails = &mxdu[0];
				hr = mixerGetControlDetails(( HMIXEROBJ) hMixer, &mxcd, MIXER_GETCONTROLDETAILSF_VALUE);	
				ATLASSERT( _CrtCheckMemory( ));
				if( FAILED( hr))
				{
					bSuccess = FALSE;
					break;	
				}
			}
		}
	}
	if ( bSuccess)
		dwVol = mxdu[0].dwValue;
	//mxdu->dwValue = -1;
	//delete mxdu;
	//mxdu = NULL;
	ATLASSERT( _CrtCheckMemory( ));
	mixerClose( hMixer);
	return bSuccess;
}

BOOL CSoundVolumeController::setMicVolume( float coef)
{	
	micVolumeCoef = coef;
	return setVolume( MIC_VOLUME, MIXERLINE_COMPONENTTYPE_DST_WAVEIN,
		MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE, coef * MAXVOLUME);
}

float CSoundVolumeController::getMicVolume()
{
	DWORD val = -1;
	ATLASSERT( _CrtCheckMemory( ));
	if ( !getVolume( MIC_VOLUME, MIXERLINE_COMPONENTTYPE_DST_WAVEIN,
		MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE, val))
		return -1.0f;
	ATLASSERT( _CrtCheckMemory( ));
	return val * 1.0f / ( MAXVOLUME * 1.0f);
}

BOOL CSoundVolumeController::setRecordVolume( float coef)
{	
	recordVolumeCoef = coef;
	return setVolume( MIC_VOLUME, MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE,
		MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE, coef * MAXVOLUME);
}

float CSoundVolumeController::getRecordVolume()
{
	DWORD val = -1;
	if ( !getVolume( MIC_VOLUME, MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE,
		MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE, val))
		return -1.0f;
	return val * 1.0f / ( MAXVOLUME * 1.0f);
}

BOOL CSoundVolumeController::setHeadPhoneVolume( float coef)
{	
	headPhoneVolumeCoef = coef;
	return setVolume( PLAY_VOLUME, MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT,
		MIXERLINE_COMPONENTTYPE_DST_SPEAKERS, coef * MAXVOLUME);	
}

float CSoundVolumeController::getHeadPhoneVolume()
{
	DWORD val = -1;
	if ( !getVolume( PLAY_VOLUME, MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT,
		MIXERLINE_COMPONENTTYPE_DST_SPEAKERS, val))
		return -1.0f;
	return val * 1.0f / ( MAXVOLUME * 1.0f);
}

BOOL CSoundVolumeController::setPlayVolume( float coef)
{	
	playVolumeCoef = coef;	
	return setVolume( PLAY_VOLUME, MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,
		MIXERLINE_COMPONENTTYPE_DST_SPEAKERS, coef * MAXVOLUME);
}

float CSoundVolumeController::getPlayVolume()
{
	DWORD val = -1;
	if ( !getVolume( PLAY_VOLUME, MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,
		MIXERLINE_COMPONENTTYPE_DST_SPEAKERS, val))
		return -1.0f;
	return val * 1.0f / ( MAXVOLUME * 1.0f);
}

void CSoundVolumeController::restore()
{
	if ( micVolumeCoefSaver != -1.0f && micVolumeCoefSaver != micVolumeCoef)
		setMicVolume( micVolumeCoefSaver);
	if ( recordVolumeCoefSaver != -1.0f && recordVolumeCoefSaver != recordVolumeCoef)
		setRecordVolume( recordVolumeCoefSaver);
	if ( playVolumeCoefSaver != -1.0f && playVolumeCoefSaver != playVolumeCoef)
		setPlayVolume( playVolumeCoefSaver);
	if ( headPhoneVolumeCoefSaver != -1.0f && headPhoneVolumeCoefSaver != headPhoneVolumeCoef)
		setHeadPhoneVolume( headPhoneVolumeCoefSaver);
}