// SoundBuffer.cpp: implementation of the SDSoundBuffer and sound_buf classes
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SoundPtr.h"
#include "SndFileLoader_Player.h"
#include "StopCallback.h"
//#include "..\nengine\nengine\GlobalSingletonStorage.h"

#define G_BYTES2MS 1000

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

template <class TSound>
CSoundPtrBase<TSound>::CSoundPtrBase()
{	
#if AUDIERE_RELEASE_O
#else
	m_pStopCallBack = NULL;
#endif
	m_pSndFileLoaderPlayer = NULL;
	init();	
}

template <class TSound>
CSoundPtrBase<TSound>::~CSoundPtrBase()
{
	lock();
	m_bTerminating = TRUE;
	un_lock();	
#if AUDIERE_RELEASE_O
#else
	if (m_pStopCallBack)
	{
		m_pStopCallBack.DestroyPtr();
		m_pStopCallBack = NULL;
	}
#endif
	MP_DELETE( m_pSndFileLoaderPlayer);
	clear();
}

//////////////////////////////////////////////////////////////////////////

template <class TSound>
void CSoundPtrBase<TSound>::init()
{
	m_bTerminating			= FALSE;
	m_pISoundNotify			= NULL;	
	m_iId					= -1;
	m_bLoadedFile			= FALSE;
	m_bLoop					= FALSE;
	m_iPlay					= 0;
	m_sound					= NULL;
	m_fVolume				= 1.0f;
	m_fPanVolume			= 0.0f;
	m_dwPosition			= 0;
	m_bLoading				= FALSE;
	m_bStartPlay			= FALSE;
	m_bEnabled				= TRUE;
	m_bFreezzed				= FALSE;
	m_bParamsSaver			= FALSE;
	m_bStream				= TRUE;
	m_pResFile				= NULL;
	MP_NEW_P( pSndFileLoaderPlayer, CSndFileLoaderPlayer, this);
	m_pSndFileLoaderPlayer	= pSndFileLoaderPlayer;
	m_bStopped				= FALSE;
	m_bASndRemoved			= FALSE;
}

template <class TSound>
void CSoundPtrBase<TSound>::set_notify( sm::smISoundNotify *apISoundNotify)
{
	//ATLASSERT( apISoundNotify);
	m_mutexSoundNotify.lock();
	m_pISoundNotify = apISoundNotify;
	m_mutexSoundNotify.unlock();
}

template <class TSound>
sm::smISoundNotify* CSoundPtrBase<TSound>::get_notify()
{	
	CWaitAndSignal waitAndSignal(m_mutexSoundNotify);
	return m_pISoundNotify;
}

template <class TSound>
void CSoundPtrBase<TSound>::set_id( int id)
{
	this->m_iId = id;
}

template <class TSound>
int CSoundPtrBase<TSound>::get_id()
{	
	return m_iId;
}

template <class TSound>
void CSoundPtrBase<TSound>::set_name( const wchar_t *fileName)
{	
	if ( fileName)
		m_sName = CWComString( fileName);
}

template <class TSound>
const wchar_t *CSoundPtrBase<TSound>::get_name()
{
	return m_sName.GetBuffer();
}

//////////////////////////////////////////////////////////////////////////

template <class TSound>
void CSoundPtrBase<TSound>::lock()
{
	m_mutex.lock();
}

template <class TSound>
void CSoundPtrBase<TSound>::un_lock()
{
	m_mutex.unlock();
}

template <class TSound>
DWORD CSoundPtrBase<TSound>::get_pos( int flag)
{
	// реализация должна быть в классе-наследнике
	return -1;
}

template <class TSound>
void CSoundPtrBase<TSound>::set_pos( DWORD pos)
{
	// реализация должна быть в классе-наследнике
}

template <class TSound>
float CSoundPtrBase<TSound>::get_volume()
{		
	if ( !is_loaded())
	{
		return 1.0f;
	}	

	if ( m_sound != NULL)
	{
		return m_sound->getVolume();
	}
	return 1.0f;
}

template <class TSound>
void CSoundPtrBase<TSound>::set_volume( float volume)
{	
	if ( is_params_saver() || is_loading() || !is_loaded())
	{
		this->m_fVolume = volume;
		return;
	}

	if ( !is_enabled())
	{
		return;
	}

	lock();
	ATLASSERT( m_sound);
	if ( m_sound != NULL)
	{
		this->m_fVolume = volume;		
		if ( m_bFreezzed)
		{
			un_lock();
			return;
		}		
		m_sound->setVolume( volume);
	}
	un_lock();
}

template <class TSound>
float CSoundPtrBase<TSound>::get_pan()
{
	if ( !is_loaded())
	{
		return 0.0f;
	}

	ATLASSERT( m_sound);
	if ( m_sound != NULL)
		return m_sound->getPan();
	return 0.0f;
}

template <class TSound>
void CSoundPtrBase<TSound>::set_pan( float pan)
{
	if ( is_params_saver() || is_loading() || !is_loaded())
	{
		m_fPanVolume = pan;
		return;
	}
	ATLASSERT( m_sound);
	if ( m_sound != NULL && is_enabled())
	{
		m_fPanVolume = pan;
		lock();
		if ( m_bFreezzed)
		{
			un_lock();
			return;
		}		
		m_sound->setPan( pan);
		un_lock();
	}
}

template <class TSound>
BOOL CSoundPtrBase<TSound>::play()
{
	if ( !is_loaded())
	{
		return FALSE;
	}

	ATLASSERT( m_sound);
	if ( m_sound != NULL && is_enabled())
	{			
		m_iPlay = 1;

		lock();
		if ( !m_bFreezzed)
		{
			m_sound->play();
		}
		un_lock();
		
		m_bStartPlay = FALSE;
		m_bStopped = FALSE;
		return TRUE;
	}
	return FALSE;
}

template <class TSound>
BOOL CSoundPtrBase<TSound>::is_play()
{
	// реализация должна быть в классе-наследнике
	return 0;		
}

template <class TSound>
BOOL CSoundPtrBase<TSound>::is_playing()
{
	return false;
}

template <class TSound>
void CSoundPtrBase<TSound>::stop()
{
	m_bStartPlay = FALSE;
	m_iPlay = 0;
	if ( !is_loaded())
	{
		return;
	}
	if ( m_sound != NULL)
	{
		lock();
		if ( !m_bFreezzed)
		{
			m_sound->stop();
		}
		un_lock();
	}
}

#if AUDIERE_RELEASE_P
template <class TSound>
void CSoundPtrBase<TSound>::stop_and_wait()
{
	stop();
}
#endif

template <class TSound>
void CSoundPtrBase<TSound>::set_repeat()
{
	// реализация должна быть в классе-наследнике
}

template <class TSound>
void CSoundPtrBase<TSound>::set_loop( BOOL bLoop)
{
	this->m_bLoop = bLoop;	
}

template <class TSound>
BOOL CSoundPtrBase<TSound>::is_loop()
{
	return m_bLoop;
}

template <class TSound>
BOOL CSoundPtrBase<TSound>::is_seekable()
{
	// реализация должна быть в классе-наследнике
	return TRUE;
}

template <class TSound>
DWORD CSoundPtrBase<TSound>::get_frequency()
{
	// реализация должна быть в классе-наследнике
	return 0;
}

template <class TSound>
DWORD CSoundPtrBase<TSound>::get_length_in_ms()
{	
	if ( !is_loaded())
	{
		return 0;
	}
	ATLASSERT( m_sound);
	//if ( sound != NULL)
	//	return sound->getLengthInMS();
	return 0;
}

#if AUDIERE_BUILD_H
template <class TSound>
DWORD CSoundPtrBase<TSound>::get_size()
{
	if ( !is_loaded())
	{
		return 0;
	}
	ATLASSERT( m_sound);
	//if ( sound != NULL)
	//	return sound->getLengthInMS();
	return 0;
}
#endif

#if SOUND_MAN_RELEASE_H
template <class TSound>
DWORD CSoundPtrBase<TSound>::get_current_size()
{
	if ( !is_loaded())
	{
		return 0;
	}
	ATLASSERT( m_sound);	
	return 0;
}
#endif

template <class TSound>
DWORD CSoundPtrBase<TSound>::get_length()
{
	// реализация должна быть в классе-наследнике
	return 0;
}

template <class TSound>
DWORD CSoundPtrBase<TSound>::get_buffer_size()
{
	// реализация должна быть в классе-наследнике
	return 0;
}

template <class TSound>
LPSTR CSoundPtrBase<TSound>::get_buffer( DWORD *size)
{
	// реализация должна быть в классе-наследнике
	*size = 0;
	return NULL;
}

template <class TSound>
void CSoundPtrBase<TSound>::set_pitch_shift( float shift)
{
	// реализация должна быть в классе-наследнике
}

template <class TSound>
float CSoundPtrBase<TSound>::get_pitch_shift( float shift)
{
	// реализация должна быть в классе-наследнике
	return 0;
}

//////////////////////////////////////////////////////////////////////////

template <class TSound>
BOOL CSoundPtrBase<TSound>::on_loaded( int iError)
{	
	if ( iError == 0)
	{
		lock();
		m_bLoadedFile = TRUE;
		if ( !m_bLoading)
		{
			un_lock();
			return TRUE;
		}
		set_loading( FALSE);		
		if ( m_bTerminating)
		{
			un_lock();
			return TRUE;
		}
		un_lock();
		m_mutexSoundNotify.lock();
		if ( m_pISoundNotify)
		{			
			m_pISoundNotify->OnLoaded( iError);
		}
		m_mutexSoundNotify.unlock();

		lock();
		if ( m_bStartPlay)
		{
			un_lock();
			apply_params();			
			BOOL bSucc = play();
			on_play_started( bSucc);
			set_repeat();
			return bSucc;
		}
		un_lock();
		apply_params();
	}
	else
	{		
		lock();
		set_loading( FALSE);
		if ( m_bTerminating)
		{
			un_lock();
			return TRUE;
		}
		un_lock();
		m_mutexSoundNotify.lock();
		if ( m_pISoundNotify)
		{
			m_pISoundNotify->OnLoaded( iError);
		}
		m_mutexSoundNotify.unlock();
	}
	
	return TRUE;
}

template <class TSound>
BOOL CSoundPtrBase<TSound>::on_play_starting()
{
	if ( m_bStream)
	{
		return TRUE;
	}

	lock();	
	if ( m_bStartPlay)
	{
		un_lock();
		apply_params();		
		BOOL bSucc = play();
		on_play_started( bSucc);
		set_repeat();
		return bSucc;
	}
	un_lock();
	return TRUE;
}

template <class TSound>
BOOL CSoundPtrBase<TSound>::on_play_started( int iError)
{	
	return TRUE;
}

template <class TSound>
void CSoundPtrBase<TSound>::on_stopped( StopEvent::Reason reason)
{
	if ( reason == StopEvent::STOP_CALLED)
	{
		m_bStartPlay = FALSE;
		m_iPlay = 0;
		m_mutexSoundNotify.lock();
		if ( m_pISoundNotify)
		{
			m_pISoundNotify->OnStopped( STOP_BY_USER);
		}
		m_mutexSoundNotify.unlock();
	}
	else if ( m_iPlay && is_loaded())
	{
		m_bStartPlay = FALSE;
#pragma message ("Флагу m_iPlay не присваиваться FALSE при достижении конца файла")
		//m_iPlay = FALSE;
#if AUDIERE_RELEASE_O
		m_mutexSoundNotify.lock();
		if ( m_pISoundNotify)
		{
			m_pISoundNotify->OnStopped( STOP_BY_ENDFILE);
		}
		m_mutexSoundNotify.unlock();
#endif
	}
	m_bStopped = TRUE;
}

//////////////////////////////////////////////////////////////////////////

template <class TSound>
BOOL CSoundPtrBase<TSound>::open_async( AudioDevicePtr device, File *resFile, const wchar_t *fileName)
{
	lock();
	set_loading( TRUE);
	un_lock();
	ATLASSERT( fileName);
	ATLASSERT( resFile);

	if ( fileName == NULL || resFile == NULL)
		return FALSE;

	if ( fileName && resFile)
	{
		lock();
		m_pResFile = resFile;
		un_lock();
		set_name( fileName);
	}

	//m_pSndFileLoaderPlayer->terminated();
#if AUDIERE_RELEASE_O
#else
	if ( m_pStopCallBack == NULL)
	{
		MP_NEW_P( pStopCallBack, CStopCallback, this);
		m_pStopCallBack	= pStopCallBack;	
	}
#endif
	m_pSndFileLoaderPlayer->load( device, resFile);	
	return TRUE;
}

template <class TSound>
void CSoundPtrBase<TSound>::set_loading( BOOL bLoading)
{ 
	this->m_bLoading = bLoading;
}

template <class TSound>
BOOL CSoundPtrBase<TSound>::is_loading()
{
	return m_bLoading;
}

template <class TSound>
BOOL CSoundPtrBase<TSound>::is_loaded()
{
	//CWaitAndSignal waitAndSignal( m_mutex);
	return m_bLoadedFile;
}

template <class TSound>
void CSoundPtrBase<TSound>::close()
{
	lock();
	m_bLoadedFile = FALSE;
	if ( m_sound != NULL)
	{		
		m_sound.DestroyPtr();
		m_sound = NULL;		
	}	
	un_lock();
}

template <class TSound>
void CSoundPtrBase<TSound>::clear()
{
	if ( m_sound)
	{
		stop();
		close();
		//_clear();
	}
}

//////////////////////////////////////////////////////////////////////////

// устанавливает параметры, что звук начнется воспроизводится
template <class TSound>
void CSoundPtrBase<TSound>::set_play( int iPlay)
{
	m_bStartPlay = iPlay;
	m_iPlay = iPlay;
	m_bStopped = FALSE;
}

template <class TSound>
void CSoundPtrBase<TSound>::enabled()
{
	m_bEnabled = TRUE;
}

template <class TSound>
void CSoundPtrBase<TSound>::disabled()
{
	m_bEnabled = FALSE;
}

template <class TSound>
BOOL CSoundPtrBase<TSound>::is_enabled()
{
	return m_bEnabled;
}

template <class TSound>
void CSoundPtrBase<TSound>::params_saver()
{
	m_bParamsSaver = TRUE;
}

template <class TSound>
BOOL CSoundPtrBase<TSound>::apply_params()
{
	if ( m_bParamsSaver)
	{
		m_bParamsSaver = FALSE;
		ATLASSERT( m_sound);
		if ( m_sound)
		{
			_apply_params();
			return TRUE;
		}
	}
	return FALSE;
}

template <class TSound>
BOOL CSoundPtrBase<TSound>::is_params_saver()
{
	return m_bParamsSaver;
}

template <class TSound>
ISndFileLoaderPlayer *CSoundPtrBase<TSound>::get_snd_file_loader_player()
{
	return (ISndFileLoaderPlayer*) m_pSndFileLoaderPlayer;
}

#if AUDIERE_BUILD_H
template <class TSound>
int CSoundPtrBase<TSound>::update_file_info( int loaded_bytes, bool bLoaded)
{
	return 0;
}
#endif

//////////////////////////////////////////////////////////////////////////

template <class TSound>
void CSoundPtrBase<TSound>::_apply_params()
{
	if ( get_pan() != m_fPanVolume)
		set_pan( m_fPanVolume);
	if ( get_volume() != m_fVolume)
		set_volume( m_fVolume);
	if ( get_pos( 0) != m_dwPosition)
		set_pos( m_dwPosition);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CSoundStreamPtr::CSoundStreamPtr()
{
	
}

CSoundStreamPtr::~CSoundStreamPtr()
{
	m_mutexSoundNotify.lock();
	m_pISoundNotify = NULL;
	m_mutexSoundNotify.unlock();
	clear();
}

BOOL CSoundStreamPtr::open( AudioDevicePtr device, File *resFile, const wchar_t *fileName)
{
	return open( device, resFile, fileName, m_bStream);
}

int strcmp_case(const wchar_t* a, const wchar_t* b) {
	while (*a && *b) {
		wchar_t c = towlower(*a++);
		wchar_t d = towlower(*b++);

		if (c != d) {
			return c - d;
		}
	}

	wchar_t c = tolower(*a);
	wchar_t d = tolower(*b);
	return (c - d);
}

bool end_is(const wchar_t* begin, const wchar_t* ext) {
	const wchar_t* end = begin + wcslen(begin);
	int ext_length = wcslen(ext);
	if (ext_length > end - begin) {
		return false;
	} else {
		return (strcmp_case(end - ext_length, ext) == 0);
	}
}

FileFormat GuessFormat(const wchar_t* filename) {
	if (end_is(filename, L".aiff")) {
		return FF_AIFF;
	} else if (end_is(filename, L".wav")) {
		return FF_WAV;
	} else if (end_is(filename, L".wave")) {
		return FF_WAV;
	} else if (end_is(filename, L".ogg")) {
		return FF_OGG;
	} else if (end_is(filename, L".flac")) {
		return FF_FLAC;
	} else if (end_is(filename, L".mp3")) {
		return FF_MP3;
	} else if (end_is(filename, L".it") ||
		end_is(filename, L".xm") ||
		end_is(filename, L".s3m") ||
		end_is(filename, L".mod")) {
			return FF_MOD;
		} else if (end_is(filename, L".spx")) {
			return FF_SPEEX;
		} else {
			return FF_AUTODETECT;
		}
}

BOOL CSoundStreamPtr::open( AudioDevicePtr device, File *resFile, const wchar_t *fileName, bool streaming)
{
	if ( fileName)
	{
		lock();
		m_pResFile = resFile;
		un_lock();
		set_name( fileName);	
	}
	FileFormat file_format = GuessFormat( get_name());
	ATLASSERT( file_format != FF_AUTODETECT);

	OutputStreamPtr pSound = audiere::OpenSound( device, resFile, streaming, file_format);	
	lock();
	m_sound = pSound;	
	if ( m_sound != NULL)
	{
		m_sound->setSoundPtrItem( this);
		m_pDevice = device;		
		//m_bLoadedFile = TRUE;
		un_lock();
#if AUDIERE_RELEASE_O
#else
	#if AUDIERE_BUILD_H
		if ( m_pStopCallBack)
			m_pDevice->registerCallback( m_pStopCallBack.get());
	#endif
#endif
	}
	else
	{
#if AUDIERE_BUILD_H
		m_pResFile = NULL;
		un_lock();
#endif
	}
	return m_sound != NULL;
}

void CSoundStreamPtr::close()
{
#if AUDIERE_RELEASE_O
#else
#if AUDIERE_BUILD_H
	if ( m_pStopCallBack)
	{
		m_pDevice->unregisterCallback( m_pStopCallBack.get());
		m_pStopCallBack.DestroyPtr();
		m_pStopCallBack = NULL;		
	}
#endif
#endif
	CSoundPtrBase<OutputStreamPtr>::close();
}

void CSoundStreamPtr::clear()
{
	if ( m_sound)
	{
		stop();
		close();
		//_clear();
	}
}


//////////////////////////////////////////////////////////////////////////

DWORD CSoundStreamPtr::get_pos( int flag)
{
	if ( !is_loaded())
	{
		return 0;
	}

	ATLASSERT( m_sound);
	if ( m_sound != NULL)
	{
		lock();
		if ( m_bFreezzed)
		{
			un_lock();
			return m_dwPosition;
		}
		un_lock();

		int freq = m_sound->getFrequency();
		if( freq == 0)
			return -1;
		DWORD64 dw_pos = (DWORD64) G_BYTES2MS * (DWORD64) m_sound->getPosition() / freq;
		return dw_pos;
	}
	return -1;
};

void CSoundStreamPtr::set_pos( DWORD pos)
{
	if ( is_params_saver() || is_loading() || !is_loaded())
	{
		this->m_dwPosition = pos;
		return;
	}

	ATLASSERT( m_sound);
	if ( m_sound != NULL)
	{
		this->m_dwPosition = pos;
		lock();
		if ( m_bFreezzed)
		{
			un_lock();
			return;
		}
		un_lock();
		DWORD64 dw_pos = (DWORD64) m_sound->getFrequency() * (DWORD64) pos / G_BYTES2MS;
		return m_sound->setPosition( dw_pos);
	}
};

BOOL CSoundStreamPtr::is_play()
{
	if ( !is_loaded())
	{
		return FALSE;
	}

	if ( m_sound != NULL)
	{
		BOOL bPlay_ = m_sound->isPlaying();
		if ( bPlay_)
			m_bStartPlay = FALSE;
		if ( m_bStartPlay)
			return TRUE;
		return bPlay_;		
	}
	return FALSE;
}

void CSoundStreamPtr::release()
{
	MP_DELETE_THIS;
}

BOOL CSoundStreamPtr::is_playing()
{
	if ( !is_loaded())
	{
		return FALSE;
	}

	if ( m_sound != NULL)
		return m_sound->isPlaying();
	return FALSE;
}

BOOL CSoundStreamPtr::is_stopped()
{
#if AUDIERE_RELEASE_O
	return m_bStopped;
#else
	if ( m_pStopCallBack)
		return m_bStopped;

	return !is_playing();
#endif
}

#if AUDIERE_RELEASE_P
void CSoundStreamPtr::stop_and_wait()
{
	m_bStartPlay = FALSE;
	m_iPlay = 0;
	if ( !is_loaded())
	{
		return;
	}
	if ( m_sound != NULL)
	{
		lock();
		m_sound->stop_and_wait();
		un_lock();
	}
}
#endif

void CSoundStreamPtr::set_repeat()
{	
	if ( !is_loaded())
	{
		return;
	}

	ATLASSERT( m_sound);
	if ( m_sound != NULL)
	{
		lock();
		if ( m_bFreezzed)
		{
			un_lock();
			return;
		}
		m_sound->setRepeat( m_bLoop);
		un_lock();
	}
}

BOOL CSoundStreamPtr::is_seekable()
{
	if ( !is_loaded())
	{
		return TRUE;
	}

	ATLASSERT( m_sound);
	if ( m_sound != NULL)
	{
		lock();
		if ( m_bFreezzed)
		{
			un_lock();
			return TRUE;
		}
		BOOL b = m_sound->isSeekable();
		un_lock();
		return b;
	}
	return FALSE;
}

DWORD CSoundStreamPtr::get_frequency()
{
	if ( !is_loaded())
	{
		return 0;
	}

	ATLASSERT( m_sound);
	if ( m_sound != NULL)
	{		
		ATLASSERT( m_sound->getFrequency() != 0);
		return m_sound->getFrequency();
	}
	return 0;
}

DWORD CSoundStreamPtr::get_length_in_ms()
{
	if ( !is_loaded())
	{
		return 0;
	}

	ATLASSERT( m_sound);
	if ( m_sound != NULL)
	{		
		ATLASSERT( m_sound->getFrequency() != 0);
		DWORD64 len_ = ((DWORD64) G_BYTES2MS * (DWORD64) m_sound->getLength()) / m_sound->getFrequency();
		return len_;
	}
	return 0;
}

DWORD CSoundStreamPtr::get_length()
{
	if ( !is_loaded())
	{
		return 0;
	}

	ATLASSERT( m_sound);
	if ( m_sound != NULL)
		return m_sound->getLength();
	return 0;
}

#if AUDIERE_BUILD_H
DWORD CSoundStreamPtr::get_size()
{
	if ( !is_loaded())
	{
		return 0;
	}

	ATLASSERT( m_sound);
	if ( m_sound != NULL)
		return m_sound->getSize();
	return 0;
}
#endif

#if SOUND_MAN_RELEASE_H
DWORD CSoundStreamPtr::get_current_size()
{
	if ( !is_loaded())
	{
		return 0;
	}

	ATLASSERT( m_sound);
	ATLASSERT( m_pResFile);
	if ( m_sound != NULL && m_pResFile != NULL)
	{
		int dataSize = m_pResFile->getTotalReadBytes() - m_sound->getHeaderSize();
		return dataSize > 0 ? dataSize : 0;
	}
	return 0;
}
#endif


DWORD CSoundStreamPtr::get_buffer_size()
{
	if ( !is_loaded())
	{
		return 0;
	}

	ATLASSERT( m_sound);
	if ( m_sound != NULL)
		return 0;//sound->GetBufferSize();
	return 0;
}

LPSTR CSoundStreamPtr::get_buffer( DWORD *size)
{
	if ( !is_loaded())
	{
		*size = 0;
		return NULL;
	}
	ATLASSERT( m_sound);
	if ( m_sound != NULL)
		return 0;//sound->GetBuffer( size);
	*size = 0;
	return NULL;
}

#if AUDIERE_BUILD_H
int CSoundStreamPtr::update_file_info( int loaded_bytes, bool bLoaded)
{	
	lock();
	if ( !is_loading() && !is_loaded())
	{
		un_lock();
		// ресурс звука не загружен и процесс загрузки ресурса не идет
		return ERROR_RES_IS_NOT_OPENED;
	}
	
	if ( !m_pResFile)
	{
		un_lock();
		return ERROR_RES_INVALID;
	}

	CMemFile *pMemFile = (CMemFile *) m_pResFile;
	
	if ( bLoaded)
	{		
		if ( pMemFile->IsAttached() && !pMemFile->CopyFilePtr())
		{
			un_lock();
			return RMML_ERROR_FILE_INVALID;
		}		
		set_repeat();
	}

	if ( is_loading() || !is_loaded())
	{
		un_lock();
		// идет процесс загрузки ресурса
		return bLoaded ? WARNING_RES_IS_OPENING : WARNING_RES_IS_NOT_UPDATED;
	}	
	
	ATLASSERT( m_sound);
	if ( !m_sound)
	{
		un_lock();
		return ERROR_FILE_OPENING;	
	}

	int iResult = m_sound->updateFileInfo( loaded_bytes, bLoaded);
	un_lock();
	if ( iResult == OMS_OK && m_iPlay && !is_playing())
	{
		// попытаться снова запустить проигрывание звука с позиции, на которой был достигнут конец файла 
		// до текущего обновления данного файла
		m_bStopped = FALSE;
		m_sound->resume_play();
	}

	if ( iResult == -1)	
		return WARNING_RES_IS_NOT_UPDATED;

	if ( iResult == -2)
		return WARNING_RES_IS_NOT_NEED_TO_UPDATE;

	if ( iResult > 0)
		return ERROR_FILE_STREAM_READ;

	BOOL bSucc = pMemFile->UpdateState();
	if ( !bSucc)
		return RMML_ERROR_FILE_INVALID;
	return iResult;
}
#endif

#if AUDIERE_RELEASE_P
BOOL CSoundStreamPtr::is_res_attached()
{
	if ( m_pResFile)
	{
		return ((CMemFile *) m_pResFile)->IsAttached();
	}
	return FALSE;
}

ifile *CSoundStreamPtr::get_file()
{
	if ( m_pResFile)
	{
		return ((CMemFile *) m_pResFile)->GetFile();
	}
	return NULL;
}

void CSoundStreamPtr::freeze()
{
	if ( !is_loaded())
	{
		return;
	}

	m_pSndFileLoaderPlayer->releaseBuffer();
}

void CSoundStreamPtr::unfreeze()
{
	if ( !is_loaded())
	{
		return;
	}

	m_pSndFileLoaderPlayer->createBuffer();
}

BOOL CSoundStreamPtr::is_freeze()
{
	return m_bFreezzed;
}

void CSoundStreamPtr::create_buffer()
{
	lock();
	if ( m_sound && m_bFreezzed)
	{
		m_sound->createBuffer();
	}
	m_bFreezzed = FALSE;
	un_lock();

	set_pos(m_dwPosition);
	set_volume(m_fVolume);
	set_pan(m_fPanVolume);
	set_repeat();
}

void CSoundStreamPtr::release_buffer()
{
	if ( m_sound)
	{
		m_dwPosition = get_pos( 0);
		m_fVolume = get_volume();
		m_fPanVolume = get_pan();
		if ( is_play())
		{
			stop();
		}

		lock();
		m_sound->clear( false);
		m_bFreezzed = TRUE;
		un_lock();
	}
}

void CSoundStreamPtr::prepare_for_delete()
{
	m_pSndFileLoaderPlayer->releaseSound();
}

BOOL CSoundStreamPtr::is_prepare_for_delete()
{
	return m_bASndRemoved;
}

void CSoundStreamPtr::release_sound()
{
	lock();
	m_bLoadedFile = FALSE;
	m_bLoading = FALSE;
	if ( m_sound)
	{
		m_sound.DestroyPtr();
		m_sound = NULL;
	}
	m_bASndRemoved = TRUE;
	un_lock();
}
#endif

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CSoundEffectPtr::CSoundEffectPtr( )
{

}

CSoundEffectPtr::~CSoundEffectPtr()
{
	close();
}

BOOL CSoundEffectPtr::open( AudioDevicePtr device, File *resFile, const wchar_t *fileName)
{	
	set_name( fileName);	
	m_sound = audiere::OpenSoundEffect( device, resFile, SINGLE);
	if ( m_sound != NULL)
		m_bLoadedFile = TRUE;
	return m_sound != NULL;
}

void CSoundEffectPtr::close()
{
	clear();
}

//////////////////////////////////////////////////////////////////////////

/*BOOL CSoundEffectPtr::is_play()
{
	if ( m_sound != NULL)
		return m_iPlay;
	return FALSE;
}

BOOL CSoundEffectPtr::is_playing()
{
	//if ( sound != NULL)
	//	return sound->isPlaying();
	return FALSE;
}*/

BOOL CSoundEffectPtr::is_play()
{
	if ( !is_loaded())
		return FALSE;
	if ( m_sound != NULL)
	{		
		if ( m_iPlay)
			m_bStartPlay = FALSE;
		if ( m_bStartPlay)
			return TRUE;		
		return m_iPlay;
	}
	return FALSE;
}

BOOL CSoundEffectPtr::is_playing()
{
	if ( !is_loaded())
		return FALSE;
	if ( m_sound != NULL)
		return m_iPlay;
	return FALSE;
}

DWORD CSoundEffectPtr::get_buffer_size()
{
	if ( !is_loaded())
		return 0;
	ATLASSERT( m_sound);
	if ( m_sound != NULL)
		return 0;//sound->GetBufferSize();
	return 0;
}

LPSTR CSoundEffectPtr::get_buffer( DWORD *size)
{
	if ( !is_loaded())
	{
		*size = 0;
		return NULL;
	}
	ATLASSERT( m_sound);
	if ( m_sound != NULL)
		return 0;//sound->GetBuffer( size);
	*size = 0;
	return NULL;
}

void CSoundEffectPtr::set_pitch_shift( float shift)
{
	if ( m_sound != NULL)
	{		
		m_sound->setPitchShift( shift);
	}
}

float CSoundEffectPtr::get_pitch_shift( float shift)
{
	if ( m_sound != NULL)
		return m_sound->getPitchShift();
	return -1.0f;
}

#if AUDIERE_BUILD_H
int CSoundEffectPtr::update_file_info( int loaded_bytes, bool bLoaded)
{
	if ( !is_loading() && !is_loaded())
		return -1;
	ATLASSERT( m_sound);
	if ( !m_sound)
		return -1;
	return 0;
}
#endif

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "SoundManager.h"

struct SoundRemoveFunctionParams
{
	ISoundPtrBase *soundPtr;
	CSoundManager *soundMan;
};

// Threads
DWORD WINAPI remove_sound_function(void * objPtr)
{
	ATLASSERT( objPtr);
	SoundRemoveFunctionParams *tsoundPtr = (SoundRemoveFunctionParams *)objPtr;
	tsoundPtr->soundMan->iSndRemoving++;
	tsoundPtr->soundPtr->release();	
	tsoundPtr->soundMan->iSndRemoving--;
	MP_DELETE(tsoundPtr);
	return 0;
}


CSoundPtrMap::CSoundPtrMap()
{

}

CSoundPtrMap::~CSoundPtrMap()
{

}

//////////////////////////////////////////////////////////////////////////

BOOL CSoundPtrMap::remove( int ID, CSoundManager *pSoundManager, BOOL bAsync, BOOL bRemove /* = FALSE */)
{
	mutex.lock();
	if ( has_object( ID)) 
	{
		if ( bRemove)
		{
			ISoundPtrBase *soundPtr = objMap[ ID];
			if ( bAsync)
			{
				SoundRemoveFunctionParams *pSound = MP_NEW(SoundRemoveFunctionParams);
				pSound->soundMan = pSoundManager;
				pSound->soundPtr = soundPtr;
				create( remove_sound_function, pSound);
			}
			else
			{
				soundPtr->release();
				soundPtr = NULL;
			}
		}
		objMap.erase( ID);
		mutex.unlock();
		return TRUE;
	} 
	mutex.unlock();
	return FALSE;
}

BOOL CSoundPtrMap::remove( ISoundPtrBase *soundPtr, CSoundManager *pSoundManager, BOOL bAsync, BOOL bRemove /* = FALSE */)
{
	if ( soundPtr == NULL)
		return FALSE;
	int id = soundPtr->get_id();
	mutex.lock();
	if ( has_object( id)) 
	{
		if ( bRemove)
		{	
			if ( bAsync)
			{
				SoundRemoveFunctionParams *pSound = MP_NEW(SoundRemoveFunctionParams);
				pSound->soundMan = pSoundManager;
				pSound->soundPtr = soundPtr;
				create( remove_sound_function, pSound);
			}
			else
			{
				soundPtr->release();
				soundPtr = NULL;
			}
		}
		objMap.erase( id);
		mutex.unlock();
		return TRUE;
	}
	mutex.unlock();
	return FALSE;
}

int CSoundPtrMap::get_new_id()
{	
	mutex.lock();
	objectMap::iterator iter = objMap.begin(), end = objMap.end();
	int max_id = -1;
	for ( ; iter != end; ++iter)
	{
		int id = iter->first;
		if ( max_id < id)
			max_id = id;
	}
	mutex.unlock();
	return max_id + 1;
}

void CSoundPtrMap::remove_all_objects()
{	
	objectMap::iterator iter = objMap.begin(), end = objMap.end();
	
	BOOL bAllStopped = FALSE;
	int count = 0;
	while (!bAllStopped)
	{
		bAllStopped = TRUE;
		iter = objMap.begin();
		for ( ; iter != end; ++iter)
		{
			ISoundPtrBase *soundPtr = iter->second;
			if ( soundPtr != NULL)
			{
				if ( soundPtr->is_play())
				{
					soundPtr->stop();
				}
				if ( !soundPtr->is_stopped())
				{
					bAllStopped = FALSE;
					count++;
					Sleep( 50);
					break;
				}
			}
		}
		if ( count > 40)
			break;
	}

	iter = objMap.begin();

	for ( ; iter != end; ++iter)
	{
		ISoundPtrBase *soundPtr = iter->second;
		//SAFE_DELETE( soundPtr);
		soundPtr->release();
		soundPtr = NULL;
	}	
	objMap.clear();	
}

void CSoundPtrMap::remove_all_objects( CSoundManager *pSound)
{
	objectMap::iterator iter = objMap.begin(), end = objMap.end();

	BOOL bAllStopped = FALSE;
	int count = 0;
	while (!bAllStopped)
	{
		bAllStopped = TRUE;
		iter = objMap.begin();
		for ( ; iter != end; ++iter)
		{
			ISoundPtrBase *soundPtr = iter->second;
			if ( soundPtr != NULL)
			{
				if ( soundPtr->is_play())
				{
					soundPtr->stop();
				}
				if ( !soundPtr->is_stopped())
				{
					bAllStopped = FALSE;					
					count++;
					Sleep( 50);
					break;
				}
			}
		}
		if ( count > 40)
			break;
	}

	iter = objMap.begin();

	for ( ; iter != end; ++iter)
	{
		ISoundPtrBase *soundPtr = iter->second;
		if ( soundPtr != NULL)
		{
			if ( soundPtr->is_play())
			{
				soundPtr->stop();
			}
			//pSound->removeThreadBySoundPtr( soundPtr);
			//MP_DELETE( soundPtr);
			soundPtr->release();
			soundPtr = NULL;
		}
	}	
	objMap.clear();
}



BOOL CSoundPtrMap::update_all_sound( int act, CSoundManager *pSound)
{
	if ( act == SOUNDS_DISABLED)
	{
		set_all_sounds_disabled();		
		return TRUE;
	}
	
	if ( act == SOUNDS_ENABLED)
	{
		set_all_sounds_enabled();
		return TRUE;
	}

	if ( act == SOUNDS_STOP)
	{
		stop_all_sounds();
		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

void CSoundPtrMap::set_all_sounds_disabled()
{
	mutex.lock();
	objectMap::iterator iter = objMap.begin(), end = objMap.end();
	for ( ; iter != end; ++iter)
	{
		ISoundPtrBase *soundPtr =  iter->second;
		if ( soundPtr != NULL)
		{
			soundPtr->disabled();
		}
	}
	mutex.unlock();
}

void CSoundPtrMap::set_all_sounds_enabled()
{
	mutex.lock();
	objectMap::iterator iter = objMap.begin(), end = objMap.end();
	for ( ; iter != end; ++iter)
	{
		ISoundPtrBase *soundPtr =  iter->second;
		if ( soundPtr != NULL)
		{
			soundPtr->enabled();
		}
	}
	mutex.unlock();
}

void CSoundPtrMap::stop_all_sounds()
{
	mutex.lock();
	objectMap::iterator iter = objMap.begin(), end = objMap.end();
	for ( ; iter != end; ++iter)
	{
		ISoundPtrBase *soundPtr = iter->second;
		if ( soundPtr != NULL)
		{
			if ( soundPtr->is_play())
			{
				soundPtr->stop();
			}
		}
	}
	mutex.unlock();
}