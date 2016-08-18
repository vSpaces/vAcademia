#include <StdAfx.h>

#include "RecordState.h"
#include <tchar.h>


CRecordState::CRecordState()
{
	bRecordState = FALSE;
	wmemset( sFileName, L'\0', MAX_PATH);
	bBackgroundNoise = FALSE;
	channels = 1;
	samples_per_sec = 8000;
	bits_per_sample = 16;
}

CRecordState::~CRecordState()
{		
}

//////////////////////////////////////////////////////////////////////////

void CRecordState::setFileName( wchar_t *sFileName)
{
	wmemset( this->sFileName, L'\0', MAX_PATH);
	wcscpy_s( this->sFileName, MAX_PATH, sFileName);
}

void CRecordState::setRecordState( BOOL bRecordState)
{
	this->bRecordState = bRecordState;
}

wchar_t *CRecordState::getFileName()
{
	return sFileName;
}

BOOL CRecordState::getRecordState()
{
	return bRecordState;
}

/*void CRecordState::setBlockReady( BOOL bBlockReady)
{
	this->bBlockReady = bBlockReady;
}

BOOL CRecordState::getBlockReady()
{
	return bBlockReady;
}*/

void CRecordState::setBackgroundNoise( BOOL bBackgroundNoise)
{
	this->bBackgroundNoise = bBackgroundNoise;
}

BOOL CRecordState::getBackgroundNoise()
{
	return bBackgroundNoise;
}

void CRecordState::setChannels( int channels)
{
	this->channels = channels;
}

int CRecordState::getChannels()
{
	return channels;
}

void CRecordState::setSamples_per_sec( int samples_per_sec)
{
	this->samples_per_sec = samples_per_sec;
}

int CRecordState::getSamples_per_sec()
{
	return samples_per_sec;
}

void CRecordState::setBits_per_sample( int bits_per_sample)
{
	this->bits_per_sample = bits_per_sample;
}

int CRecordState::getBits_per_sample()
{
	return bits_per_sample;
}

