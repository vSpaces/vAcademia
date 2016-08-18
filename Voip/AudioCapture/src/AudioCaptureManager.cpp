#include "StdAfx.h"
#include "..\include\audiocapturemanager.h"
#include "..\include\DefaultAudioDeviceManager.h"
#include "..\include\AudioRepeater.h"

CAudioCaptureManager::CAudioCaptureManager(void)
{
	captureOnProgress = false;	
	m_pDefaultAudioDeviceManager = MP_NEW( CDefaultAudioDeviceManager);
	m_pAudioRepeater = MP_NEW( CAudioRepeater);
}

CAudioCaptureManager::~CAudioCaptureManager(void)
{
	StopCapture();
	MP_DELETE( m_pDefaultAudioDeviceManager);
	MP_DELETE( m_pAudioRepeater);	
}


VOIPAC_ERROR	CAudioCaptureManager::StartCapture()
{
	if( captureOnProgress)
		return ACERROR_ISBISY;

	VOIPAC_ERROR	errorCode;

	errorCode = m_pDefaultAudioDeviceManager->ReplaceDefaultAudioDevice( GetVirtualAudioCableName());
	if( errorCode == ACERROR_OUTDEVICENOTFOUND)
		return ACERROR_NOAUDIOCABLEINSTALLED;
	else	if( errorCode == ACERROR_DEVICECHANGEFAILED)
		return ACERROR_PRIMARYCHANGEFAILED;
	else	if( errorCode == ACERROR_DEFAULTALLREADY)
		return ACERROR_DEFAULTALLREADY;

	errorCode = m_pAudioRepeater->Start( GetVirtualAudioCableName(), m_pDefaultAudioDeviceManager->GetReplacedAudioDeviceName());
	if( errorCode != ACERROR_NOERROR)
		return ACERROR_REPEATERSTARTFAILED;

	captureOnProgress = true;

	return ACERROR_NOERROR;
}

VOIPAC_ERROR	CAudioCaptureManager::StopCapture()
{
	if( !captureOnProgress)
		return ACERROR_NOTINITIALIZED;

	captureOnProgress = false;

	VOIPAC_ERROR	errorCode;

	errorCode = m_pAudioRepeater->Stop();
	ATLASSERT( errorCode == ACERROR_NOERROR);

	errorCode = m_pDefaultAudioDeviceManager->RestoreReplacedDevice();
	if( errorCode != ACERROR_NOERROR && errorCode != ACERROR_DEFAULTALLREADY)
		return ACERROR_DEFAULTSRESTOREFAILED;

	return ACERROR_NOERROR;
}

const wchar_t* CAudioCaptureManager::GetReplacedAudioDeviceName()
{
	return m_pDefaultAudioDeviceManager->GetReplacedAudioDeviceName();
}

VOIPAC_ERROR	CAudioCaptureManager::RestoreReplacedDeviceAfterError( const wchar_t* alpwcDeviceName)
{
	return m_pDefaultAudioDeviceManager->RestoreReplacedDeviceAfterError( alpwcDeviceName);
}