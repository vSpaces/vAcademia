// WaveDevice.h: interface for the CWaveDevice class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Wave.h"

class CWaveDevice  
{
public:
	CWaveDevice(const CWaveDevice& copy);
	UINT GetDevice() const;
	bool IsOutputFormat(const CWave& wave);
	bool IsInputFormat(const CWave& wave);
	CWaveDevice(UINT nDevice = WAVE_MAPPER);
	virtual ~CWaveDevice();

private:
	UINT m_nDevice;
};

