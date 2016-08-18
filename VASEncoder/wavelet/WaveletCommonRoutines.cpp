
#include "StdAfx.h"
#include "WaveletCommonRoutines.h"

CWaveletsCommonRoutines::CWaveletsCommonRoutines()
{
	g_waveletBufferSize = 0;
	g_mask = NULL;
	g_tmp = NULL;
	isCalculated = false;
}
CWaveletsCommonRoutines::~CWaveletsCommonRoutines()
{
	if (g_mask != NULL)
		MP_DELETE_ARR( g_mask);
	if (g_tmp != NULL)
		MP_DELETE_ARR( g_tmp);
}

void CWaveletsCommonRoutines::CalculateLookupsIfNeeded()
{
	if (isCalculated)
	{
		return;
	}

	isCalculated = true;

	int count = 0;
	int value = 1;
	for (int i = 0; i < MAX_ST2_COUNT; i++)
	{
		lookup_st2[i] = value;
		value *= 2;
	}

	for (int passID = 0; passID < MAX_PASS_COUNT; passID++)
	{
		count = (passID + 1) * MAX_ROW_LENGTH;
		for (int x = 0; x < MAX_ROW_LENGTH; x++)
		{
			bool fnd = false;

			for (int k = 0; k <= passID; k++)
			if (x % lookup_st2[k + 1] == lookup_st2[k])
			{
				fnd = true;
				break;
			}

		//	lookup_fnd[passID + 1][x] = fnd;
			lookup_fnd[count] = fnd;
			count++;
		}
	}
	for (int x = 0; x < MAX_ROW_LENGTH; x++)
	{
		lookup_fnd[x] = false;
		//lookup_fnd[0][x] = false;
	}

	/*for (int x = 0; x < MAX_ROW_LENGTH; x++)
	{
		bool isTrue = false;
		for (int i = 1; i <= MAX_PASS_COUNT; i++)
		if (lookup_fnd[i])
		{
			isTrue = true;
			break;
		}

		lookup_fnd[MAX_PASS_COUNT + 1][x] = !isTrue;
	}*/
}

void CWaveletsCommonRoutines::AllocateWaveletBuffersIfNeeded(unsigned int size)
{
	if (size > g_waveletBufferSize)
	{
		if (g_mask)
		{
			MP_DELETE_ARR( g_mask);
			g_mask = NULL;
		}

		if (g_tmp)
		{
			MP_DELETE_ARR( g_tmp);
			g_tmp = NULL;
		}

		g_tmp = MP_NEW_ARR( unsigned char, size);
		g_mask = MP_NEW_ARR( unsigned char, size);
	}

	g_waveletBufferSize = size;
}