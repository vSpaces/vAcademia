// WaveBuffer.h: interface for the CWaveBuffer class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CWaveBuffer  
{
public:
	int GetSampleSize() const;
	void SetBuffer(void* pBuffer, DWORD dwNumSamples, int nSize);
	void SetNumSamples(DWORD dwNumSamples, int nSize = sizeof(short));
	void CopyBuffer(void* pBuffer, DWORD dwNumSamples, int nSize = sizeof(short));
	DWORD GetNumSamples() const;
	void* GetBuffer() const;
	CWaveBuffer();
	virtual ~CWaveBuffer();

private:
	int m_nSampleSize;
	void* m_pBuffer;
	DWORD m_dwNum;
};

