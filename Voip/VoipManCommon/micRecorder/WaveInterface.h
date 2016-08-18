// WaveInterface.h: interface for the CWaveInterface class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CWaveInterface  
{
public:
	static CComString GetWaveInName(UINT nIndex);
	static UINT GetWaveInCount();
	static CComString GetWaveOutName(UINT nIndex);
	static UINT GetWaveOutCount();
};

