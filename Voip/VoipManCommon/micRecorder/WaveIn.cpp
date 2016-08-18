// WaveIn.cpp: implementation of the CWaveIn class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "WaveIn.h"
#include "..\soundLog\MP3Encoder.h"
#include "paec\paec.h"
#include <assert.h>

//////////////////////////////////////////////////////////////////////
void CALLBACK waveInProc(HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	switch( uMsg) 
	{
	case MM_WIM_DATA:
		WAVEHDR* pWaveHdr = ( (WAVEHDR*) dwParam1 );
		CWaveIn* pWaveIn = (CWaveIn*)( pWaveHdr->dwUser);

		if ( pWaveHdr && hwi && pWaveIn) 
		{
			if ( (pWaveHdr->dwFlags & WHDR_DONE) == WHDR_DONE) 
			{
				pWaveHdr->dwFlags = 0;
				if ( pWaveIn->IsError( waveInUnprepareHeader( hwi, pWaveHdr, sizeof( WAVEHDR))) ) 
				{
					break;
				}
				if (pWaveHdr->dwBytesRecorded > 0) 
				{
					pWaveIn->AddNewBuffer( pWaveHdr);
				}
				MP_DELETE_ARR(pWaveHdr->lpData);				
			}

			if ( !pWaveIn->ResetRequired( pWaveIn) )
			{
				if ( !pWaveIn->AddNewHeader( hwi) ) 
				{
					break;
				}
			}
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
CWaveIn::CWaveIn(WAVEFORMATEX tagFormat, const CWaveDevice& aDevice) : m_waveDevice(aDevice), \
	m_hWaveIn(0), m_nIndexWaveHdr(NUMWAVEINHDR - 1), m_bResetRequired(true)
{
	SetWaveFormat(tagFormat);
	m_pMP3Encoder = NULL;
	InitListOfHeader();
#ifdef HAS_AEC
	paec = NULL;
#endif
}

//////////////////////////////////////////////////////////////////////
CWaveIn::CWaveIn() : m_hWaveIn(0), m_bResetRequired(true)
{
	m_pMP3Encoder = NULL;
	InitListOfHeader();
#ifdef HAS_AEC
	paec = NULL;
#endif
}

//////////////////////////////////////////////////////////////////////
CWaveIn::~CWaveIn()
{
	Close();	
	if ( m_pMP3Encoder)
		MP_DELETE( m_pMP3Encoder);
	FreeListOfBuffer();
	FreeListOfHeader();
}

//////////////////////////////////////////////////////////////////////
// Initialization
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void CWaveIn::SetDevice(const CWaveDevice &aDevice)
{
	m_waveDevice = aDevice;
}

//////////////////////////////////////////////////////////////////////
void CWaveIn::SetWaveFormat(WAVEFORMATEX tagFormat)
{
	m_wave.BuildFormat(tagFormat.nChannels, tagFormat.nSamplesPerSec, tagFormat.wBitsPerSample);
	for ( int i = 0; i < NUMWAVEINHDR; i++) 
	{
		m_tagWaveHdr[ i].dwFlags = WHDR_DONE;
	}
}

//////////////////////////////////////////////////////////////////////
void CWaveIn::InitListOfHeader()
{
	for ( int i = 0; i < NUMWAVEINHDR; i++) 
	{
		m_tagWaveHdr[i].lpData = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
// Son
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
bool CWaveIn::Close()
{
	if ( m_hWaveIn != NULL) 
	{
		if ( !Stop() )
		{
			return false;
		}

		mutex.Wait();
		if ( m_pMP3Encoder)
		{
			m_pMP3Encoder->save();
			MP_DELETE( m_pMP3Encoder);			
		}
		mutex.Signal();

		if ( IsError( waveInClose(m_hWaveIn)) ) 
		{
			return false;
		}
		m_hWaveIn = 0;
	}


#ifdef HAS_AEC
	if ( paec)
	{
		MP_DELETE_ARR( paec);		
	}
#endif

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CWaveIn::Continue()
{
	if ( m_hWaveIn) 
	{
		return !IsError( waveInStart( m_hWaveIn));
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CWaveIn::Open()
{
	return !IsError( waveInOpen( &m_hWaveIn, m_waveDevice.GetDevice(), &m_wave.GetFormat(), (DWORD) waveInProc, NULL, CALLBACK_FUNCTION));
}


//////////////////////////////////////////////////////////////////////
bool CWaveIn::Pause()
{
	if (m_hWaveIn) {
		return !IsError( waveInStop(m_hWaveIn) );
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CWaveIn::Record( UINT nTaille/* = 4096*/)
{
	assert(nTaille > 0);
	assert(m_hWaveIn);

	if ( !Stop() ) 
	{
		return false;
	}
	m_bResetRequired = false;
	FreeListOfBuffer();
	FreeListOfHeader();
	SetWaveFormat( m_wave.GetFormat() );

#ifdef HAS_AEC
	if ( !paec)
		paec = MP_NEW_ARR( PAec, m_wave.GetFormat().nSamplesPerSec);
#endif
	
	m_nIndexWaveHdr = NUMWAVEINHDR - 1;
	m_nBufferSize = nTaille;
	for ( int i = 0; i < NUMWAVEINHDR; i++) 
	{
		//m_tagWaveHdr[ i].dwFlags = WHDR_DONE;
		if ( !AddNewHeader(m_hWaveIn) ) 
		{
			return false;
		}
	}
	if ( IsError(waveInStart(m_hWaveIn)) ) 
	{
		return false;
	}

	return true;
}

bool CWaveIn::Record2File( WCHAR *lpFileName, UINT nTaille /* = 4096 */)
{
	ATLASSERT( !m_pMP3Encoder);
	if ( m_pMP3Encoder)
		MP_DELETE( m_pMP3Encoder);
	mutex.Wait();
	USES_CONVERSION;
	MP_NEW_P( m_pMP3Encoder_, CMP3Encoder, W2A(lpFileName));
	m_pMP3Encoder = m_pMP3Encoder_;
	m_pMP3Encoder->setFileName( lpFileName);
	WAVEFORMATEX waveFormat = m_wave.GetFormat();
	m_pMP3Encoder->setParams( waveFormat.nChannels, waveFormat.nSamplesPerSec, waveFormat.wBitsPerSample);
	m_pMP3Encoder->setMP3FileParams( 16, JOINT_STEREO, 7, 22050);
	bool bSucc = Record( nTaille);
	if ( !bSucc)
	{
		ATLASSERT( bSucc);
		MP_DELETE( m_pMP3Encoder);
	}
	mutex.Signal();
	return bSucc;
}

bool CWaveIn::IsFileRecording()
{
	CWaitAndSignal waitAndSignal( mutex);
	return m_pMP3Encoder != NULL;
}

//////////////////////////////////////////////////////////////////////
bool CWaveIn::Stop()
{
	if ( m_hWaveIn != NULL) 
	{
		m_bResetRequired = true;
		::Sleep(10);
		if ( IsError(waveInReset(m_hWaveIn)) ) 
		{
			return false;
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CWaveIn::AddNewBuffer(WAVEHDR *pWaveHdr)
{
	assert(pWaveHdr);

	//m_listOfBuffer.AddTail(new CWaveBuffer);
	//( (CWaveBuffer*)m_listOfBuffer.GetTail() )->CopyBuffer( pWaveHdr->lpData, \
	//	pWaveHdr->dwBytesRecorded / m_wave.GetFormat().nBlockAlign, \
	//	m_wave.GetFormat().nBlockAlign );
	ATLASSERT( m_pMP3Encoder);
	if ( !m_pMP3Encoder)
		return false;

#ifdef HAS_AEC
	if ( paec)
		paec->Send( (BYTE*) pWaveHdr->lpData, (unsigned int &) pWaveHdr->dwBytesRecorded);
#endif

	short* input = (short*)pWaveHdr->lpData;
	int leftVal;
	int rightVal;
	int size = pWaveHdr->dwBytesRecorded;
	m_fCurrentInputLevel = 0.0f;

	for ( int i = 0; i < size; i+=4)
	{
		leftVal = abs( *input);
		if ( m_fCurrentInputLevel < leftVal)
			m_fCurrentInputLevel = (float)leftVal;
		input++;
		rightVal = abs( *input);
		if ( m_fCurrentInputLevel < rightVal)
			m_fCurrentInputLevel = (float)rightVal;
	}

	m_pMP3Encoder->addData( pWaveHdr->lpData, pWaveHdr->dwBytesRecorded);

	m_fCurrentInputLevel /= 32768.0f;

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CWaveIn::AddNewHeader(HWAVEIN hwi)
{
	assert(m_nBufferSize > 0);

	m_nIndexWaveHdr = (m_nIndexWaveHdr == NUMWAVEINHDR - 1) ? 0 : m_nIndexWaveHdr + 1;
	if (m_tagWaveHdr[m_nIndexWaveHdr].lpData == NULL) {
		m_tagWaveHdr[m_nIndexWaveHdr].lpData = MP_NEW_ARR( char, m_nBufferSize);
	}
	ZeroMemory(m_tagWaveHdr[m_nIndexWaveHdr].lpData, m_nBufferSize);
	m_tagWaveHdr[m_nIndexWaveHdr].dwBufferLength = m_nBufferSize;
	m_tagWaveHdr[m_nIndexWaveHdr].dwFlags = 0;
	m_tagWaveHdr[m_nIndexWaveHdr].dwUser = (DWORD)(void*)this;
	if ( IsError(waveInPrepareHeader(hwi, &m_tagWaveHdr[m_nIndexWaveHdr], sizeof(WAVEHDR))) ) {
		return false;
	}
	if ( IsError(waveInAddBuffer(hwi, &m_tagWaveHdr[m_nIndexWaveHdr], sizeof(WAVEHDR))) ) {
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
void CWaveIn::FreeListOfHeader()
{
	for (int i = 0; i < NUMWAVEINHDR; i++) {
		if (m_tagWaveHdr[i].lpData != NULL)
			MP_DELETE_ARR(m_tagWaveHdr[i].lpData);		
	}
}

//////////////////////////////////////////////////////////////////////
void CWaveIn::FreeListOfBuffer()
{
	/*POSITION pos = m_listOfBuffer.GetHeadPosition();
	while (pos) {
		CWaveBuffer* pBuf = (CWaveBuffer*)m_listOfBuffer.GetNext(pos);
		if (pBuf) {
			MP_DELETE( pBuf);
			pBuf = NULL;
		}
	}
	m_listOfBuffer.RemoveAll();
	*/
}

//////////////////////////////////////////////////////////////////////
// GET
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
DWORD CWaveIn::GetNumSamples()
{
	/*DWORD dwTotal = 0L;
	POSITION pos = m_listOfBuffer.GetHeadPosition();
	while (pos) {
		CWaveBuffer* p_waveBuffer = (CWaveBuffer*) m_listOfBuffer.GetNext(pos);
		dwTotal += p_waveBuffer->GetNumSamples();
	}
	return dwTotal;*/
	return 0;
}

//////////////////////////////////////////////////////////////////////
CComString CWaveIn::GetError() const
{
	if (m_nError != MMSYSERR_NOERROR) {
		TCHAR szText[MAXERRORLENGTH + 1];
		if ( waveInGetErrorText(m_nError, szText, MAXERRORLENGTH) == MMSYSERR_NOERROR ) {
			return szText;
		}
	}
	return "";
}

//////////////////////////////////////////////////////////////////////
DWORD CWaveIn::GetPosition()
{
	if (m_hWaveIn) {
		MMTIME mmt;
		mmt.wType = TIME_SAMPLES;
		if ( IsError(waveInGetPosition(m_hWaveIn, &mmt, sizeof(MMTIME))) ) {
			return -1;
		}
		else {
			return mmt.u.sample;
		}
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////
bool CWaveIn::IsError(MMRESULT nResult)
{
	m_nError = nResult;
	return (m_nError != MMSYSERR_NOERROR);
}

//////////////////////////////////////////////////////////////////////
bool CWaveIn::IsRecording()
{
	bool bResult = false;
	if (m_nIndexWaveHdr > -1 && m_tagWaveHdr[m_nIndexWaveHdr].dwFlags != 0) {
		bResult |= !((m_tagWaveHdr[m_nIndexWaveHdr].dwFlags & WHDR_DONE) == WHDR_DONE);
	}
	return bResult;
}

//////////////////////////////////////////////////////////////////////
CWave CWaveIn::MakeWave()
{
	return m_wave;
}


//////////////////////////////////////////////////////////////////////
bool CWaveIn::ResetRequired(CWaveIn* pWaveIn)
{
	return m_bResetRequired;
}

HWAVEIN CWaveIn::getHWaveIn()
{
	return m_hWaveIn;
}

WCHAR *CWaveIn::getFileName()
{
	ATLASSERT( m_pMP3Encoder);
	if ( !m_pMP3Encoder)
		return L"";
	return m_pMP3Encoder->getFileName();
}

float CWaveIn::GetCurrentInputLevel()
{
	return m_fCurrentInputLevel;
}