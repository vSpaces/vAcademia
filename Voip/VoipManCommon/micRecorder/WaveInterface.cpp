// WaveInterface.cpp: implementation of the CWaveInterface class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WaveInterface.h"

#include <mmsystem.h>
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
UINT CWaveInterface::GetWaveInCount()
{
	return waveInGetNumDevs();	
}

//////////////////////////////////////////////////////////////////////
CComString CWaveInterface::GetWaveInName(UINT nIndex)
{
	assert(nIndex < GetWaveInCount());
	WAVEINCAPS tagCaps;
	switch (waveInGetDevCaps(nIndex, &tagCaps, sizeof(tagCaps))) {
	case MMSYSERR_NOERROR:
		return tagCaps.szPname;
		break;
	default:
		return "";
	}
}

//////////////////////////////////////////////////////////////////////
UINT CWaveInterface::GetWaveOutCount()
{
	return waveOutGetNumDevs();	
}

//////////////////////////////////////////////////////////////////////
CComString CWaveInterface::GetWaveOutName(UINT nIndex)
{
	assert(nIndex < GetWaveOutCount());
	WAVEOUTCAPS tagCaps;
	switch (waveOutGetDevCaps(nIndex, &tagCaps, sizeof(tagCaps))) {
	case MMSYSERR_NOERROR:
		return tagCaps.szPname;
		break;
	default:
		return "";
	}
}
