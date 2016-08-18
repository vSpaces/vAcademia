// micRecorder.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "micRecorder.h"
#include "micrec_consts.h"
#include "..\soundLog\MP3Encoder.h"
#include "Wave.h"
#include "WaveIn.h"
#include <tchar.h>

CWaveIn waveIn;
CWave monWave;

int Start_Recording()
{	
	WAVEFORMATEX tagFormat;
	tagFormat.nChannels = 2;
	tagFormat.nSamplesPerSec = 22050;
	tagFormat.wBitsPerSample = 16;	
	waveIn.SetWaveFormat( tagFormat);

	CWComString tmpFileFullName = L"";
	CWComString tmpFileName = L"";
	
	if ( !waveIn.Open())
	{
		CComString s;
		s.Format( "ERROR|%s", waveIn.GetError().GetBuffer());		
		return OPEN_DEVICE_ERROR;
	}

	if ( !CMP3Encoder::getFileFullNameFromUuid( tmpFileFullName, tmpFileName, L"", L"_rec"))
	{
		tmpFileName = L"mic_rec.mp3";
	}
	else
		tmpFileName += L".mp3";

	if ( !waveIn.Record2File( tmpFileName.GetBuffer(), 4080)) 
	{
		CComString s;
		s.Format( "ERROR|%s", waveIn.GetError().GetBuffer());		
		return RECORD_ERROR;
	}

	return NO_ERROR;
}

int Stop_Recording( LPWSTR *fileName, unsigned int aSize)
{
	wcscpy_s( *fileName, aSize, waveIn.getFileName());
	if ( !waveIn.Close())
		return STOP_ERROR;
	return NO_ERROR;
}

float Get_CurrentMicInputLevelNormalized()
{
	if ( !waveIn.IsFileRecording())
		return 0.0f;
	return waveIn.GetCurrentInputLevel();
}