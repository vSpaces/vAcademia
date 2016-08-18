
// nrmAudio.cpp: implementation of the nrmAudio class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AudioEventsManager.h"
#include "ThreadAffinity.h"
#include "nrmAudio.h"

nrmAudioEventsManager::nrmAudioEventsManager() : MP_VECTOR_INIT(m_audioEvents)
{
	evEventsUpdated.Create( NULL, true, false);
	m_csEvents.Init();
	Start();
}

nrmAudioEventsManager::~nrmAudioEventsManager()
{	
	m_csEvents.Lock();

	ShouldStop();

	MP_VECTOR<TAudioEvent*>::iterator it = m_audioEvents.begin()
									, itend = m_audioEvents.end();
	for ( ; it != itend; it++)
	{
		TAudioEvent *pAudioEvent = *it;		
		MP_DELETE( pAudioEvent);
	}
	m_audioEvents.clear();

	evEventsUpdated.SetEvent();

	m_csEvents.Unlock();

	Stop();

	m_csEvents.Term();
}

void	nrmAudioEventsManager::AddEvents( nrmAudio* apAudio, int aType, int aCode)
{
	m_csEvents.Lock();

	if( IsRunning())
	{
		MP_NEW_P3(newEvent, TAudioEvent, apAudio, aType, aCode);
		m_audioEvents.push_back(newEvent);
		evEventsUpdated.SetEvent();
	}

	m_csEvents.Unlock();
}

void	nrmAudioEventsManager::RemoveEvents( nrmAudio* apAudio)
{
	m_csEvents.Lock();

	if( IsRunning())
	{
		MP_VECTOR<TAudioEvent*>::iterator it = m_audioEvents.begin();
		for( ; it != m_audioEvents.end(); )
		{
			TAudioEvent* pEvent = *it;
			if( apAudio == pEvent->pAudio)
			{
				MP_DELETE(pEvent);
				it = m_audioEvents.erase( it);
			}
			else
			{
				it++;
			}
		}
	}

	m_csEvents.Unlock();
}

DWORD	nrmAudioEventsManager::Run()
{
	SetSecondaryThreadAffinity( m_hThread);

	// Sample thread code...
	while( !ShouldStop() ) 
	{
		evEventsUpdated.WaitForEvent();
		
		m_csEvents.Lock();

		std::vector<TAudioEvent*> audioEvents;
		MP_VECTOR<TAudioEvent*>::iterator it = m_audioEvents.begin(), itend = m_audioEvents.end();
		for ( ; it != itend; it++)
		{
			TAudioEvent *pAudioEvent = *it;
			audioEvents.push_back( pAudioEvent);
		}
		m_audioEvents.clear();

		evEventsUpdated.ResetEvent();

		m_csEvents.Unlock();

		std::vector<TAudioEvent*>::iterator it_ = audioEvents.begin(), itend_ = audioEvents.end();
		for ( ; it_ != itend_; it_++)
		{
			TAudioEvent *pAudioEvent = *it_;
			if ( pAudioEvent->type == nrmAudio::ON_LOADED)
			{
				pAudioEvent->pAudio->HandleOnLoaded( pAudioEvent->code);
			}
			else if ( pAudioEvent->type == nrmAudio::ON_STOPPED)
			{
				pAudioEvent->pAudio->HandleOnStopped( pAudioEvent->code);
			}
			else if ( pAudioEvent->type == nrmAudio::ON_DESTROY)
			{
				pAudioEvent->pAudio->HandleOnDestroy();
			}
			MP_DELETE( pAudioEvent);
		}
		audioEvents.clear();
	}

	return 0;
}