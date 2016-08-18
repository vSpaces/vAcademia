// WaveBuffer.cpp: implementation of the CWaveBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WaveBuffer.h"
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
CWaveBuffer::CWaveBuffer() : m_dwNum(0), m_pBuffer(NULL), m_nSampleSize(0)
{
}

//////////////////////////////////////////////////////////////////////
CWaveBuffer::~CWaveBuffer()
{
	m_dwNum = 0L;
	if( m_pBuffer != NULL)
	{
		MP_DELETE_ARR( m_pBuffer);	
	}
}

//////////////////////////////////////////////////////////////////////
void* CWaveBuffer::GetBuffer() const
{
	return m_pBuffer;
}

//////////////////////////////////////////////////////////////////////
DWORD CWaveBuffer::GetNumSamples() const
{
	return m_dwNum;
}

//////////////////////////////////////////////////////////////////////
void CWaveBuffer::CopyBuffer(void* pBuffer, DWORD dwNumSamples, int nSize)
{
	assert(dwNumSamples >= 0);
	assert(nSize);

	if (!m_pBuffer)
		SetNumSamples(dwNumSamples, nSize);

	if (__min(m_dwNum, dwNumSamples) * nSize > 0) {
		ZeroMemory(m_pBuffer, m_dwNum * m_nSampleSize);
		CopyMemory(m_pBuffer, pBuffer, __min(m_dwNum, dwNumSamples) * nSize);
	}
}

//////////////////////////////////////////////////////////////////////
void CWaveBuffer::SetNumSamples(DWORD dwNumSamples, int nSize)
{
	assert(dwNumSamples >= 0);
	assert(nSize > 0);

	if (dwNumSamples == 0 || nSize == 0)
		return;

	void* pBuffer = NULL;

	pBuffer = MP_NEW_ARR( char, nSize * dwNumSamples);
	SetBuffer(pBuffer, dwNumSamples, nSize);
}

//////////////////////////////////////////////////////////////////////
void CWaveBuffer::SetBuffer(void *pBuffer, DWORD dwNumSamples, int nSize)
{
	assert(dwNumSamples >= 0);
	assert(nSize);

	if( m_pBuffer != NULL)
	{
		MP_DELETE_ARR(m_pBuffer);
	}
	m_pBuffer = pBuffer;
	m_dwNum = dwNumSamples;
	m_nSampleSize = nSize;
}

//////////////////////////////////////////////////////////////////////
int CWaveBuffer::GetSampleSize() const
{
	return m_nSampleSize;
}
