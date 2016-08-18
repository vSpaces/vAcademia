// Wave.cpp: implementation of the CWave class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wave.h"
#include <assert.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWave::CWave()
{
	ZeroMemory((void*)&m_pcmWaveFormat, sizeof(m_pcmWaveFormat));
	m_pcmWaveFormat.wFormatTag = 1;
}

//////////////////////////////////////////////////////////////////////
CWave::CWave( const CWave &copy)
{
	m_pcmWaveFormat = copy.GetFormat();
	m_buffer.SetNumSamples( copy.GetNumSamples(), copy.GetFormat().nBlockAlign ) ;
	m_buffer.CopyBuffer( copy.GetBuffer(), copy.GetNumSamples(), copy.GetFormat().nBlockAlign );
}

//////////////////////////////////////////////////////////////////////
CWave& CWave::operator = ( const CWave& wave)
{
	if ( &wave != this) 
	{
		m_pcmWaveFormat = wave.GetFormat();
		m_buffer.SetNumSamples( wave.GetNumSamples(), wave.GetFormat().nBlockAlign );
		m_buffer.CopyBuffer( wave.GetBuffer(), wave.GetNumSamples(), wave.GetFormat().nBlockAlign );
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////
CWave::~CWave()
{
}

//////////////////////////////////////////////////////////////////////
void CWave::BuildFormat( WORD nChannels, DWORD nFrequency, WORD nBits)
{
	m_pcmWaveFormat.wFormatTag = WAVE_FORMAT_PCM;
	m_pcmWaveFormat.nChannels = nChannels;
	m_pcmWaveFormat.nSamplesPerSec = nFrequency;
	m_pcmWaveFormat.nBlockAlign = nChannels * nBits / 8;
	m_pcmWaveFormat.nAvgBytesPerSec = nFrequency * m_pcmWaveFormat.nBlockAlign;	
	m_pcmWaveFormat.wBitsPerSample = nBits;
	m_buffer.SetNumSamples( 0L, m_pcmWaveFormat.nBlockAlign);
}	

//////////////////////////////////////////////////////////////////////
inline WAVEFORMATEX CWave::GetFormat() const
{
	return m_pcmWaveFormat;
}

//////////////////////////////////////////////////////////////////////
void* CWave::GetBuffer() const
{
	return m_buffer.GetBuffer();
}

//////////////////////////////////////////////////////////////////////
DWORD CWave::GetNumSamples() const
{
	return m_buffer.GetNumSamples();
}

//////////////////////////////////////////////////////////////////////
DWORD CWave::GetBufferLength() const
{
	return ( m_buffer.GetNumSamples() * m_pcmWaveFormat.nBlockAlign );
}

//////////////////////////////////////////////////////////////////////
void CWave::SetBuffer( void* pBuffer, DWORD dwNumSample, bool bCopy)
{
	assert( pBuffer);
	assert( dwNumSample > 0);
	assert( m_pcmWaveFormat.nBlockAlign > 0);

	if ( bCopy) 
	{
		m_buffer.CopyBuffer( pBuffer, dwNumSample, m_pcmWaveFormat.nBlockAlign);
	}
	else 
	{
		m_buffer.SetBuffer( pBuffer, dwNumSample, m_pcmWaveFormat.nBlockAlign);
	}
}