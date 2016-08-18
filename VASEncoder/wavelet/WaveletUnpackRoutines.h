
#pragma once

#include "WaveletCommonRoutines.h"

class CWaveletUnpackRoutines
{
public:
	CWaveletUnpackRoutines();
	~CWaveletUnpackRoutines();
	__forceinline void YUVtoRGB(unsigned char y, unsigned char u, unsigned char v, unsigned char* rgb)
	{
		int _r, _g, _b;
// 		_r = y + (int)((v - 128) * 14075 / 10000);
// 		_g = y - (int)(((v - 128) * 7169 + (u - 128) * 3455)  / 10000);
// 		_b = y + (int)((u - 128) * 17790  / 10000);

		_r = y + ((5765 * v) >> 12) - 179;
		_g = y - ((1415 * u + 2936 * v) >> 12) + 136;
		_b = y + ((7286 * u) >> 12) - 227;

		CLAMP_255(_r);
		CLAMP_255(_g);
		CLAMP_255(_b);

		rgb[0] = _r;
		rgb[1] = _g;
		rgb[2] = _b;
	}

	void DecodeWaveletSequence(unsigned char passCount,
							unsigned char* data, int width, int height, int quantizeShift, bool abEasyPacked);
	CWaveletsCommonRoutines wlCommonRoutines;
};