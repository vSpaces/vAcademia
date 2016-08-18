#include "stdafx.h"
#include "SndFileLoader_Player.h"
#include "SoundPtr.h"

CSndFileLoaderPlayer::CSndFileLoaderPlayer( ISoundPtrBase *pSoundPtr)
{
	m_pSoundPtr = pSoundPtr;
	bCreateBufferNeed = FALSE;
	bReleaseBufferNeed = FALSE;
	bReleaseSoundNeed = FALSE;
}

CSndFileLoaderPlayer::~CSndFileLoaderPlayer()
{
	if ( m_handle != NULL)
	{
		terminating();
		int i = 0;
		while( true)
		{
			i++;
			if ( m_bTerminated || i > 10)
				break;
			Sleep( 30);
		}
	}
	terminated();
	m_pSoundPtr = NULL;
}

//////////////////////////////////////////////////////////////////////////

BOOL CSndFileLoaderPlayer::load( AudioDevicePtr device, File *resFile)
{
	ATLASSERT( device);
	ATLASSERT( resFile);
	if ( !device || !resFile)
		return FALSE;
	m_pDevice = device;
	m_pResFile = resFile;
	bCreateBufferNeed = FALSE;
	bReleaseBufferNeed = FALSE;
	bReleaseSoundNeed = FALSE;
	return create( NULL);
}

#include <signal.h>

class CTestFailed
{
public:
	CTestFailed(){ x = 0;};
	~CTestFailed(){};

	void run()
	{
		x++;
	}
	int x;
};

#define BIG_NUMBER 0x00ffffff

void recure_alloc()
{
	int *pi = new int[ BIG_NUMBER];
	pi;
	int ax = 0;
	recure_alloc();
}

void CSndFileLoaderPlayer::main()
{
	if ( m_bTerminating)
		return;

	if ( m_pSoundPtr->open( m_pDevice, m_pResFile, NULL))
	{
		if ( m_bTerminated || m_bTerminating)
			return;
		m_pSoundPtr->on_loaded( 0);
		//m_pSoundPtr->on_play_starting();
		while( true)
		{
			// бесконечный цикл нужен, чтобы проигрывался звук
			if ( m_bTerminating)
				break;

			if ( bReleaseBufferNeed)
			{
				(( CSoundStreamPtr*) m_pSoundPtr)->release_buffer();
				bReleaseBufferNeed = FALSE;
			}
			if ( bCreateBufferNeed)
			{
				(( CSoundStreamPtr*) m_pSoundPtr)->create_buffer();
				bCreateBufferNeed = FALSE;
			}

			if ( bReleaseSoundNeed)
			{
				(( CSoundStreamPtr*) m_pSoundPtr)->release_sound();
				bReleaseSoundNeed = FALSE;
			}

			Sleep( 10);
		}
	}
	else 
	{
		if ( m_bTerminated || m_bTerminating)
			return;
		m_pSoundPtr->on_loaded( 1);
	}
	m_bTerminated = TRUE;
}

void CSndFileLoaderPlayer::createBuffer()
{
	bCreateBufferNeed = TRUE;
}

void CSndFileLoaderPlayer::releaseBuffer()
{
	bReleaseBufferNeed = TRUE;
}

void CSndFileLoaderPlayer::releaseSound()
{
	bReleaseSoundNeed = TRUE;
}