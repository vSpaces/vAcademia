
#pragma once

#define CLAMP_255(x) if (x < 0) x = 0; if (x > 255) x = 255;

#define MAX_ST2_COUNT	12
#define MAX_PASS_COUNT	10
#define MAX_ROW_LENGTH	1920*2

class CWaveletsCommonRoutines
{
	public:
		CWaveletsCommonRoutines();
		~CWaveletsCommonRoutines();
		unsigned int lookup_st2[MAX_ST2_COUNT];
		bool lookup_fnd[(MAX_PASS_COUNT + 1)*MAX_ROW_LENGTH];
		//bool lookup_fnd[MAX_PASS_COUNT + 1][MAX_ROW_LENGTH];

		unsigned int g_waveletBufferSize;
		unsigned char* g_mask;
		unsigned char* g_tmp;

		void CalculateLookupsIfNeeded();
		void AllocateWaveletBuffersIfNeeded(unsigned int size);
	private:
		bool isCalculated;
};
