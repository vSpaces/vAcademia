// Recognize.h: interface for the CRecognize class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECOGNIZE_H__ADEA6171_1A96_484B_B3A5_E53A0FB1FEDB__INCLUDED_)
#define AFX_RECOGNIZE_H__ADEA6171_1A96_484B_B3A5_E53A0FB1FEDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define PI 3.14159265358979
#define SAMPLE_RATE 8000           /* 8000 KHz */
#define CHANNEL_NUMBER 1            /* Channel number of Sound Stream  */
#define TIME_START  0 
#define TIME_END    SAMPLE_RATE*50  /* Sample data: 16bit linear */
#define AMPLITUDE   16384           /* 2^14 */
#define FIR_TAPS    11              /* 0..22 (total of 23) */
#define Error       (void) fprintf
#define SOUND_FILE_NAME  "L16PCM_1KHz.au"
#define BASE_FREQUENCY   220 /* unit in Hz */

#define	AUDIO_FILE_MAGIC		((u_32)0x2e736e64) /* Sun magic ".snd" */
#define	AUDIO_FILE_ENCODING_LINEAR_16	(3)	/* 16-bit linear PCM */
#define BLOCK_SIZE 800*3

// Fast transform Furie
#define FFT 		-1
#define REV_FFT  	 1


typedef unsigned        u_32;

struct Value
{ DWORD value;
  bool sign;
};

#include <mmsystem.h>

class CRecognize  
{
public:
	void fft(float *x,float *y,int order,int param);
	float FIR(BYTE *aData, int Num);
	FILE *fp;
	HMMIO hFile;
	
	void write_header();
	int DoFilter(DWORD aDataSize, BYTE* aData, DWORD position, float limit);
	void FIR_coef(float Wc, int   NoT);
	void BPF(register float *x, register float *y, register int N);
	CRecognize();
	virtual ~CRecognize();

	float HL[FIR_TAPS+1] ,  HR[FIR_TAPS+1];
	float RXL[FIR_TAPS+1], RXR[FIR_TAPS+1];
	Value time[BLOCK_SIZE/2+1];
	int index_time;
};

#endif // !defined(AFX_RECOGNIZE_H__ADEA6171_1A96_484B_B3A5_E53A0FB1FEDB__INCLUDED_)
