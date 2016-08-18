#pragma once

#ifdef USE_CONAITO_SDK_LIB
#include "EvoWrapper.h"
#endif

#ifdef USE_MUMBLE_SDK
#include "MumbleAPI.h"
using namespace api;
#endif

#ifdef USE_CONAITO_SDK_LIB
extern void ConvertEVOToWin32Encoding(wchar_t* apwcString);
#endif

#define GET_RANGED_VAL(i,a,x)	(i+(a-i)*x)

class CEvoSettings
{
public:
	CEvoSettings(oms::omsContext* aContext);
	~CEvoSettings();

public:
	unsigned int GetGainLevel();
	unsigned int GetActivationLevel();
	unsigned int GetRecorderFreq();
	unsigned int GetUseDS();
	unsigned int GetVoiceQuality();
	unsigned int GetLogMode();
	std::wstring GetReplacedAudioDeviceName( );
	void SetReplacedAudioDeviceName( const wchar_t* alpwcDeviceName);

protected:
	unsigned int GetUIntSetting( LPCTSTR sName);
	oms::omsContext* context;
};

typedef struct _ConnectRequestParams
{
	CComString		voipServerAddress;
	unsigned int	voipPort;
	CComString		recordServerAddress;
	unsigned int	recordPort;
	CComString		roomName;
	//////////////////////////////////////////////////////////////////////////
} ConnectRequestParams;

enum	CONNECTION_STATUS	{CS_DISCONNECTED, CS_DISCONNECTING, CS_CONNECTING, CS_CONNECTED, CS_CONNECTFAILED, CS_LOSTCONNECTION};