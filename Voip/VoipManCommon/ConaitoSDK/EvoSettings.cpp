#include "StdAfx.h"

#include "ConaitoSDK\EvoSettings.h"
#ifdef USE_CONAITO_SDK_LIB
//#include "EvoWrapper.h"
#endif

#define SERVER_TCP_PORT	10300
#define SERVER_UDP_PORT	10301

#define AUDIO_SUBSYSTEM_XP		1
#define AUDIO_SUBSYSTEM_VISTA	2

#ifdef USE_CONAITO_SDK_LIB
void ConvertEVOToWin32Encoding(wchar_t* apwcString)
{
	unsigned uLength = wcslen( apwcString) + 1;
	char lpWin32Name[EVO_STRLEN];
	for (unsigned int i=0; i<uLength; i++)
		lpWin32Name[i] = (char)apwcString[i];
	lpWin32Name[uLength] = 0;

	MultiByteToWideChar(CP_ACP, 0, lpWin32Name, uLength, apwcString, EVO_STRLEN);
}
#endif

/************************************************************************/
/*                                                                      */
/************************************************************************/
CEvoSettings::CEvoSettings(oms::omsContext* aContext)
{
	context = aContext;
}
CEvoSettings::~CEvoSettings()
{
}
unsigned int CEvoSettings::GetUIntSetting( LPCTSTR sName)
{
	ATLASSERT( context);
	ATLASSERT( context->mpApp);
	CComString val;
	if( !context->mpApp->GetSettingFromIni( "voice", sName, val.GetBufferSetLength(100), 100))
		return 0;
	return rtl_atoi( val.GetBuffer());
}
unsigned int CEvoSettings::GetGainLevel()
{
	if( !context || !context->mpApp)	return 1000;
	return GetUIntSetting( "gainLevel");
}
unsigned int CEvoSettings::GetActivationLevel()
{
	if( !context || !context->mpApp)	return 5;
	return GetUIntSetting( "activationLevel");
}
unsigned int CEvoSettings::GetRecorderFreq()
{
	if( !context || !context->mpApp)	return 16000;
	return GetUIntSetting( "recorderFreq");
}
unsigned int CEvoSettings::GetUseDS()
{
	if( !context || !context->mpApp)	return 1;
	return GetUIntSetting( "useDirectSound");
}
unsigned int CEvoSettings::GetVoiceQuality()
{
	if( !context || !context->mpApp)	return 3;
	return GetUIntSetting( "voiceQuality");
}
unsigned int CEvoSettings::GetLogMode()
{
	if( !context || !context->mpApp)	return 0;
	return GetUIntSetting( "logMode");
}

void	CEvoSettings::SetReplacedAudioDeviceName( const wchar_t* alpwcDeviceName)
{
	if( !context || !context->mpApp)
		return;
	context->mpApp->SetSetting( L"replacedOutDevice", alpwcDeviceName);
}

std::wstring CEvoSettings::GetReplacedAudioDeviceName()
{
	if( !context || !context->mpApp)	return L"";

	wchar_t pwcValue[1024] = L"";
	if( !context->mpApp->GetSetting( L"replacedOutDevice", pwcValue, sizeof(pwcValue)/sizeof(pwcValue[0])))
		return L"";
	return std::wstring( pwcValue);
}