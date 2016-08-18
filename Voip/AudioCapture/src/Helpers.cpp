#include "StdAfx.h"
#include "..\include\Helpers.h"

//-----------------------------------------------------------------------------
// Name: GetWaveFormatFromIndex()
// Desc: Returns 20 different wave formats based on nIndex
//-----------------------------------------------------------------------------
VOID FillWaveFormatAlignAndBPS( WAVEFORMATEX* pwfx )
{
	pwfx->nBlockAlign = pwfx->nChannels * ( pwfx->wBitsPerSample / 8 );
	pwfx->nAvgBytesPerSec = pwfx->nBlockAlign * pwfx->nSamplesPerSec;
}

VOID GetWaveFormatFromIndex( INT nIndex, WAVEFORMATEX* pwfx )
{
	INT iSampleRate = nIndex / 4;
	INT iType = nIndex % 4;

	switch( iSampleRate )
	{
	case 0: pwfx->nSamplesPerSec = 48000; break;
	case 1: pwfx->nSamplesPerSec = 44100; break;
	case 2: pwfx->nSamplesPerSec = 22050; break;
	case 3: pwfx->nSamplesPerSec = 11025; break;
	case 4: pwfx->nSamplesPerSec =  8000; break;
	}

	switch( iType )
	{
	case 0: pwfx->wBitsPerSample =  8; pwfx->nChannels = 1; break;
	case 1: pwfx->wBitsPerSample = 16; pwfx->nChannels = 1; break;
	case 2: pwfx->wBitsPerSample =  8; pwfx->nChannels = 2; break;
	case 3: pwfx->wBitsPerSample = 16; pwfx->nChannels = 2; break;
	}
	FillWaveFormatAlignAndBPS( pwfx);
}

bool IsAudioFormatEnumerated( unsigned int auSampleRate, unsigned int auChannelsCount,	unsigned int auBitsPerSample )
{
	WAVEFORMATEX  wfx;
	ZeroMemory( &wfx, sizeof(wfx));

	// Try 20 different standard formats to see if they are supported
	for( INT iIndex = 0; iIndex < 20; iIndex++ )
	{
		GetWaveFormatFromIndex( iIndex, &wfx );
		if( wfx.nChannels == auChannelsCount &&
			wfx.wBitsPerSample == auBitsPerSample &&
			wfx.nSamplesPerSec == auSampleRate)
		{
			return true;
		}
	}
	return false;
}

BOOL CALLBACK DirectSoundEnumerationRoutine(LPGUID guid, LPCWSTR desc, LPCWSTR driver, LPVOID user)
{
	DEVICE_ENUMERATION_PARAMS* deviceParams = (DEVICE_ENUMERATION_PARAMS*)user;
	if( deviceParams->inDeviceName.compare(desc) == 0)
	{
		deviceParams->outDeviceGuid = *guid;
		return FALSE;
	}
	return TRUE;
}


BOOL CALLBACK DirectSoundEnumerateAllRoutine(LPGUID guid, LPCWSTR desc, LPCWSTR driver, LPVOID user)
{
	VecOutDevicesParams* vecDevices = (VecOutDevicesParams*)user;
	if( desc != NULL)
	{
		DEVICE_ENUMERATION_PARAMS	params;
		params.inDeviceName = desc;
		if( guid)
			params.outDeviceGuid = *guid;
		else
			params.outDeviceGuid = GUID_NULL;
		vecDevices->push_back( params);
	}
	return TRUE;
}
