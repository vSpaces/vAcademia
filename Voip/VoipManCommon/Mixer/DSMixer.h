#if !defined(DXMIXER_INCLUDED)
#define DXMIXER_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "strmif.h"
#include "uuids.h"
#include "atlcomcli.h"
#include <xstring>

class CDSMixer
{
public:
	CDSMixer(const std::wstring& awsDeviceName);
	virtual ~CDSMixer();

public:
	WORD	GetVolume();
	BOOL	SetVolume(WORD dwVol);
	BOOL	EnableMicrophone(BOOL abEnable);

private:
	CComQIPtr<IAMAudioInputMixer>	GetDeviceVolumeControl();
	CComQIPtr<IAMAudioInputMixer>	GetMicrophoneVolumeControl();

public:
	static void						WriteLog(const std::wstring& str);
	static void						WriteLog(const std::string& str);
	static void						WriteLog(double val);
	static void						WriteLogNoNewLine(const std::string& str);

private:
	MP_WSTRING	m_wsDeviceName;
};

#endif // !defined(DXMIXER_INCLUDED)