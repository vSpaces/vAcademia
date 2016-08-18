// SoundManager.cpp: implementation of the CSoundManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../mdump/mdump.h"
#include "SoundManager.h"
#include <fstream>
#include <assert.h>
#include "SoundPtr.h"
#include "rm/rm.h"
#include "ifile.h"
#include "ThreadAffinity.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSoundManager::CSoundManager( omsContext* aContext)
{
	#ifdef _DEBUG
		//_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		//_CrtSetBreakAlloc(63);
	#endif

	InitSecondaryThreadAffinityRoutine();

	m_pContext = aContext;

	prevTime = 0;
	IndexThread = 0;
	BSoundThread  = TRUE;
	BPausa = FALSE;
		
	// Формат звуковых данных		 
	PositionPlaySoundThread = -1;
	num_avatar = -1;
	playingState = SOUNDS_ENABLED;
	device = NULL;
	bDeviceChanging = FALSE;

	iSndRemoving = 0;
}

CSoundManager::~CSoundManager()
{ 	
	//terminating();
	ATLTRACE("~CSoundManager\n");
	playingState = SOUNDS_ENABLED;	
	soundPtrMap.stop_all_sounds();
	soundPtrMap.remove_all_objects();
	device = NULL;
}

//////////////////////////////////////////////////////////////////////////

int CSoundManager::OpenAudiereOutput()
{ 
	//device = OpenDevice( 0, "global=false,buffer=4096,min_buffer_size=1024");
	device = OpenDevice( 0, "global=true,buffer=4096,min_buffer_size=1024");

	return TRUE;
}

//---------------------------------------------------------
// 		  SoundArray[].play  = 0; - silence
// 		  SoundArray[].play  = 1; - playing
// 		  SoundArray[].play  = 2; - freeze

omsresult CSoundManager::setResManager( IResMan *aResMan)
{	
	setResMan( aResMan);
	OpenAudiereOutput();
	return S_OK;
}

// smISoundManager
omsresult CSoundManager::CreateMO( mlMedia* apMLMedia)
{
	return m_pContext->mpRM->CreateMO( apMLMedia);
}

omsresult CSoundManager::DestroyMO( mlMedia* apMLMedia)
{
	return m_pContext->mpRM->DestroyMO( apMLMedia);
}


//////////////////////////////////////////////////////////////////////////

BOOL CSoundManager::LoadSoundFile( ISoundPtrBase *soundPtr, LPCWSTR name, ifile *pfile, BOOL bImmediate, BOOL bAttached /* = FALSE */)
{
	if ( mpResMan != NULL)
	{				
		MP_NEW_P2( res_file, CMemFile, name, pfile);
		int resID = res_file->Init( soundPtr, device, bAttached);
		if ( resID == 1)
		{
			if ( bImmediate)
			{
				if ( !soundPtr->open( device, res_file, name))
				{
					return FALSE;
				}
			}
			else
			{
				soundPtr->params_saver();
				if ( !soundPtr->open_async( device, res_file, name))
				{
					return FALSE;
				}
			}
			return TRUE;
		}
		else if ( resID == 2)		
			return TRUE;		
	}
	return FALSE;
}

BOOL CSoundManager::LoadSound( ISoundPtrBase *soundPtr, LPCWSTR name, res::IResource* res, BOOL bImmediate)
{	
	if ( mpResMan != NULL)
	{
		MP_NEW_P3( res_file, CResourceFile, mpResMan, name, res);
		int resID = res_file->Init( soundPtr, device);
		if ( resID == 1)
		{
			if ( bImmediate)
			{
				if ( !soundPtr->open( device, res_file, name))
				{
					return FALSE;
				}
			}
			else
			{
				soundPtr->params_saver();
				if ( !soundPtr->open_async( device, res_file, name))
				{
					return FALSE;
				}
			}
			return TRUE;
		}
		else if ( resID == 2)		
			return TRUE;		
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

omsresult CSoundManager::setCooperativeLevel( UINT hWnd, UINT level)
{
	//if ( direct_sound == NULL)
		//return S_FALSE;
	return S_FALSE;
	// direct_sound->SetCooperativeLevel( (HWND) hWnd, (DWORD)level);
}

omsresult CSoundManager::openSoundFile( ifile* pfile, BOOL bImmediate, sm::smISoundNotify *apISoundNotify /* = NULL */, BOOL bAttached /* = FALSE */)
{	
	int iSoundIndex = soundPtrMap.get_new_id();
	ISoundPtrBase *soundPtr = MP_NEW( CSoundStreamPtr);
	if ( apISoundNotify)
		soundPtr->set_notify( apISoundNotify);
	//ISoundPtrBase *soundPtr = MP_NEW(CSoundEffectPtr);

	soundPtr->set_id( iSoundIndex);
	soundPtr->clear();
	LPCWSTR name = pfile->get_file_path();

	if ( bImmediate)
	{
		if ( !LoadSoundFile( soundPtr, name, pfile, FALSE, bAttached))// || !soundPtr->is_loaded())  
			return -1;	
	}
	else
		soundPtr->set_name( name);
	soundPtrMap.insert( iSoundIndex, soundPtr);

	return iSoundIndex;
}

omsresult CSoundManager::openSoundFile( int aiSoundIndex, ifile* pfile, BOOL bImmediate, sm::smISoundNotify *apISoundNotify /* = NULL */, BOOL bAttached /* = FALSE */)
{	
	int iSoundIndex = aiSoundIndex;//soundPtrMap.get_new_id();
	ISoundPtrBase *soundPtr = MP_NEW( CSoundStreamPtr);
	if ( apISoundNotify)
		soundPtr->set_notify( apISoundNotify);
	//ISoundPtrBase *soundPtr = MP_NEW(CSoundEffectPtr);

	soundPtr->set_id( iSoundIndex);
	soundPtr->clear();
	LPCWSTR name = pfile->get_file_path();

	if ( bImmediate)
	{
		if ( !LoadSoundFile( soundPtr, name, pfile, FALSE, bAttached))// || !soundPtr->is_loaded())  
			return -1;	
	}
	else
		soundPtr->set_name( name);
	soundPtrMap.insert( iSoundIndex, soundPtr);

	return iSoundIndex;
}

omsresult CSoundManager::openSound( LPCWSTR name, IResource* res, BOOL bImmediate, sm::smISoundNotify *apISoundNotify)
{
	int iSoundIndex = soundPtrMap.get_new_id();	
	ISoundPtrBase *soundPtr = MP_NEW( CSoundStreamPtr);
	if ( apISoundNotify)
		soundPtr->set_notify( apISoundNotify);
	//ISoundPtrBase *soundPtr = MP_NEW( CSoundEffectPtr);	

	soundPtr->set_id( iSoundIndex);
	soundPtr->clear();
	if ( bImmediate)
	{
		if ( !LoadSound( soundPtr, name, res, FALSE))// || !soundPtr->is_loaded())  
			return -1;	
	}
	else
		soundPtr->set_name( name);
	soundPtrMap.insert( iSoundIndex, soundPtr);

	return iSoundIndex;
}

omsresult CSoundManager::OpenSoundFile( ifile *pfile, BOOL immediate, int *id, sm::smISoundNotify *apISoundNotify /* = NULL */, bool bAttached /* = FALSE */)
{
	CWaitAndSignal waitAndSignal( mutexOpenClose);
	if ( !device)
	{
		return SNDM_E_RESMANNOTSET;
	}
	*id = openSoundFile( pfile, immediate, apISoundNotify, bAttached);
	if ( *id == -1) 
	{
		return SNDM_E_RESMANNOTSET;
	}
	ATLTRACE("\nOpenSound id=%d with name = %s\n \n", *id, pfile->get_file_path());
	return S_OK;
}

omsresult CSoundManager::OpenSoundRes( IResource* res, LPCWSTR name, BOOL immediate, int *id, sm::smISoundNotify *apISoundNotify)
{
	CWaitAndSignal waitAndSignal( mutexOpenClose);
	if ( !device)
	{
		return SNDM_E_RESMANNOTSET;
	}
	*id = openSound( name, res, immediate, apISoundNotify);
	if ( *id == -1) 
	{
		return SNDM_E_RESMANNOTSET;
	}
	ATLTRACE("\nOpenSound id=%d with name = %s\n \n",*id,name);
	return S_OK;
}

omsresult CSoundManager::playSound( int index, int num_person, BOOL loop)
{
	CWaitAndSignal waitAndSignal( mutex);
	ATLASSERT( index > -1);
	//make_dump( index > -1);
	if ( index < 0)
		return 0;
	interval = 0;

	// Lets find sound 
	ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( index);
	if ( soundPtr == NULL)
		return -1;

	soundPtr->lock();
	if ( soundPtr->is_playing())
	{
		soundPtr->un_lock();
		return soundPtr->get_id();
	}
	
	if ( !soundPtr->is_loading())
	{
		if ( !soundPtr->is_loaded())
		{ 	
			soundPtr->un_lock();
			if ( !LoadSound( soundPtr, soundPtr->get_name(), NULL, FALSE))
				return -1;		
		}
		else
			soundPtr->un_lock();
	}
	else
		soundPtr->un_lock();
	
	soundPtr->lock();
	soundPtr->set_loop( loop);
	soundPtr->set_play( 1);		

	if ( soundPtr->is_loading())
	{
		soundPtr->un_lock();
		return soundPtr->get_id();
	}

	BOOL bLoaded = soundPtr->is_loaded();
	soundPtr->un_lock();

	if ( bLoaded)
	{
		soundPtr->play();
		soundPtr->set_repeat();		
	}
	//soundPtr->set_repeat();
	//soundPtr->play();
		
	return soundPtr->get_id();
	
}

//--------------------------------------------------------
omsresult CSoundManager::stopSound( int index)
{	
	CWaitAndSignal waitAndSignal( mutex);
	ATLASSERT( index > -1);
	//make_dump( index > -1);
	if ( index < 0)
		return 0;
	ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( index);
	if ( soundPtr == NULL)
		return -1;
		
	soundPtr->stop();

	return FALSE;
}

BOOL CSoundManager::closeSound( int id)
{
	CWaitAndSignal waitAndSignal( mutexOpenClose);
	ATLASSERT( id > -1);
	//make_dump( id > -1);
	if ( id < 0)
		return 0;
	stopSound( id);
	ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( id); //&SoundArray[ id];
	if ( soundPtr != NULL)
		ATLTRACE(_T("\nCloseSound N %d name=%s\n"), id, soundPtr->get_name());
	else
		return FALSE;
	soundPtr->lock();
	soundPtr->set_loading( FALSE);
	soundPtr->un_lock();
	// Let's stop sound and close Resource file
	ATLASSERT( soundPtr->get_id() == id);
	if ( soundPtr->get_id() != id)
		DeleteElementInSoundArray( id);
	else
		removeSound( soundPtr);
	return TRUE;	
}

void CSoundManager::removeSound( ISoundPtrBase *soundPtr)
{
	if ( soundPtr != NULL)
	{
		if ( soundPtr->is_playing() || soundPtr->is_play() || !soundPtr->is_stopped())
		{
			if ( soundPtr->is_playing() || soundPtr->is_play())
			{
				soundPtr->stop();
			}
			/*int count = 0;
			while( true)
			{
				if ( soundPtr->is_stopped())
					break;
				Sleep( 10);
				if ( count > 50)
					break;
				count++;
			}*/
		}

		soundPtrMap.remove( soundPtr, this, TRUE, TRUE);
	}	
}

//////////////////////////////////////////////////////////////////////////

BOOL CSoundManager::IsPlaying( int index)
{
	CWaitAndSignal waitAndSignal( mutex);
	ATLASSERT( index > -1);	
	if ( index < 0)
		return 0;
	ISoundPtrBase *sound_ptr = soundPtrMap.get_obj_by_id( index);
	if ( sound_ptr == NULL)
		return FALSE;
	if ( sound_ptr->is_play() == 1 || sound_ptr->is_play() == 3) 
		return TRUE;
	return FALSE;
}

BOOL CSoundManager::IsLoaded( int index)
{
	CWaitAndSignal waitAndSignal( mutex);
	ATLASSERT( index > -1);	
	if ( index < 0)
		return 0;
	ISoundPtrBase *sound_ptr = soundPtrMap.get_obj_by_id( index);
	if ( sound_ptr == NULL)
		return FALSE;
	return sound_ptr->is_loaded();	
}

DWORD CSoundManager::GetPosition( int index)
{  
	CWaitAndSignal waitAndSignal( mutex);
	ATLASSERT( index > -1);	
	if ( index < 0)
		return 0;
	ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( index);
	if ( soundPtr == NULL)
		return 0;
	return soundPtr->get_pos( 0);	    
}

omsresult CSoundManager::GetPosition( int id, DWORD *time)
{
	*time = GetPosition( id);
	return S_OK;
}

DWORD CSoundManager::GetFrequency( int index)
{  
	CWaitAndSignal waitAndSignal( mutex);
	ATLASSERT( index > -1);	
	if ( index < 0)
		return 0;
	ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( index);
	if ( soundPtr == NULL)
		return 0;
	return soundPtr->get_frequency();  
}

omsresult CSoundManager::GetFrequency( int index, DWORD *time)
{
	*time = GetFrequency( index);
	return S_OK;
}

DWORD CSoundManager::GetDuration( int index)
{  
	CWaitAndSignal waitAndSignal( mutex);
	ATLASSERT( index > -1);	
	if ( index < 0)
		return 0;
	ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( index);
	if ( soundPtr == NULL)
		return 0;
	return soundPtr->get_length_in_ms();  
}

omsresult CSoundManager::GetDuration( int index, DWORD *time)
{
	*time = GetDuration( index);
	return S_OK;
}

DWORD CSoundManager::GetLength( int index)
{  
	CWaitAndSignal waitAndSignal( mutex);
	ATLASSERT( index > -1);	
	if ( index < 0)
		return 0;
	ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( index);
	if ( soundPtr == NULL)
		return 0;
	return soundPtr->get_length();  
}

omsresult CSoundManager::GetLength( int index, DWORD *length)
{
	*length = GetLength( index);
	return S_OK;
}

DWORD CSoundManager::GetSize( int index)
{  
	CWaitAndSignal waitAndSignal( mutex);
	ATLASSERT( index > -1);
	if ( index < 0)
		return 0;
	ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( index);
	if ( soundPtr == NULL)
		return 0;
	return soundPtr->get_size();  
}

omsresult CSoundManager::GetSize( int index, DWORD *size)
{
	*size = GetSize( index);
	return S_OK;
}

DWORD CSoundManager::GetCurrentSize( int index)
{  
	CWaitAndSignal waitAndSignal( mutex);
	ATLASSERT( index > -1);	
	if ( index < 0)
		return 0;
	ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( index);
	if ( soundPtr == NULL)
		return 0;
	return soundPtr->get_current_size();  
}

omsresult CSoundManager::GetCurrentSize( int index, DWORD *size)
{
	*size = GetCurrentSize( index);
	return S_OK;
}

omsresult CSoundManager::GetVolume( int id, float *volume)
{
	CWaitAndSignal waitAndSignal( mutex);
	ATLASSERT( id > -1);	
	if ( id < 0)
		return FALSE;
	ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( id);
	if ( soundPtr != NULL)// || !soundPtr->is_enabled())
	{
		*volume = soundPtr->get_volume();
		return S_OK;
	}
	return S_FALSE;
}

BOOL CSoundManager::SetPosition( int index, DWORD time)
{	
	CWaitAndSignal waitAndSignal( mutexPosition);
	if ( playingState == SOUNDS_DISABLED)
		return true;
	ATLASSERT( index > -1);	
	if ( index < 0)
		return FALSE;

	ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( index);
	if ( soundPtr == NULL)
		return FALSE;
	if ( soundPtr->is_loaded())
	{
		soundPtr->set_pos( time); 		
		return TRUE;
	}
	return FALSE;
}

void CSoundManager::SetVolume( float volume)
{
	//CWaitAndSignal waitAndSignal( mutex);
	if ( playingState == SOUNDS_DISABLED)
		return;

	/*for ( int i = 0; i < MAX_SOUNDS; i++)
	{ 
		CPlaySoundThread *plSoundThread = &PlaySoundThreads[ i];
		if ( plSoundThread->get_handle() != NULL && plSoundThread->is_resume()) 
		{			
			ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( plSoundThread->get_sound_id());
			if ( soundPtr == NULL || !soundPtr->is_enabled())
				return;
			soundPtr->set_volume( volume);			
		}
	}*/	

	/*if ( volume == 0.5f)
	{
		while( true)
		{
			int x = 0;
		}
	}*/
}

oms::omsresult CSoundManager::SetVolume( double adVol)
{
	/*CWaitAndSignal waitAndSignal( mutex);
	if ( playingState == SOUNDS_DISABLED)
		return OMS_ERROR_FAILURE;
		
	if ( adVol == 0.5)
	{
		while( true)
		{
			int x = 0;
		}
	}
	*/
	return OMS_ERROR_NOT_IMPLEMENTED;
}

BOOL CSoundManager::Set3DSoundCoef( int id, float coef, float left_coef, float right_coef)
{	
	C3DSoundFactor t3DSoundFactor;
	t3DSoundFactor._attenutedCoef = coef;
	t3DSoundFactor._leftAttenutedCoef = left_coef;
	t3DSoundFactor._rightAttenutedCoef = right_coef;
	return Set3DSoundParams( id, &t3DSoundFactor);	
}

BOOL CSoundManager::Set3DSoundParams( int id, float distX, float distY, float distZ, float dirX, float dirY, float dirZ,
							  float vp_dirX, float vp_dirY, float vp_dirZ)
{
	CWaitAndSignal waitAndSignal( mutex);
	if ( playingState == SOUNDS_DISABLED)
		return FALSE;
	ATLASSERT( id > -1);	
	if ( id < 0)
		return FALSE;

	ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( id);
	if ( soundPtr != NULL)// || !soundPtr->is_enabled())
	{
		CalVector dist = CalVector( distX, distY, distZ);
		float attenutedCoef = listener.getVolumeFromDistance( t3dListenerSoundParams._attenutedCoef, t3dListenerSoundParams._minDist, 
			t3dListenerSoundParams._maxDist, t3dListenerSoundParams._attenunedDistCoef, dist.length());

		if ( attenutedCoef == 0.0f)
		{
			soundPtr->set_volume( attenutedCoef);
			return TRUE;
		}

		listener.setDirect( CalVector( vp_dirX, vp_dirY, vp_dirZ));
		// угол между вектором взгляда (ориентации) аватара и вектор, начало которого данный аватар (слушатель), а конец другой аватар av
		float angle = listener.getAngleOnHorinz( dist);
		// угол между ориентациями двух аватаров
		float angleBetweenDir = listener.getAngleBetweenDirects( CalVector( dirX, dirY, dirZ));	
		// коэффициент падения уровня громкости звука
		float leftAttenutedCoef = t3dListenerSoundParams._leftAttenutedCoef;
		// коэффициент падения уровня громкости звука
		float rightAttenutedCoef = t3dListenerSoundParams._rightAttenutedCoef;

		float angle_coef = t3dListenerSoundParams._angle_coef;

		// угол
		if ( angle > 0.0f && angle <= PI / 2)
		{
			leftAttenutedCoef *= ( 1.0f - 2 * angle / PI);
			angle_coef += 0.5 * angle / PI;
			if ( angleBetweenDir > 0.0f && angleBetweenDir <= PI / 2)
				attenutedCoef *= t3dListenerSoundParams._avDir2youCoef;
		}
		else if ( angle > PI / 2 && angle < PI)
		{
			attenutedCoef *= 0.7f;
			leftAttenutedCoef *= ( 2 * angle / PI - 1.0f);
			angle_coef += 0.5 * ( 0.5f - angle / PI);
			if ( angleBetweenDir > PI / 2 && angleBetweenDir <= PI)
				attenutedCoef *= t3dListenerSoundParams._avDir2youCoef;
		}
		else if ( angle < 0.0f && angle >= -PI / 2)
		{
			rightAttenutedCoef *= ( 1.0f + 2 * angle / PI);
			angle_coef -= 0.5 * angle / PI;
			if ( angleBetweenDir > 0.0f && angleBetweenDir <= PI / 2)
				attenutedCoef *= t3dListenerSoundParams._avDir2youCoef;
		}
		else if ( angle < -PI / 2 && angle > -PI)
		{
			attenutedCoef *= 0.7f;
			rightAttenutedCoef *= ( -2 * angle / PI - 1.0f);
			angle_coef -= 0.5 * ( 0.5f + angle / PI);
			if ( angleBetweenDir > PI / 2 && angleBetweenDir <= PI)
				attenutedCoef *= t3dListenerSoundParams._avDir2youCoef;
		}

		//leftAttenutedCoef *= leftAttenutedCoef;
		//rightAttenutedCoef *= rightAttenutedCoef;		

#pragma message ("WARNING!!!!!!!!! Experimental part leftAttenutedCoef and rightAttenutedCoef")
		/*if ( leftAttenutedCoef < 1.0f)
		leftAttenutedCoef *= angle_coef;
		else if ( rightAttenutedCoef < 1.0f)
		rightAttenutedCoef *= angle_coef;*/

		if ( leftAttenutedCoef < 1.0f)
		{
			soundPtr->set_pan( 1.0f - leftAttenutedCoef);
		}
		else if ( rightAttenutedCoef < 1.0f)
		{
			soundPtr->set_pan( -1.0f + rightAttenutedCoef);
		}
		soundPtr->set_volume( attenutedCoef);
	}
	return TRUE;
}


BOOL CSoundManager::Set3DSoundParams( int id, C3DSoundFactor *p3DSoundFactor)
{
	CWaitAndSignal waitAndSignal( mutex);
	if ( playingState == SOUNDS_DISABLED)
		return FALSE;
	ATLASSERT( id > -1);	
	if ( id < 0)
		return FALSE;

	ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( id);
	if ( soundPtr != NULL)// || !soundPtr->is_enabled())
	{
		// коэффициент падения уровня громкости звука
		float attenutedCoef = p3DSoundFactor->_attenutedCoef;

		if ( attenutedCoef < 0.0000001f)
		{
			soundPtr->set_volume( 0.0f);
			return TRUE;
		}
		
		// коэффициент падения уровня громкости звука левого канала
		float leftAttenutedCoef = p3DSoundFactor->_leftAttenutedCoef;
		// коэффициент падения уровня громкости звука правого канала
		float rightAttenutedCoef = p3DSoundFactor->_rightAttenutedCoef;

		if ( leftAttenutedCoef < 1.0f)
		{
			soundPtr->set_pan( 1.0f - leftAttenutedCoef);
		}
		else if ( rightAttenutedCoef < 1.0f)
		{
			soundPtr->set_pan( -1.0f + rightAttenutedCoef);
		}
		soundPtr->set_volume( attenutedCoef);
	}
	return TRUE;
}

BOOL CSoundManager::SetSoundType( int id, int type)
{
	if ( playingState == SOUNDS_DISABLED)
		return FALSE;
	return TRUE;
}

BOOL CSoundManager::SetVolume( int id, float volume)
{
	CWaitAndSignal waitAndSignal( mutex);
	if ( playingState == SOUNDS_DISABLED)
		return FALSE;
	ATLASSERT( id > -1);	
	if ( id < 0)
		return FALSE;
	ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( id);
	if ( soundPtr != NULL)// || !soundPtr->is_enabled())
	{
		soundPtr->set_volume( volume);
		return TRUE;
	}
	return FALSE;
}

BOOL CSoundManager::SetPitchShift( int id, float shift)
{
	CWaitAndSignal waitAndSignal( mutex);
	if ( playingState == SOUNDS_DISABLED)
		return FALSE;
	ATLASSERT( id > -1);	
	if ( id < 0)
		return FALSE;
	ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( id);
	if ( soundPtr != NULL || !soundPtr->is_enabled())
	{
		soundPtr->set_pitch_shift( shift);
		return TRUE;
	}
	return FALSE;
}

BOOL CSoundManager::UpdateAllSound( int act)
{	
	CWaitAndSignal waitAndSignal( mutex);
	playingState = act;
	if ( act == SOUNDS_ENABLED || act == SOUNDS_DISABLED)
		return true;
	return soundPtrMap.update_all_sound( act, this);
}

omsresult CSoundManager::UpdateFileInfo( int id, int loaded_bytes, bool bLoaded)
{
	CWaitAndSignal waitAndSignal( mutexUpdateData);
	ATLASSERT( id > -1);	
	if ( id < 0)
		return -1;
	ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( id);
	if ( soundPtr == NULL)
		return -1;
	return soundPtr->update_file_info( loaded_bytes, bLoaded);
}

/*omsresult CSoundManager::GetSoundOutputDevices(std::wstring& soundOutputDevices)
{
#if	AUDIERE_RELEASE_P
	bool bSucc = device->GetSoundOutputDevices(soundOutputDevices);
	return bSucc ? S_OK : S_FALSE;
#else
	return S_OK;
#endif
}*/

#define SIMPLE_DEVICE_CHANGE 1

#if SIMPLE_DEVICE_CHANGE
struct TSound
{
	int id;
	std::wstring sName;
	BOOL bLoading;
	BOOL bPlaying;
	float fVolume;
	DWORD dwPosition;
	DWORD dwPan;
	BOOL bLoop;
	BOOL bAttached;
	ifile *pfile;
	sm::smISoundNotify *pISoundNotify;
	~TSound()
	{
		if ( !bAttached)
		{
			pfile->release();
			pfile = NULL;
		}
	}
};
#endif

omsresult CSoundManager::ReplaceDevice(std::wstring &sDeviceName)
{
	if (m_pContext)
	if (m_pContext->mpLogWriter)
	{
		m_pContext->mpLogWriter->WriteLn("ReplaceDevice ", sDeviceName);
	}

#if	AUDIERE_RELEASE_P
	if ( !device)
	{
		return SNDM_E_RESMANNOTSET;
	}

	if ( !device->IsDeviceChangeNeed( sDeviceName))
	{
		return S_OK;
	}

	if (m_pContext)
	if (m_pContext->mpLogWriter)
	{
		m_pContext->mpLogWriter->WriteLn("ReplaceDevice ", sDeviceName, " need to change");
	}

	
	mutexOpenClose.lock();
	mutexPosition.lock();
	mutexUpdateData.lock();
	mutex.lock();

	bDeviceChanging = TRUE;

	device->Lock();

#if SIMPLE_DEVICE_CHANGE
	std::vector<TSound*> soundResNames;

	int size = soundPtrMap.get_size();
	int max_id = soundPtrMap.get_new_id();
	size = max_id;
	for ( int id = 0; id < size; id++)
	{
		ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( id);
		if ( soundPtr == NULL)
		{
			continue;
		}

		ATLASSERT( soundPtr->get_file() != NULL);
		if ( !soundPtr->get_file())
		{			
			continue;
		}
		
		TSound *pSound = MP_NEW( TSound);
		pSound->id = soundPtr->get_id();
		pSound->bLoading = soundPtr->is_loading();
		while( pSound->bLoading)
		{
			pSound->bLoading = soundPtr->is_loading();
			Sleep( 100);
		}
		pSound->sName = soundPtr->get_name();
		pSound->bPlaying = soundPtr->is_play();
		pSound->fVolume = soundPtr->get_volume();
		pSound->dwPosition = soundPtr->get_pos(0);
		pSound->dwPan = soundPtr->get_pan();
		pSound->bLoop = soundPtr->is_loop();
		pSound->bAttached = soundPtr->is_res_attached();		

		if ( pSound->bAttached)
		{
			pSound->pfile = soundPtr->get_file();
		}
		else
		{		
			pSound->pfile = soundPtr->get_file()->duplicate();
		}
		pSound->pfile->set_file_path( soundPtr->get_name());
		pSound->pISoundNotify = soundPtr->get_notify();
		if ( soundPtr->is_play())
		{
			soundPtr->stop_and_wait();
		}

		soundResNames.push_back( pSound);

		soundPtr->set_notify( NULL);
		soundPtr->prepare_for_delete();
	}

	BOOL bSndRemoved = FALSE;
	while ( !bSndRemoved)
	{
		bSndRemoved = TRUE;
		for ( int id = 0; id < size; id++)
		{
			ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( id);
			if ( soundPtr == NULL)
			{
				continue;
			}

			bSndRemoved = bSndRemoved && soundPtr->is_prepare_for_delete();
		}

		Sleep( 50);
	}
	
	for ( int id = 0; id < size; id++)
	{
		ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( id);
		if ( soundPtr == NULL)
		{
			continue;
		}
		soundPtrMap.remove( soundPtr, this, FALSE, TRUE);
	}

	soundPtrMap.remove_all_objects();

	/*while( iSndRemoving > 0)
	{
		// ждем когда все звуковые объекты удаляться
		Sleep( 100);
	}*/

	bool bSucc = device->ReplaceDevice( sDeviceName);//audiere::ReplaceDevice(device, sDeviceName);	

	if ( !bSucc)
	{		
		return S_FALSE;
	}
	
	std::vector<TSound*>::iterator it = soundResNames.begin(), end = soundResNames.end();
	for ( ; it != end; it++)
	{
		TSound *pSound = *it;
		openSoundFile( pSound->id, pSound->pfile, TRUE, pSound->bAttached ? pSound->pISoundNotify : NULL, pSound->bAttached);
	}

	size = soundPtrMap.get_size();
	max_id = soundPtrMap.get_new_id();
	size = max_id;
	BOOL bLoaded = FALSE;
	while ( !bLoaded)
	{
		bLoaded = TRUE;
		for ( int id = 0; id < size; id++)
		{
			ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( id);
			if ( soundPtr == NULL)
			{
				continue;
			}

			bLoaded = bLoaded && soundPtr->is_loaded();
		}
		Sleep( 100);
	}

	std::vector<TSound*>::iterator resIt = soundResNames.begin(), resEnd = soundResNames.end();
	for ( int id = 0; id < size; id++)
	{
		ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( id);
		if ( soundPtr == NULL)
		{
			continue;
		}

		ATLASSERT( resIt != resEnd);
		if ( resIt == resEnd)
		{
			break;
		}

		TSound *pSound = *resIt;

		ATLASSERT( pSound->id == soundPtr->get_id());

		if ( soundPtr->get_notify() == NULL)
		{			
			soundPtr->set_notify( pSound->pISoundNotify);
		}
		soundPtr->set_loop( pSound->bLoop);
		soundPtr->set_volume( pSound->fVolume);		
		soundPtr->set_pan( pSound->dwPan);
		soundPtr->set_repeat();
		soundPtr->set_pos( pSound->dwPosition);
		if ( pSound->bPlaying)
		{
			soundPtr->set_play( 1);
		}
		resIt++;
		MP_DELETE( pSound);
	}
	
	for ( int id = 0; id < size; id++)
	{
		ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( id);
		if ( soundPtr == NULL)
		{
			continue;
		}
		if ( soundPtr->is_play())
		{
			soundPtr->play();
		}
	}

	soundResNames.clear();

#else	

	std::vector<int> playingSoundIDs;
	int size = soundPtrMap.get_size();
	int max_id = soundPtrMap.get_new_id();
	size = max_id;
	for ( int id = 0; id < size; id++)
	{
		ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( id);
		if ( soundPtr == NULL)
		{
			continue;
		}

		if ( soundPtr->is_play())
		{
			playingSoundIDs.push_back(soundPtr->get_id());
			soundPtr->stop_and_wait();
		}
		soundPtr->freeze();
	}

	BOOL bFreezzed = FALSE;
	while ( !bFreezzed)
	{
		bFreezzed = TRUE;
		for ( int id = 0; id < size; id++)
		{
			ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( id);
			if ( soundPtr == NULL)
			{
				continue;
			}

			bFreezzed = bFreezzed && soundPtr->is_freeze();
		}
		Sleep( 50);
	}

	bool bSucc = device->ReplaceDevice( sDeviceName);

	if ( !bSucc)
	{		
		return S_FALSE;
	}

	for ( int id = 0; id < size; id++)
	{
		ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( id);
		if ( soundPtr == NULL)
		{
			continue;
		}

		soundPtr->unfreeze();
	}

	bFreezzed = TRUE;
	while ( bFreezzed)
	{
		bFreezzed = FALSE;
		for ( int id = 0; id < size; id++)
		{
			ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( id);
			if ( soundPtr == NULL)
			{
				continue;
			}

			bFreezzed = bFreezzed || soundPtr->is_freeze();
		}
		Sleep( 50);
	}

	std::vector<int>::iterator it = playingSoundIDs.begin(), end = playingSoundIDs.end();
	for ( ; it != end; it++)
	{
		int id = *it;
		ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( id);
		if ( soundPtr == NULL)
		{
			continue;
		}
		soundPtr->play();
	}
	playingSoundIDs.clear();	
	
#endif	

	device->UnLock();
	bDeviceChanging = FALSE;
	mutex.unlock();
	mutexUpdateData.unlock();
	mutexPosition.unlock();
	mutexOpenClose.unlock();

	return bSucc ? S_OK : S_FALSE;
#else
	return S_OK;
#endif
}

//////////////////////////////////////////////////////////////////////////

int CSoundManager::FindSound( int id)
{
	CWaitAndSignal waitAndSignal( mutex);
	ATLASSERT( id > -1);
	//make_dump( id > -1);
	if ( id < 0)
		return FALSE;
	ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( id);
	if ( soundPtr == NULL)
		return -1;
	return soundPtr->get_id();	
}

void CSoundManager::DeleteElementInSoundArray( int index)
{
	ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( index);
	removeSound( soundPtr);	
}

//////////////////////////////////////////////////////////////////////////

void CSoundManager::FreezeAll()
{
	/*BPausa = TRUE;
	for ( int i = 0; i < MAX_SOUNDS; i++)
	{ 
		CPlaySoundThread *plSoundThread = &PlaySoundThreads[ i];
		if ( plSoundThread->get_handle() != NULL && plSoundThread->is_resume()) 
		{	 		
			ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( plSoundThread->get_sound_id());
			if ( soundPtr == NULL)
				return;
			plSoundThread->set_pos( soundPtr->get_pos( 1));
			plSoundThread->set_pause( TRUE);
			soundPtr->stop();			
			if ( !plSoundThread->suspend())
			{
				DWORD iError = GetLastError();
				ATLASSERT( iError == 0);
			}
			ATLTRACE("Freeze All Suspend thread Position N %d\n",i);			
		}
	}*/
}

void CSoundManager::UnFreezeAll()
{
	/*BPausa = FALSE;
	for ( int i = 0; i < MAX_SOUNDS; i++)
	{ 
		CPlaySoundThread *plSoundThread = &PlaySoundThreads[ i];
		if ( plSoundThread->get_handle() != NULL && plSoundThread->is_pause()) 
		{	 		
			ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( plSoundThread->get_sound_id());
			if ( soundPtr == NULL)
				return;
			soundPtr->set_pos( plSoundThread->get_pos());
			if ( !plSoundThread->resume())
			{
				DWORD iError = GetLastError();
				ATLASSERT( iError == 0);
			}			
		}
	}*/
}

void CSoundManager::Silence( BOOL mode)
{
	/*for ( int i = 0; i < MAX_SOUNDS; i++)
	{ 
		CPlaySoundThread *plSoundThread = &PlaySoundThreads[ i];
		if ( plSoundThread->get_handle() != NULL && plSoundThread->is_resume())
		{
			// Volume control			
			ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( plSoundThread->get_sound_id());
			if ( soundPtr == NULL)
				return;
			// Sound
			if ( mode == 0)
			{	
				soundPtr->set_volume( plSoundThread->volume);  				
			}
			// Silence
			else
			{				
				plSoundThread->volume = soundPtr->get_volume();
				soundPtr->set_volume( 0.0f);
			}
		}
	}*/
}


//////////////////////////////////////////////////////////////////////////

omsresult CSoundManager::GetLexema( int num_person, int *lexema)
{	
	/*if ( sound->GetPositionPlaySoundThread() >= 0) 
	{ 
		for ( int i = 0; i < MAX_SOUNDS; i++)
		{ 
			CPlaySoundThread *plSoundThread = sound->getPlaySoundThread( i);
			if ( plSoundThread->is_resume())
				if ( plSoundThread->get_bot_number() == num_person)
				{  
					// 20.04.2006 Комментирую стандартный вызов лексемы
					*lexema = sound->RecognizeLexemaAudiereSound( i);


					#ifdef DEBUG_INFO
					ATLTRACE(_T("LexemaPS =%d; Avatar = %d Num_Bot=%d\n"),*lexema,num_person,sound->PlaySoundThreads[i].num_bot); return S_OK;
					#endif
				}
		}
	}
	*/

	/*	else if (sound->inputActive == FALSE)
	if (sound->num_avatar != -1)
	if (num_person == sound->num_avatar)	
	{ 
	*lexema = sound->RecognizeLexema(); 
	#ifdef DEBUG_INFO
	ATLTRACE(_T("Microphone_no_active Lexema=%d; Avatar=ActiveAvatar ID_BUFFER=%d\n"),*lexema,sound->id_buffer); return S_OK;
	#endif
	}
	else 
	{	
	*lexema = -1;  
	return S_OK; 
	}
	else	
	{	
	*lexema = -1;  
	#ifdef DEBUG_INFO
	ATLTRACE(_T("Microphone_no_active Lexema=-1; Avatar=%d; ActiveAvatar=-1\n"),num_person); return S_OK;
	#endif
	return S_OK; 
	}
	else if (num_person == sound->num_avatar) 
	{   
	*lexema = sound->RecognizeLexema(); 
	#ifdef DEBUG_INFO
	ATLTRACE(_T("Microphone Active Lexema=%d ActiveAvatar=Avatar ID_BUFFER=%d\n"),*lexema,sound->id_buffer); 
	#endif
	return S_OK;
	}
	else 
	{ 
	*lexema = -1; 
	return S_OK; 
	}*/

	return S_OK;
}

int CSoundManager::RecognizeLexema()
{
	/*	DWORD start_position_sound,end_position_sound;
	DWORD nPosition;
	if ( dx_use_output)
	{ 
	buf.DxBuffers[id_buffer].dx_bufer->GetCurrentPosition(&nPosition, 0);
	NewPlaySoundPosition = nPosition;
	while ( nPosition >= DATA_SIZE) nPosition-=DATA_SIZE;
	}
	else
	{	
	mmtimeOut.wType = TIME_BYTES; 
	waveOutGetPosition(hWaveOut, (LPMMTIME)&mmtimeOut, sizeof(MMTIME));
	DWORD nPosition = mmtimeOut.u.cb-buf.num_byte;
	}

	if ( nPosition - 256<0) 
	start_position_sound = 0;
	else 
	start_position_sound = nPosition - 256;

	if ( nPosition + 256 > DATA_SIZE) 
	end_position_sound = DATA_SIZE;
	else 
	end_position_sound = nPosition + 256;

	if ( buf.DxBuffers[ id_buffer].position > 0)
	{ 
	if (NewPlaySoundPosition-PreviousPlaySoundPosition<DATA_SIZE/2)
	{ 
	#ifdef DEBUG_MODE
	//	  ATLTRACE("PreviousLexema = %d\n",PreviousLexema);
	#endif
	return PreviousLexema; 
	}
	else
	{ 
	buf.DxBuffers[id_buffer].get_buffer();
	PreviousPlaySoundPosition = NewPlaySoundPosition;
	float limit = 2.0;
	// ----------------------------------------------------------------------
	// 20.04.2006
	// Нужно обязательно поправить обратно
	PreviousLexema=recognize.DoFilter(end_position_sound, (BYTE *) buf.DxBuffers[id_buffer].data, start_position_sound,limit);
	// ----------------------------------------------------------------------
	//	  PreviousLexema=fnGetLexema(end_position_sound, (BYTE *) buf.DxBuffers[id_buffer].data, start_position_sound,limit);
	#ifdef DEBUG_MODE
	ATLTRACE("Lexema = %d\n",PreviousLexema);
	#endif
	return PreviousLexema;
	}
	}
	*/
	return -1;
}

//---------------------------------------------------------
int CSoundManager::RecognizeLexemaBeforeSend(LPSTR buffer, WORD buflen)
{ 
	return recognize.DoFilter(buflen, (unsigned char *)buffer, 0,3.5);
}

//----------------------------------------------
int CSoundManager::RecognizeLexemaAudiereSound( int position)
{
	/*CPlaySoundThread *plSoundThread = &PlaySoundThreads[ position];
	//int id = FindSound( PlaySoundThreads[ position].id_sound);
	//if (id == -1) return 0;
	if ( plSoundThread->get_sound_id() == -1) return 0;

	ISoundPtrBase *soundPtr = soundPtrMap.get_obj_by_id( plSoundThread->get_sound_id());
	if ( soundPtr == NULL)
		return 0;

	DWORD max_length_sound = soundPtr->get_buffer_size();

	DWORD end_position_sound, start_position_sound, current_buffer_position;

	plSoundThread->newPlaySoundPosition = soundPtr->get_pos( 1);

	if ( plSoundThread->newPlaySoundPosition - plSoundThread->previousPlaySoundPosition < 6000) 
	{    
#ifdef DEBUG_INFO
		ATLTRACE(_T("Return previous lexema: NewPosition=%d; OldPosition=%d \n"),PlaySoundThreads[position].NewPlaySoundPosition, PlaySoundThreads[position].PreviousPlaySoundPosition);
#endif
		return plSoundThread->previousLexema; 
	}

	LPSTR str;
	DWORD size;
	str = soundPtr->get_buffer( &size);
	if ( str != NULL)
	{
		plSoundThread->previousLexema = recognize.DoFilter( size, (BYTE *)str, 0, 3.5);
		//  PlaySoundThreads[position].PreviousLexema = fnGetLexema(size, (BYTE *)str, 0);

		if ( size != 0) 
			GlobalFreePtr(str);
		plSoundThread->previousPlaySoundPosition = plSoundThread->newPlaySoundPosition;
		return plSoundThread->previousLexema;
	}*/
	return 0;
}

//////////////////////////////////////////////////////////////////////////

void CSoundManager::setSoundBase( BSTR newVal)
{
	sSoundBase = newVal;
}

AudioDevicePtr CSoundManager::GetDevice()
{
	return device;
}

void CSoundManager::setResMan( res::IResMan *aResMan)
{
	mpResMan = aResMan;
}

void CSoundManager::Lock()
{
	mutex.lock();
}

void CSoundManager::UnLock()
{
	mutex.unlock();
}