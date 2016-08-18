//////////////////////////////////////////////////////////////////////
// Volume Controler
// by Whoo(whoo@isWhoo.com)
// Oct.1 2001

//////////////////////////////////////////////////////////////////////
// Mixer.h: interface for the CMixer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MIXER_H__6AC26CD7_97BC_4721_8248_423000A8B0E7__INCLUDED_)
#define AFX_MIXER_H__6AC26CD7_97BC_4721_8248_423000A8B0E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wtypes.h>
#include <xstring>
#include <vector>

class CMixer  
{
public:
	DWORD	GetVolume();
	void	SetVolume(DWORD dwVol);
	static std::vector<std::wstring>	GetMicrophoneNames( const std::wstring& awsDeviceName);
	static std::vector<std::wstring>	GetOutputLineNames( const std::wstring& awsDeviceName, int adwComponentType);
	static std::vector<int>				GetMicrophoneIDs( const std::wstring& awsDeviceName);
	static std::vector<std::wstring>	GetMixersNames( const std::wstring& awsDeviceName);
	static bool SetOutputDeviceLine( const std::wstring& awsDeviceName,  const std::wstring& awsLineName);
	static void	LogAllMicrophoneNames();
	static bool IsExludeName( const wchar_t* aName);
	static bool IsMixerName( const wchar_t* aName);
	// Destination Kind;
	enum DestKind
	{
		Record,
		Play
	};
	CMixer(DWORD ComponentType, DestKind dkKind);
	virtual ~CMixer();

private:
	DWORD m_dwChannels;
	DWORD m_dwControlID;
	bool m_bOK;
	MP_WSTRING	m_wsMicrophoneName;	
	static wchar_t* predefinedMixersNames[];
	static wchar_t* predefinedExcludeNames[];
};

#endif // !defined(AFX_MIXER_H__6AC26CD7_97BC_4721_8248_423000A8B0E7__INCLUDED_)
