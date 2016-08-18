
// nrmAudio.cpp: implementation of the nrmAudio class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "nrmAudio.h"
#include "IAsynchResourcemanager.h"
#include "nrm3DObject.h"
#include "ifile.h"

#include "RMContext.h"
#include "nrm3DSoundManager.h"
#include "SplinePoints.h"
#include "../nmath/BSPoint.h"
#include "../nmath/BSSegment.h"
#include "../nmath/BSpline.h"
#include "mdump.h"
#include "ThreadAffinity.h"
#include "SimpleThreadStarter.h"

nrmAudio::nrmAudio(mlMedia* apMLMedia) : nrmObject(apMLMedia)
{
	m_iSoundId					= -1;
	m_iEmitterType				= SND_NONE;
	m_dwPos						= 0;
	m_bLoaded					= TRUE;
	m_bLoading					= FALSE;
	m_bOpening					= FALSE;
	m_bPlayingByLoading			= FALSE;
#if SND_BUILD_H_STREAMING
	m_bStreaming				= FALSE;
	m_dwSize					= 0;
	m_dwLoadedBytes				= 0;
	m_dwDuration				= 0;	
#endif
	m_p3DObj					= NULL;
	m_dFactorDist				= 6000.0;
	m_dMinDist					= 1500.0;
	m_iMode						= ENABLE_PLAY;
	m_wsSrc						= L"";
	m_bLoadNeed					= FALSE;
	m_fVolume					= 0.0f;
	m_iScriptVolume				= -1;  // уровень громкости звука от скрипта не задан
#ifdef DEBUG
	destroy_counter				= 0;
#endif
	m_bNeedUpdate				= TRUE;
	m_pAsyncResource			= NULL;
	m_bRemoving					= FALSE;
	m_pSpline					= NULL;
	m_pSplinePoints				= NULL;
	m_iStopReason				= PLAY;
	m_bPlaying					= FALSE;
	m_bPlayed					= FALSE;
	m_sScriptLocation			= L"";
	m_bPlayingEnabled			= TRUE;
	m_bAlwaysPlaying			= FALSE;
}

nrmAudio::~nrmAudio()
{	
	if (m_pSplinePoints != NULL)
		MP_DELETE( m_pSplinePoints);
	if (m_pSpline != NULL)
		MP_DELETE( m_pSpline);

#ifdef DEBUG
	destroy_counter++;
	ATLASSERT( destroy_counter == 1);
#endif

	ATLASSERT(m_pRenderManager);
	rm::rmISoundManager* m_pSoundManager = NULL;
	if ( m_pRenderManager)
		m_pSoundManager = m_pRenderManager->GetSoundManager();
	if ( m_pSoundManager && m_iSoundId != -1)
	{
		m_pSoundManager->CloseSound( m_iSoundId);
		m_iSoundId = -1;
	}	

	if ( m_pAsyncResource && m_bLoading)
		PauseLoading( m_pAsyncResource);

	m_pAsyncResource = NULL;
}

// загрузка нового ресурса
bool nrmAudio::SrcChanged()
{
	ATLASSERT(m_pRenderManager);
	rm::rmISoundManager* m_pSoundManager = NULL;
	if ( m_pRenderManager)
		m_pSoundManager = m_pRenderManager->GetSoundManager();
	if ( !m_pSoundManager)
	{
		rm_trace("ERROR! nrmAudio::SrcChanged - Sound Manager is NULL\n");
		return false;
	}
	// 1. проверить, если звук играется, то был ли до этого создан буфер вершин
		// 1.1. остановить
		// 1.1. запустить
	BOOL bSpeech = GetMLMedia()->GetType() == MLMT_SPEECH;
	mlIContinuous* pCont = GetMLMedia()->GetIContinuous();
#if SND_BUILD_H_STREAMING
	m_bStreaming = GetMLMedia()->GetILoadable()->IsStreaming();
#endif
	if ( bSpeech)
	{
		ATLASSERT( pCont == NULL);	
		if ( pCont != NULL) 
			return false;
		if ( m_iSoundId > -1)
		{
			m_dwPos = 0;
			GetMLMedia()->GetIContinuous()->SetCurPos( m_dwPos);
			m_pSoundManager->CloseSound( m_iSoundId);
			m_iSoundId = -1;
			if ( pCont->GetPlaying())
			{
				// если статус проигрывания звука в rmml равен true, то его надо остановить
				pCont->Stop();
			}
		}
	}
	else
	{
		ATLASSERT( pCont);	
		if ( pCont == NULL) 
			return false;

		if ( m_iSoundId > -1)
		{
			if ( pCont->GetPlaying())
			{
				m_pSoundManager->StopSound(m_iSoundId);
			}
			m_dwPos = 0;
			GetMLMedia()->GetIContinuous()->SetCurPos( m_dwPos);
			m_pSoundManager->CloseSound(m_iSoundId);
			m_iSoundId = -1;
			if ( pCont->GetPlaying())
			{
				// если статус проигрывания звука в rmml равен true, то его надо остановить
				pCont->Stop();
			}
		}
	}

	m_bLoaded = FALSE;
	const wchar_t* pwcSrc = mpMLMedia->GetILoadable()->GetSrc();
	if ( pwcSrc != NULL)
	{
		m_wsSrc = pwcSrc;
		m_bNeedUpdate = TRUE;
		
		if ( !m_bAlwaysPlaying && m_iMode == DIS_PLAY)
		{
			m_bLoadNeed = TRUE;
			return true;
		}

		if ( !OpenSound( pwcSrc))
			return false;		
	}

	if ( !bSpeech)
		PlayingChanged();
	return true;
}

bool nrmAudio::EmitterTypeChanged()
{
	const wchar_t* pwcType = mpMLMedia->GetIAudible()->GetEmitterType();
	if ( pwcType == NULL)
	{
		return false;
	}
	
	nrm3DSoundManager *p3DSoundManager = m_pRenderManager->Get3DSoundManager();

	ATLASSERT( p3DSoundManager);
	if ( p3DSoundManager == NULL)
		return false;	

	m_iEmitterType = p3DSoundManager->GetAudioType( pwcType);
	int iPrevMode = m_iMode;
	m_iMode = p3DSoundManager->GetAudioMode( m_iEmitterType);

	if ( m_bAlwaysPlaying && m_iMode == DIS_PLAY)
	{
		// устанавливаем режим проигрывания звука, так как установлено св-во всегда проигрывать
		m_iMode = ENABLE_PLAY;
	}

	if ( iPrevMode == DIS_PLAY && iPrevMode != m_iMode)
	{
		// открываем звук, если запрос был сделан и он еще не открыт.
		if ( m_bLoadNeed)
		{
			m_bLoadNeed = FALSE;
			OpenSound( m_wsSrc.c_str());
		}
	}

	if ( m_iScriptVolume < 0)
	{
		m_fVolume = p3DSoundManager->GetAudioVolume( m_iEmitterType);
		SetAudioVolume( m_fVolume);
	}
	
	return true;
}

inline CVector3D string_to_vector( std::string spos)
{	
	std::string s_pos = spos;
	int pos = spos.find( ',');
	float a_pos[3];
	int j = 0;
	while ( pos > -1)
	{
		std::string spos1 = s_pos.substr( 0, pos);
		a_pos[ j] = StringToFloat( spos1.c_str());
		s_pos = s_pos.substr( pos + 1, s_pos.length() - pos);		
		pos = s_pos.find( ',');
		j++;
	}
	a_pos[ j] = StringToFloat( s_pos.c_str());

	return CVector3D( a_pos[ 0], a_pos[ 1], a_pos[ 2]);
}

// загрузка нового ресурса
bool nrmAudio::SplineSrcChanged()
{
	const wchar_t* pwcSrc = mpMLMedia->GetIAudible()->GetSplineSrc();

	if ( pwcSrc == NULL)
	{
		return false;
	}

	USES_CONVERSION;
	LPCSTR s = W2A(pwcSrc);

	if ( strlen( s) < 5)
	{
		return false;
	}

	//std::string path = "/resources/sound/sqvu_LineGround.xml";
	std::string path = s;
	ifile* file = g->rl->GetResMemFile(path.c_str());
	if (file)
	{
		TiXmlDocument doc;
		unsigned char* data = MP_NEW_ARR(unsigned char, file->get_file_size() + 1);
		data[file->get_file_size()] = 0;
		file->read(data, file->get_file_size());
		doc.Parse((const char*)data);
		MP_DELETE_ARR(data);
		g->rl->DeleteResFile(file);

		if (m_pSpline != NULL)
			MP_DELETE( m_pSpline);
		m_pSpline = MP_NEW(CBSpline);
		
		TiXmlNode* props = (TiXmlNode*) doc.FirstChild();

		if ( props == NULL || props->NoChildren())
		{		
			return false;
		}

		TiXmlNode* prop = props->FirstChild();

		int i = 0;
		while (prop != NULL)
		{
			TiXmlElement* elem = prop->ToElement();

			if ((!elem->Attribute("position")) ||
				(!elem->Attribute("type")) ||
				(!elem->Attribute("invec")) ||
				(!elem->Attribute("outvec")) ||
				i != 0 && (!elem->Attribute("length"))
				)
			{
				continue;
			}	

			CVector3D v_pos = string_to_vector( elem->Attribute("position"));
			CVector3D v_invec = string_to_vector( elem->Attribute("invec"));
			CVector3D v_outvec = string_to_vector( elem->Attribute("outvec"));
						
			if ( i != 0)
			{
				std::string slength = elem->Attribute("length");
			}
			CBSCPoint point = CBSCPoint( v_pos, v_invec, v_outvec);
			m_pSpline->AddPoint( point);
			
			i++;

			prop = props->IterateChildren(prop);			
		}

		if (m_pSplinePoints != NULL)
			MP_DELETE( m_pSplinePoints);
		m_pSplinePoints = MP_NEW(CSplinePoints);
		m_pSplinePoints->initialize( m_pSpline, 0.005f);

		return true;
	}
	return false;
}

bool nrmAudio::LocationChanged()
{
	const wchar_t* sLocation = mpMLMedia->GetIAudible()->GetLocation();

	if ( sLocation == NULL)
	{
		return false;
	}
	
	m_sScriptLocation = sLocation;

	return true;
}

bool nrmAudio::VolumeChanged()
{
	const int volume_ = mpMLMedia->GetIAudible()->GetVolume();	

	ATLASSERT(m_pRenderManager);
	rm::rmISoundManager* m_pSoundManager = NULL;

	if ( m_pRenderManager)
	{
		m_pSoundManager = m_pRenderManager->GetSoundManager();
	}

	if ( !m_pSoundManager)
	{
		return false;
	}

	if (!GetMLMedia())
	{
		return false;
	}
	
	if ( volume_ >= 0)
	{
		m_fVolume = ((float) volume_) / 100.0f;	
		m_iScriptVolume = -1;
		SetAudioVolume( m_fVolume);
		m_iScriptVolume = volume_;
		return true;
	}

	m_iScriptVolume = volume_;

	// отрицательное значение, отключаем установку звука от скрипта
	nrm3DSoundManager *p3DSoundManager = m_pRenderManager->Get3DSoundManager();

	ATLASSERT( p3DSoundManager);
	if ( p3DSoundManager == NULL)
		return false;	

	// возвращаем исходное значение громкости звука в зависимости от типа звука
	m_fVolume = p3DSoundManager->GetAudioVolume( m_iEmitterType);
	SetAudioVolume( m_fVolume);		
	
	return true;
}

// Начать/Закончить проигрывание звука
void nrmAudio::PlayingChanged()
{
	ATLASSERT(m_pRenderManager);
	rm::rmISoundManager* m_pSoundManager = NULL;

	if ( m_pRenderManager)
		m_pSoundManager = m_pRenderManager->GetSoundManager();

	if ( !m_pSoundManager)
		return;

	if (!GetMLMedia())
	{
		return;
	}
	
	if ( !m_bLoaded && !m_bLoading && !m_bOpening)
	{
		const wchar_t* pwcSrc = mpMLMedia->GetILoadable()->GetSrc();
		if ( pwcSrc)
		{
			rm_trace("Sound is not loaded already [PlayingChanged]: %s\n", CComString( pwcSrc).GetBuffer());
		}
		return;
	}	
	bool	bplay = GetMLMedia()->GetIContinuous()->GetPlaying();
	if ( bplay)
	{
		if ( !m_bAlwaysPlaying && m_iMode == DIS_PLAY)
		{
			//GetMLMedia()->GetIContinuous()->Stop();
			return;
		}
		if ( m_iSoundId != -1)
		{
			m_playingMutex.lock();
			m_bPlayed = FALSE;
			m_bPlaying = TRUE;
			m_iStopReason = PLAY;
			if ( m_bPlayingEnabled)
			{
				m_pSoundManager->PlaySound( m_iSoundId, -1, GetMLMedia()->GetIContinuous()->GetLoop());
			}

			m_playingMutex.unlock();
			PosChanged();
		}
		else if ( m_bLoading || m_bOpening)
		{
			m_playingMutex.lock();
			m_bPlayingByLoading = TRUE;
			m_playingMutex.unlock();
		}
	}
	else
	{
		if ( m_iSoundId == -1)	
		{
			if ( m_bLoading || m_bOpening)
			{
				m_playingMutex.lock();
				m_bPlayingByLoading = FALSE;
				m_playingMutex.unlock();
			}
			return;
		}
		m_playingMutex.lock();
		m_bPlayingByLoading = FALSE;		
		if ( m_pSoundManager->IsPlaying( m_iSoundId))
		{
			m_playingMutex.unlock();
			m_pSoundManager->GetPosition( m_iSoundId, m_dwPos);
			GetMLMedia()->GetIContinuous()->SetCurPos( m_dwPos);
			m_playingMutex.lock();

			m_pSoundManager->StopSound( m_iSoundId);
			m_bPlaying = FALSE;
			m_bPlayed = FALSE;
			m_iStopReason = STOP_BY_USER;
			m_playingMutex.unlock();
		}
		else
		{
			if ( m_iStopReason == STOP_BY_DISTANCE || m_iStopReason == STOP_BY_DISALLOW)
				m_iStopReason = STOP_BY_USER;
			m_bPlaying = FALSE;
			m_bPlayed = FALSE;
			m_playingMutex.unlock();
		}
	}
}

// переставить звук на новое место
void nrmAudio::FrameChanged()
{
}

// переставить звук на новое место (ms)
void nrmAudio::PosChanged()
{
	ATLASSERT(m_pRenderManager);
	rm::rmISoundManager* m_pSoundManager = NULL;
	if ( m_pRenderManager)
		m_pSoundManager = m_pRenderManager->GetSoundManager();
	if ( !m_bLoaded && !m_bLoading && !m_bOpening)
	{
		if (GetMLMedia())
		{
			const wchar_t* pwcSrc = GetMLMedia()->GetILoadable()->GetSrc();
			if ( pwcSrc)
				rm_trace("Sound is not loaded already [PosChanged]: %s\n", CComString( pwcSrc).GetBuffer());
		}
		return;
	}
	if ( m_iSoundId == -1 || !m_pSoundManager)	
		return;

	if (GetMLMedia())
	{
		unsigned long lPos = GetMLMedia()->GetIAudible()->GetRMCurPos();
		m_pSoundManager->GetPosition( m_iSoundId, m_dwPos);
		if ( lPos != m_dwPos)
		{
			m_pSoundManager->SetPosition( m_iSoundId, lPos);
			m_dwPos = lPos;		
		}
	}
}

// запрашивает значение _duration (продолжительность в ms)
long nrmAudio::GetDuration()
{
	ATLASSERT(m_pRenderManager);
	rm::rmISoundManager* m_pSoundManager = NULL;
	if ( m_pRenderManager)
		m_pSoundManager = m_pRenderManager->GetSoundManager();
	DWORD	duration = 0; 
	if ( !m_pSoundManager)	return 0;
	// Get sound duration
	if ( m_iSoundId == -1 && !m_bLoading)
	{
		//const wchar_t* pwcSrc = mpMLMedia->GetILoadable()->GetSrc();
		//OpenSound( pwcSrc);		
		return -1;
	}
	if ( m_iSoundId != -1)
	{
#if SND_BUILD_H_STREAMING
		if ( m_bStreaming)
		{			
			if ( !IsLoaded())
			{
				// ресурс полностью не загружен				
				if ( m_bOpening)
					return 0;
				
				DWORD iFrequency = 0;
				m_pSoundManager->GetFrequency( m_iSoundId, iFrequency);
				if ( iFrequency == 0)
					return 0;
				
				m_pSoundManager->GetSize( m_iSoundId, duration);
				if ( duration > 0)
				{
					// в заголовке есть информация о длине файла
					m_pSoundManager->GetLength( m_iSoundId, duration);
					duration = (DWORD)((1000.0f * (float)duration) / (float)iFrequency);
				}
				else 
				{
					duration = m_dwDuration;
				}
			}
			else if ( m_bNeedUpdate && m_bLoaded)
			{
				if ( m_bOpening)
					return 0;
				// требуется обновить ресурс в SoundMan
				m_loadingUpdateMutex.lock();
				omsresult iResult = m_pSoundManager->UpdateFileInfo( m_iSoundId, m_dwSize, true);				
				if ( !( iResult == OMS_OK || iResult == WARNING_RES_IS_NOT_NEED_TO_UPDATE))
				{
					m_loadingUpdateMutex.unlock();
					// идет процесс загрузки звука в audiere
					return 0;
				}
				m_loadingUpdateMutex.unlock();
				m_pSoundManager->GetDuration( m_iSoundId, duration);
				if ( m_dwDuration != duration)
				{
					CComString s;
					s.Format( "nrmAudio::GetDuration m_bNeedUpdate = %b", m_bNeedUpdate);
					g->lw.WriteLn( s.GetBuffer(), 0);
					s.Format( "nrmAudio::GetDuration m_dwDuration = %d", m_dwDuration);
					g->lw.WriteLn( s.GetBuffer(), 0);
					s.Format( "nrmAudio::GetDuration duration = %d\n", duration);
					g->lw.WriteLn( s.GetBuffer(), 0);
					m_dwDuration = duration;
				} 
				m_bNeedUpdate = FALSE;
			}
			else
			{
				m_pSoundManager->GetDuration( m_iSoundId, duration);
				if ( m_dwDuration != duration)
				{
					CComString s;
					s.Format( "nrmAudio::GetDuration m_dwDuration = %d", m_dwDuration);
					g->lw.WriteLn( s.GetBuffer(), 0);
					s.Format( "nrmAudio::GetDuration duration = %d\n", duration);
					g->lw.WriteLn( s.GetBuffer(), 0);
					m_dwDuration = duration;
				} 
			}
			
		}
		else
			m_pSoundManager->GetDuration( m_iSoundId, duration);
#else
		m_pSoundManager->GetDuration( m_iSoundId, duration);
#endif
	}
	return (long) duration;
}

void nrmAudio::update(DWORD /*delta*/)
{
	if (!GetMLMedia())
	{
		return;
	}

	if ( m_iSoundId == -1)
		return;	
	
#if SND_BUILD_H_STREAMING
	if ( !m_bLoaded && !m_bStreaming)	
		return;	

	if ( !m_bLoaded && !m_bLoading)
		return;

	if ( m_bOpening)
		return;

#else
	if ( !m_bLoaded)	
		return;	

	if ( !IsLoaded() && !m_bOpening)
		return;
#endif
	

	ATLASSERT( m_pRenderManager);
	rm::rmISoundManager* m_pSoundManager = NULL;
	if( m_pRenderManager)
		m_pSoundManager = m_pRenderManager->GetSoundManager();
	mlIContinuous* pCont = GetMLMedia()->GetIContinuous();
	if ( pCont == NULL)
	{ 
		// speech
		return;
	}
	else
	{
		// audio		
		if ( (!m_bAlwaysPlaying && m_iMode == DIS_PLAY) || !m_bPlayingEnabled)
		{
			//GetMLMedia()->GetIContinuous()->Stop();
			return;
		}
		if ( pCont->GetPlaying() && m_iMode != DIS_PLAY)
		{
			m_playingMutex.lock();
			if ( !m_pSoundManager->IsPlaying( m_iSoundId) && m_iStopReason != STOP_BY_DISTANCE && m_iStopReason != STOP_BY_DISALLOW)
			{
				// воспроизведение звука было остановлено из-за достижения конца файла или пользователем
				m_bPlayed = FALSE;
				m_bPlaying = FALSE;
#if SND_BUILD_H_STREAMING
				m_iStopReason = STOP_BY_ENDFILE;
				m_playingMutex.unlock();
				m_pSoundManager->GetPosition( m_iSoundId, m_dwPos);
				GetMLMedia()->GetIContinuous()->SetCurPos( m_dwPos);
				if ( m_bStreaming)
				{
					if ( IsLoaded() && !m_bOpening)
						pCont->onPlayed();
				}
				else
					pCont->onPlayed();
#else
				pCont->onPlayed();
#endif
			}
			else
			{
				m_playingMutex.unlock();
			}			
		}

		
		m_playingMutex.lock();
		if ( m_bPlayed)
		{
			// если флаг по какой-то причине не был сброшен, то надо остановить воспроизведение звука
			CComString s;
			const wchar_t* pwcSrc = mpMLMedia->GetILoadable()->GetSrc();
			if ( pwcSrc != NULL)		
			{
				s.Format("nrmAudio::update m_bPlayed == TRUE for '%s'", CComString( pwcSrc).GetBuffer());
				rm_trace("nrmAudio::update m_bPlayed == TRUE for '%s'\n", CComString( pwcSrc).GetBuffer());
			}
			else
			{
				s.Format("nrmAudio::update m_bPlayed == TRUE");
				rm_trace("nrmAudio::update m_bPlayed == TRUE\n");
			}
			g->lw.WriteLn( s.GetBuffer());
			
			if ( m_pSoundManager->IsPlaying( m_iSoundId))
			{
				m_pSoundManager->StopSound( m_iSoundId);
			}
			if ( pCont->GetPlaying())
			{
				pCont->onPlayed();
			}
			m_bPlayed = FALSE;
			m_bPlaying = FALSE;
			m_iStopReason = STOP_BY_ENDFILE;
		}
		m_playingMutex.unlock();		
	}
}

unsigned long nrmAudio::GetCurPos()
{
	ATLASSERT( m_pRenderManager);
	rm::rmISoundManager* m_pSoundManager = NULL;
	if ( m_pRenderManager)
		m_pSoundManager = m_pRenderManager->GetSoundManager();
	if ( m_iSoundId == -1 || !m_pSoundManager) 
		return 0xFFFFFFFF;

#if AUDIERE_BUILD_H
	//if ( !m_bLoading && IsLoaded() && !m_bOpening && !m_pSoundManager->IsPlaying( m_iSoundId)) 
	//	return 0xFFFFFFFF;
#else
	if ( !m_pSoundManager->IsPlaying( m_iSoundId)) 
		return m_dwPos;
#endif
	unsigned long uiPos = 0;
	m_pSoundManager->GetPosition( m_iSoundId, uiPos);
	return uiPos;
}

void nrmAudio::OnLoaded( int iError)
{
	if( !m_pRenderManager)
		return;

	m_removingMutex.lock();
	if( !m_bRemoving)
		m_pRenderManager->GetAudioEventsManager().AddEvents( this, ON_LOADED, iError);
	m_removingMutex.unlock();

	/*
	TAudioEvent *pAudioEvent = MP_NEW(TAudioEvent);
	pAudioEvent->type = ON_LOADED;
	pAudioEvent->code = iError;

	m_eventMutex.lock();
	m_audioEvents.push_back( pAudioEvent);
	evEventsUpdated.SetEvent();
	m_eventMutex.unlock();
	*/
}

void nrmAudio::OnStopped( int iReason)
{
	return;
	if( !m_pRenderManager)
		return;

	m_removingMutex.lock();
	if( !m_bRemoving)
		m_pRenderManager->GetAudioEventsManager().AddEvents( this, ON_STOPPED, iReason);
	m_removingMutex.unlock();

	/*TAudioEvent *pAudioEvent = MP_NEW(TAudioEvent);
	pAudioEvent->type = ON_STOPPED;
	pAudioEvent->code = iReason;
	m_eventMutex.lock();
	m_audioEvents.push_back( pAudioEvent);
	evEventsUpdated.SetEvent();
	m_eventMutex.unlock();	*/
}

void nrmAudio::HandleOnLoaded( int iError)
{
	if ( iError > 0)
	{
		CComString s;
		const wchar_t* pwcSrc = mpMLMedia->GetILoadable()->GetSrc();
		if ( pwcSrc != NULL)		
		{
			rm_trace("ERROR! nrmAudio::OnLoaded - Sound Resource '%s' is invalid\n", CComString( pwcSrc).GetBuffer());
			s.Format("ERROR! nrmAudio::OnLoaded - Sound Resource '%s' is invalid", CComString( pwcSrc).GetBuffer());
			g->lw.WriteLn( s.GetBuffer());
		}
		else
		{
			rm_trace("ERROR! nrmAudio::OnLoaded - Sound Resource is invalid\n");
			s.Format("ERROR! nrmAudio::OnLoaded - Sound Resource is invalid");
			g->lw.WriteLn( s.GetBuffer());
		}
		if (GetMLMedia())
		{
			GetMLMedia()->GetILoadable()->onLoad( RMML_LOAD_STATE_LOADERROR);
		}
		m_iSoundId = -1;
		return;
	}

	onOpened();
}

void nrmAudio::HandleOnStopped( int iReason)
{
	if ( IsLoaded())
	{		
		m_playingMutex.lock();
		if ( !m_bPlaying)
		{
			m_playingMutex.unlock();
			return;
		}

		rm::rmISoundManager* m_pSoundManager = NULL;
		if ( m_pRenderManager)
			m_pSoundManager = m_pRenderManager->GetSoundManager();
		ATLASSERT( m_pSoundManager);
		if ( m_pSoundManager == NULL)
		{
			m_playingMutex.unlock();
			return;
		}
		
		if (( m_iStopReason == STOP_BY_DISTANCE || m_iStopReason == STOP_BY_DISALLOW) && iReason == STOP_BY_ENDFILE)
		{
			m_iStopReason = iReason;
			m_bPlayed = TRUE;
			m_bPlaying = FALSE;
		}
		m_playingMutex.unlock();
	}
}

static DWORD WINAPI DeleteAudioThreadProc(LPVOID pData)
{
	if( pData)
	{
		nrmAudio* pnrmAudio = (nrmAudio*)pData;
		MP_DELETE(pnrmAudio);
	}
	return 0;
}

void nrmAudio::HandleOnDestroy()
{
	CSimpleThreadStarter deleteThreadStarter(__FUNCTION__);	
	deleteThreadStarter.SetRoutine(DeleteAudioThreadProc);
	deleteThreadStarter.SetParameter(this);
	deleteThreadStarter.Start();
}

void nrmAudio::Destroy()
{
#if SND_BUILD_H_STREAMING
	m_removingMutex.lock();
	m_bRemoving = TRUE;
	m_removingMutex.unlock();
#endif

	ATLASSERT(m_pRenderManager);
	rm::rmISoundManager* m_pSoundManager = NULL;
	if ( m_pRenderManager)
	{
		m_pSoundManager = m_pRenderManager->GetSoundManager();
		if ( m_pSoundManager && m_iSoundId != -1)
		{
			m_pSoundManager->StopSound( m_iSoundId);
		}
		m_pRenderManager->GetAudioEventsManager().RemoveEvents( this);
	}

	if ( m_p3DObj)
	{
		ATLTRACE( "Destroy::m_p3DObj=%u \n", (DWORD)m_p3DObj);
		if (GetMLMedia())
			m_p3DObj->detachSound( GetMLMedia());
	}
	m_p3DObj = NULL;

	m_pRenderManager->GetAudioEventsManager().AddEvents( this, ON_DESTROY, 0);
}

void nrmAudio::OnAsynchResourceLoaded(IAsynchResource* asynch)
{
	ATLTRACE( "nrmAudio::on_asynch_resource_loaded \n");
	rm::rmISoundManager* m_pSoundManager = NULL;
	if ( m_pRenderManager)
		m_pSoundManager = m_pRenderManager->GetSoundManager();
	ATLASSERT( m_pSoundManager);
	if ( m_pSoundManager == NULL)
		return;

#if SND_BUILD_H_STREAMING

	if ( m_bRemoving)
		return;

	if ( !asynch)
		return;

	if ( !GetMLMedia())
		return;

	const wchar_t* pwcSrc = GetMLMedia()->GetILoadable()->GetSrc();
	if ( pwcSrc == NULL)
		return;
	USES_CONVERSION;
	LPCSTR sSrc = W2A( pwcSrc);
	if ( strcmpi( sSrc, asynch->GetFileName()) != 0)
	{
		assert( false);
		return;
	}

	m_dwSize = asynch->GetSize();
	m_dwLoadedBytes = m_dwSize;

	mlILoadable *pmlILoadable = GetMLMedia()->GetILoadable();
	pmlILoadable->onLoading( m_dwLoadedBytes, m_dwSize);

	m_pAsyncResource = NULL;

	if ( m_bStreaming)
	{	
		CComString s;
		// если не вызвалось OnAsynchResourceLoadedPersent
		ifile *pfile = asynch->GetIFile();
		if ( !pfile)
		{
			s.Format( "Cannot found audio stream-file '%d' [%s]", m_iSoundId, pfile->get_file_path());
			g->lw.WriteLn( s.GetBuffer());
			rm_trace("Cannot found audio stream-file '%d' [%S]\n", m_iSoundId, pfile->get_file_path());
			if (GetMLMedia())
			{
				GetMLMedia()->GetILoadable()->onLoad( RMML_LOAD_STATE_LOADERROR);
			}
			return;
		}
		s.Format( "nrmAudio::OnAsynchResourceLoaded m_iSoundId = %d", m_iSoundId);
		g->lw.WriteLn( s.GetBuffer());
		if ( m_iSoundId == -1)
		{
			m_iSoundId  = m_pSoundManager->OpenSound( pfile, this, FALSE);
			if ( m_iSoundId == -1)
			{
				rm_trace("Cannot open audio stream-file m_iSoundId == -1 [%S]\n", pfile->get_file_path());
				s.Format("Cannot open audio stream-file m_iSoundId == -1 [%s]", pfile->get_file_path());
				g->lw.WriteLn( s.GetBuffer());				
				if (GetMLMedia())
				{
					GetMLMedia()->GetILoadable()->onLoad( RMML_LOAD_STATE_LOADERROR);
				}
				return;
			}
			m_bOpening = TRUE;
			m_bNeedUpdate = FALSE;
		}
		else
		{
			m_loadingUpdateMutex.lock();
			omsresult iResult = m_pSoundManager->UpdateFileInfo( m_iSoundId, m_dwSize, true);			
			if ( iResult == OMS_OK || iResult == WARNING_RES_IS_NOT_NEED_TO_UPDATE)
			{
				m_bNeedUpdate = FALSE;
			}
			else
			{
				m_bNeedUpdate = TRUE;
			}
			m_loadingUpdateMutex.unlock();
			
			DWORD duration = 0;
			m_pSoundManager->GetDuration( m_iSoundId, duration);
			s.Format( "nrmAudio::OnAsynchResourceLoaded duration = %d", duration);
			g->lw.WriteLn( s.GetBuffer());
			m_bLoaded = TRUE;
		}
		m_bLoading = FALSE;
		if ((!m_bOpening) && (GetMLMedia()))
		{
			GetMLMedia()->GetILoadable()->onLoad( RMML_LOAD_STATE_SRC_LOADED);
		}
		return;
	}

	ifile *pfile = asynch->GetIFile();
	if ( pfile)
	{
		m_iSoundId = m_pSoundManager->OpenSound( pfile, this, FALSE);
		if ( m_iSoundId == -1)
		{
			CComString s;
			rm_trace("Cannot open audio file m_iSoundId == -1 [%S]\n", pfile->get_file_path());
			s.Format("Cannot open audio file m_iSoundId == -1 [%s]", pfile->get_file_path());
			g->lw.WriteLn( s.GetBuffer());
			if (GetMLMedia())
			{
				GetMLMedia()->GetILoadable()->onLoad( RMML_LOAD_STATE_LOADERROR);
			}
			return;
		}
		m_bOpening = TRUE;
		m_bLoading = FALSE;
	}
	else
	{
		CComString s;
		rm_trace("Cannot open audio file pfile == NULL m_iSoundId = '%d'\n", m_iSoundId);
		s.Format("Cannot open audio file pfile == NULL m_iSoundId = '%d'", m_iSoundId);
		g->lw.WriteLn( s.GetBuffer());
		if (GetMLMedia())
		{
			GetMLMedia()->GetILoadable()->onLoad( RMML_LOAD_STATE_LOADERROR);
		}
		return;
	}

#else

	LPCSTR sFileName = asynch->GetFileName();	
	if ( sFileName != NULL)
	{
		USES_CONVERSION;
		const wchar_t* pwcSrc = A2W( sFileName);
		m_iSoundId = m_pSoundManager->OpenSound( pwcSrc, this);
		if ( m_iSoundId == -1)
		{			
			CComString s;
			rm_trace("Cannot open audio file m_iSoundId == -1 [%S]\n", CComString( pwcSrc).GetBuffer());
			s.Format("Cannot open audio file m_iSoundId == -1 [%s]", CComString( pwcSrc).GetBuffer());
			g->lw.WriteLn( s.GetBuffer());
			if (GetMLMedia())
			{
				GetMLMedia()->GetILoadable()->onLoad( RMML_LOAD_STATE_LOADERROR);
			}
			return;
		}
		m_bOpening = TRUE;
		m_bLoading = FALSE;
	}

#endif

}

void nrmAudio::OnAsynchResourceError(IAsynchResource* /*asynch*/)
{	
	CComString s;
	const wchar_t* pwcSrc = mpMLMedia->GetILoadable()->GetSrc();
	if ( pwcSrc != NULL)		
	{
		s.Format("nrmAudio::OnAsynchResourceError for '%d' src = '%s'", m_iSoundId, CComString( pwcSrc).GetBuffer());
		rm_trace("nrmAudio::OnAsynchResourceError for '%d' src = '%s'\n", m_iSoundId, CComString( pwcSrc).GetBuffer());
	}
	else
	{
		s.Format("nrmAudio::OnAsynchResourceError");
		rm_trace("nrmAudio::OnAsynchResourceError\n");
	}
	g->lw.WriteLn( s.GetBuffer());

	if (GetMLMedia())
	{
		GetMLMedia()->GetILoadable()->onLoad( RMML_LOAD_STATE_LOADERROR);
	}
	m_pAsyncResource = NULL;
	if ( m_iSoundId != -1)
	{
		rm::rmISoundManager* m_pSoundManager = NULL;
		if ( m_pRenderManager)
			m_pSoundManager = m_pRenderManager->GetSoundManager();
		if ( m_pSoundManager)
			m_pSoundManager->CloseSound( m_iSoundId);
		m_iSoundId = -1;
	}
#if SND_BUILD_H_STREAMING

#endif
}


void nrmAudio::OnAsynchResourceLoadedPersent(IAsynchResource* asynch, unsigned char /*percent*/)
{
	if ( !asynch)
		return;

	if (!GetMLMedia())
	{
		return;
	}

	unsigned int isize = asynch->GetSize();
	unsigned int loaded_bytes = asynch->GetBytesLoaded();	
	mlILoadable *pmlILoadable = GetMLMedia()->GetILoadable();
	
	pmlILoadable->onLoading( loaded_bytes, isize);

#if SND_BUILD_H_STREAMING
	if ( !m_bStreaming)
		return;

	if ( m_bRemoving)
		return;

	const wchar_t* pwcSrc = mpMLMedia->GetILoadable()->GetSrc();
	if ( pwcSrc == NULL)
	{
		PauseLoading( asynch);		
		asynch = NULL;
		return;
	}
	USES_CONVERSION;
	LPCSTR sSrc = W2A( pwcSrc);
	if ( strcmpi( sSrc, asynch->GetFileName()) != 0)
	{
		PauseLoading( asynch);		
		asynch = NULL;
		return;
	}	

	rm::rmISoundManager* m_pSoundManager = NULL;
	if( m_pRenderManager)
		m_pSoundManager = m_pRenderManager->GetSoundManager();
	ATLASSERT( m_pSoundManager);
	if ( m_pSoundManager == NULL)
		return;

	ifile *pfile = asynch->GetIFile();
	if ( !pfile)
	{
		rm_trace("Cannot found audio stream-file '%d' [%S]\n", m_iSoundId, pfile->get_file_path());		
		CComString s;
		s.Format( "Cannot found audio stream-file '%d' [%S]\n", m_iSoundId, pfile->get_file_path());
		g->lw.WriteLn( s.GetBuffer());
		GetMLMedia()->GetILoadable()->onLoad( RMML_LOAD_STATE_LOADERROR);
		return;
	}

	m_dwSize = isize;
	m_dwLoadedBytes = loaded_bytes;

	if ( m_iSoundId != -1)
	{		
		if ( !m_bOpening)
		{			
			if ( m_dwDuration == 0)
			{
				CComString s;
				DWORD dwCurrSize = 0;
				m_pSoundManager->GetCurrentSize( m_iSoundId, dwCurrSize);
				
				s.Format( "\nnrmAudio::OnAsynchResourceLoadedPersent dwCurrSize = %ld", dwCurrSize);					
				g->lw.WriteLn( s.GetBuffer());

				DWORD dwLength = 0;
				m_pSoundManager->GetLength( m_iSoundId, dwLength);
				s.Format( "nrmAudio::OnAsynchResourceLoadedPersent dwLength = %ld", dwLength);
				g->lw.WriteLn( s.GetBuffer());
			}

			m_loadingUpdateMutex.lock();
			m_pSoundManager->UpdateFileInfo( m_iSoundId, loaded_bytes, loaded_bytes == m_dwSize);
			m_loadingUpdateMutex.unlock();

			if ( m_dwDuration == 0)
			{
				DWORD iFrequency = 0;
				m_pSoundManager->GetFrequency( m_iSoundId, iFrequency);
				ATLASSERT( iFrequency > 0);
				if ( iFrequency == 0)
					return;
				DWORD duration = 0;
				m_pSoundManager->GetSize( m_iSoundId, duration);
				if ( duration > 0)
				{
					// в заголовке есть информация о длине файла
					m_pSoundManager->GetLength( m_iSoundId, duration);
					m_dwDuration = (DWORD)((1000.0f * (float)duration) / (float)iFrequency);
				}
				else 
				{
					CComString s;
					DWORD dwCurrSize = 0;
					m_pSoundManager->GetCurrentSize( m_iSoundId, dwCurrSize);

					s.Format( "\nnrmAudio::OnAsynchResourceLoadedPersent loaded_bytes = %d", loaded_bytes);
					g->lw.WriteLn( s.GetBuffer());

					s.Format( "nrmAudio::OnAsynchResourceLoadedPersent dwCurrSize = %d", dwCurrSize);					
					g->lw.WriteLn( s.GetBuffer());

					s.Format( "nrmAudio::OnAsynchResourceLoadedPersent isize = %d", isize);					
					g->lw.WriteLn( s.GetBuffer());
										
					m_pSoundManager->GetDuration( m_iSoundId, duration);
					s.Format( "nrmAudio::OnAsynchResourceLoadedPersent duration = %d", duration);
					g->lw.WriteLn( s.GetBuffer());

					// делаем проверку, если размер заголовка больше, чем размер скаченных данных
					// то мы не можем определить длительность звучания mp3-файла
					// 16 поставлено, чтобы увеличить точноcть
					if ( duration > 16 && dwCurrSize > 0)
					{
						DWORD64 duration_ = (DWORD64) ((DWORD64) isize * (DWORD64) duration) / (DWORD64) dwCurrSize;
						m_dwDuration = duration_;

						s.Format( "nrmAudio::OnAsynchResourceLoadedPersent m_dwDuration = %d", m_dwDuration);
						g->lw.WriteLn( s.GetBuffer());	
					}					
				}
			}

			//m_loadingUpdateMutex.lock();
/*			if ( iResult == WARNING_RES_IS_NOT_UPDATED)
				m_bNeedUpdate = TRUE;
			else if ( iResult == OMS_OK)*/
			//m_bNeedUpdate = FALSE;
			//m_loadingUpdateMutex.unlock();
		}
		return;
	}

	float coef = 0.0125f;
	if ( isize < 250000)
		coef = 0.8f * ( 1.5f - (float) isize / 250000.0f);
	if ( isize < 500000)
		coef = 0.4f * ( 1.5f - (float) isize / 500000.0f);
	else if ( isize < 1000000)
		coef = 0.2f * ( 1.5f - (float) isize / 1000000.0f);
	else if ( isize < 2000000)
		coef = 0.1f * ( 1.5f - (float) isize / 2000000.0f);
	else if ( isize < 4000000)
		coef = 0.05f * ( 1.5f - (float) isize / 4000000.0f);
	else if ( isize < 8000000)
		coef = 0.025f * ( 1.5f - (float) isize / 8000000.0f);

	if ( m_iSoundId == -1 && loaded_bytes >= isize * coef)
	{
		m_iSoundId  = m_pSoundManager->OpenSound( pfile, this, TRUE);
		if ( m_iSoundId == -1)
		{
			CComString s;
			s.Format( "Cannot open audio file [%s]", sSrc);
			g->lw.WriteLn( s.GetBuffer());
			rm_trace("Cannot open audio file [%S]\n", sSrc);
			GetMLMedia()->GetILoadable()->onLoad( RMML_LOAD_STATE_LOADERROR);
			return;
		}		
		GetMLMedia()->GetILoadable()->onLoad( RMML_LOAD_STATE_SRC_LOADING);
		//m_bNeedUpdate = FALSE;
		m_bOpening = TRUE;
	}
#endif
}

BOOL nrmAudio::OpenSound( const wchar_t* pwcSrc)
{
	m_bLoaded = FALSE;
	rm::rmISoundManager* m_pSoundManager = NULL;
	if( m_pRenderManager)
		m_pSoundManager = m_pRenderManager->GetSoundManager();
	ATLASSERT( m_pSoundManager);	
	if ( m_pSoundManager == NULL)
		return false;
	IResLibrary* res_lib = gRM->resLib;
	if ( res_lib == NULL)
		return false;
	IAsynchResourceManager* asynch_res_man = res_lib->GetAsynchResMan();
	if ( asynch_res_man == NULL)	
		return false;

#if SND_BUILD_H_STREAMING

	ifile *pfile = res_lib->GetResMemFile( pwcSrc);

	if ( pfile)
	{
		m_iSoundId  = m_pSoundManager->OpenSound( pfile, this, FALSE);
		res_lib->DeleteResFile( pfile);
		if ( m_iSoundId == -1)
		{			
			rm_trace("Cannot open audio file [%S] m_iSoundId == -1\n", CComString( pwcSrc).GetBuffer());
			CComString s;
			s.Format( "Cannot open audio file [%s] m_iSoundId == -1", CComString( pwcSrc).GetBuffer());
			g->lw.WriteLn( s.GetBuffer());
			if (GetMLMedia())
			{
				GetMLMedia()->GetILoadable()->onLoad( RMML_LOAD_STATE_LOADERROR);
			}
			return false;
		}
		m_bOpening = TRUE;
		m_bNeedUpdate = FALSE;
	}
	else
	{
		if ( m_pAsyncResource)
		{
#pragma message ("To make remove object of IAsyncResource")
			PauseLoading( m_pAsyncResource);
			//res_lib->DeleteAsynchResource( m_pAsyncResource);
		}

		int scriptPriority = ( m_iEmitterType & SND_BACK_MUSIC) ? ELP_BACKGROUND_SOUND : 0;

		std::vector<void*> owners;
		if ((m_p3DObj) && (m_p3DObj->m_obj3d))
		{
			owners.push_back(m_p3DObj->m_obj3d);
		}

		CComString sAnsiFileName = pwcSrc;
		m_pAsyncResource = asynch_res_man->GetAsynchResource( sAnsiFileName.GetBuffer(), true, RES_TYPE_SOUND, this, res_lib, &owners, NULL, scriptPriority);	
		
		if ( m_pAsyncResource == NULL)
		{
			rm_trace("Cannot open audio file [%s] m_pAsyncResource == NULL \n", sAnsiFileName.GetBuffer());

			CComString s;
			s.Format( "Cannot open audio file [%s] m_pAsyncResource == NULL ", sAnsiFileName.GetBuffer());
			g->lw.WriteLn( s.GetBuffer());
			if (GetMLMedia())
			{
				GetMLMedia()->GetILoadable()->onLoad( RMML_LOAD_STATE_LOADERROR);
			}
			return false;
		}
		else
		{	
			if ( ( m_iEmitterType & SND_BACK_MUSIC) != 0)
			{
				m_pAsyncResource->SetScriptPriority( ELP_BACKGROUND_SOUND);
			}
			m_bLoading = TRUE;
			asynch_res_man->AddLoadHandler( m_pAsyncResource, this);
			m_dwStartLoadTime = ::GetTickCount();			
		}
	}

#else

	m_pAsyncResource = asynch_res_man->GetAsynchResource( sAnsiFilename.GetBuffer(), RES_TYPE_SOUND, this, res_lib);		
	
	if ( m_pAsyncResource == NULL)
	{		
		m_iSoundId  = m_pSoundManager->OpenSound( (const wchar_t*) pwcSrc, this);
		if ( m_iSoundId == -1)
		{
			rm_trace("Cannot open audio file [%s] m_pAsyncResource == NULL \n", sAnsiFilename.GetBuffer());
			CComString s;
			s.Format( "Cannot open audio file [%s] m_pAsyncResource == NULL ", sAnsiFilename.GetBuffer());
			g->lw.WriteLn( s.GetBuffer());
			if (GetMLMedia())
			{
				GetMLMedia()->GetILoadable()->onLoad( RMML_LOAD_STATE_LOADERROR);
			}
			return false;
		}
		m_bOpening = TRUE;
	}
	else
	{		
		asynch_res_man->AddLoadHandler( m_pAsyncResource, this);
		m_dwStartLoadTime = ::GetTickCount();
		m_bLoading = TRUE;
	}

#endif

	return TRUE;
}

void nrmAudio::onOpened()
{
	rm::rmISoundManager* m_pSoundManager = NULL;
	if ( m_pRenderManager)
		m_pSoundManager = m_pRenderManager->GetSoundManager();
	ATLASSERT( m_pSoundManager);	
	if ( m_pSoundManager == NULL)
		return;	

	if ( ( m_bLoading || m_bOpening) && m_p3DObj)
	{
		//m_bLoading = m_bStreaming;
		if ( GetMLMedia())
		{
			if ( !m_p3DObj->attachSound( GetMLMedia(), pos3D))
			{
				const wchar_t* pwcSrc = mpMLMedia->GetILoadable()->GetSrc();
				if ( pwcSrc != NULL)
				{
					rm_trace("Cannot m_p3DObj->attachSound( GetMLMedia(), pos3D) for '%d' [%S]\n", m_iSoundId, CComString( pwcSrc).GetBuffer());
					CComString s;
					s.Format( "Cannot m_p3DObj->attachSound( GetMLMedia(), pos3D) for '%d' [%S]", m_iSoundId, CComString( pwcSrc).GetBuffer());
					g->lw.WriteLn( s.GetBuffer());
				}
				else
				{
					rm_trace("Cannot m_p3DObj->attachSound( GetMLMedia(), pos3D) for '%d'\n", m_iSoundId);
					CComString s;
					s.Format( "Cannot m_p3DObj->attachSound( GetMLMedia(), pos3D) for '%d'", m_iSoundId);
					g->lw.WriteLn( s.GetBuffer());
				}
				return;
			}
		}
		m_pSoundManager->SetVolume( m_iSoundId, 0.0f);
	}

#if SND_BUILD_H_STREAMING
	if ( !m_bLoaded && !m_bStreaming)
	{
		m_bLoaded = TRUE;
		m_bOpening = FALSE;
		if (GetMLMedia())
		{
			GetMLMedia()->GetILoadable()->onLoad( RMML_LOAD_STATE_SRC_LOADED);
		}
	}
	else if ( m_bStreaming)
	{
		if ( !m_bLoading && m_bOpening)
		{
			m_loadingUpdateMutex.lock();
			if ( m_bNeedUpdate)
			{
				int iResult = m_pSoundManager->UpdateFileInfo( m_iSoundId, m_dwSize, true);
				//ATLASSERT( iResult == OMS_OK || iResult == WARNING_RES_IS_NOT_NEED_TO_UPDATE);
				if ( iResult == OMS_OK || iResult == WARNING_RES_IS_NOT_NEED_TO_UPDATE)
					m_bNeedUpdate = FALSE;
			}
			m_loadingUpdateMutex.unlock();
			CComString s;
			DWORD duration = 0;
			m_pSoundManager->GetDuration( m_iSoundId, duration);
			s.Format( "nrmAudio::onOpened duration = %d", duration);
			g->lw.WriteLn( s.GetBuffer());
			m_bLoaded = TRUE;
		}

		if ( m_bLoading && m_bOpening)
		{
			// ничего не делаем
		}

		if ( m_bLoaded && m_bOpening && GetMLMedia())
		{
			GetMLMedia()->GetILoadable()->onLoad( RMML_LOAD_STATE_SRC_LOADED);
		}
		m_bOpening = FALSE;
	}
#else
	if ( !m_bLoaded)
	{
		m_bLoaded = TRUE;
		m_bOpening = FALSE;
		if (GetMLMedia())
		{
			GetMLMedia()->GetILoadable()->onLoad( RMML_LOAD_STATE_SRC_LOADED);
		}
	}
#endif

	nrm3DSoundManager *p3DSoundManager = m_pRenderManager->Get3DSoundManager();

	ATLASSERT( p3DSoundManager);
	if ( p3DSoundManager == NULL)
		return;

	m_playingMutex.lock();
	if ( m_bPlayingByLoading)
	{
		m_playingMutex.unlock();
		DWORD duration;
		m_pSoundManager->GetDuration( m_iSoundId, duration);

		CComString s;		
		s.Format( "nrmAudio::onOpened (in play) duration = %d", duration);
		g->lw.WriteLn( s.GetBuffer());

		if (!GetMLMedia())
		{
			return;
		}

		BOOL bSpeech = GetMLMedia()->GetType() == MLMT_SPEECH;
		if ( !bSpeech)
		{
			BOOL loop = GetMLMedia()->GetIContinuous()->GetLoop();
			bool bplay = GetMLMedia()->GetIContinuous()->GetPlaying();
			/*if ( now - m_dwStartLoadTime < duration)
			{
				if ( m_bPlayingEnabled)
				{
					m_pSoundManager->PlaySound( m_iSoundId, -1, (bool)(loop > 0));
				}
				if ( m_p3DObj)
					m_pSoundManager->SetVolume( m_iSoundId, 0.0f);
				m_pSoundManager->SetPosition( m_iSoundId,  now - m_dwStartLoadTime);
			}
			else if ( loop)
			{*/
			if ( bplay && m_iMode != DIS_PLAY)
			{
				m_playingMutex.lock();
				if ( m_bPlayingEnabled)
				{
					m_pSoundManager->PlaySound( m_iSoundId, -1, (bool)(loop > 0));
				}
				m_bPlaying = TRUE;
				m_playingMutex.unlock();
				if ( m_p3DObj)
					m_pSoundManager->SetVolume( m_iSoundId, 0.0f);
			}
			//}*/
		}
		m_playingMutex.lock();
		m_bPlayingByLoading = FALSE;
		m_playingMutex.unlock();
	}
	else
		m_playingMutex.unlock();

	m_iMode = p3DSoundManager->GetAudioMode( m_iEmitterType);	
	if ( m_bAlwaysPlaying && m_iMode == DIS_PLAY)
	{
		// устанавливаем режим проигрывания, так как установлено св-во всегда проигрывать
		m_iMode = ENABLE_PLAY;
	}

	if ( m_iScriptVolume < 0)
	{
		m_fVolume = p3DSoundManager->GetAudioVolume( m_iEmitterType);	
	}
	else
	{
		int ivolume = m_iScriptVolume;
		m_fVolume = ((float) ivolume) / 100.0f;
		m_iScriptVolume = -1;
		SetAudioVolume( m_fVolume);
		m_iScriptVolume = ivolume;
	}
}

void nrmAudio::PauseLoading( IAsynchResource* asynch)
{
	IResLibrary* res_lib = gRM->resLib;
	if ( res_lib == NULL)
		return;
	IAsynchResourceManager* asynch_res_man = res_lib->GetAsynchResMan();
	if ( asynch_res_man == NULL)
		return;	
	asynch_res_man->RemoveLoadHandler( asynch, this/*, res_lib*/);
	asynch_res_man->PauseDownloadAsyncResourse( asynch);
}

BOOL nrmAudio::IsLoading()
{
	return m_bLoading || m_bOpening;
}

BOOL nrmAudio::IsLoaded()
{
	return m_bLoaded;
}

void nrmAudio::set_p3dobj( nrm3DObject *ap3DObj, ml3DPosition aPos3D)
{
	//ATLASSERT( !this->m_p3DObj);
	ATLTRACE( "nrmAudio::set_p3dobj: %u \n", (DWORD)m_p3DObj);
	m_p3DObj = ap3DObj;
	pos3D = aPos3D;
	if ( m_p3DObj)
	{
		m_p3DObj->AddAsynchSound( m_pAsyncResource);
	}

	rm::rmISoundManager* m_pSoundManager = NULL;
	if ( m_pRenderManager)
	{
		m_pSoundManager = m_pRenderManager->GetSoundManager();
	}
	ATLASSERT( m_pSoundManager);	
	if ( m_pSoundManager == NULL)
	{
		return;
	}
	if ( m_iSoundId != -1)
	{
		if ( m_p3DObj)
		{
			m_pSoundManager->SetVolume( m_iSoundId, 0.0f);
		}
		else
		{
			m_pSoundManager->SetVolume( m_iSoundId, m_fVolume);
		}
	}
}

nrm3DObject *nrmAudio::get_p3dobj()
{
	return m_p3DObj;
}

double nrmAudio::GetFactorDist()
{
	return m_dFactorDist;
}

BOOL nrmAudio::IsFactorDist()
{
	return m_dFactorDist >= 0.1f;
}

double nrmAudio::GetMinDist()
{
	return m_dMinDist;
}

BOOL nrmAudio::IsMinDist()
{
	return m_dMinDist >= 0.1f;
}

void nrmAudio::PropIsSet()
{
	if ( IsSet( "fact_dist"))
	{
		m_dFactorDist = GetDoubleProp( "fact_dist");
	}
	if ( IsSet( "min_dist"))
	{
		m_dMinDist = GetDoubleProp( "min_dist");
	}
	if ( IsSet( "always_playing"))
	{
		BOOL bAlwaysPlaying = GetIntProp( "always_playing") == 1 ? TRUE : FALSE;
		if ( bAlwaysPlaying && m_iMode == DIS_PLAY)
		{
			// был установлен режим не проигрывать звук, устанавливаем режим проигрывать звук
			SetAudioMode( ENABLE_PLAY);
		}
		m_bAlwaysPlaying = bAlwaysPlaying;
	}
	
	nrmObject::PropIsSet();
}

void nrmAudio::SetAudioMode( int iMode)
{
	mlMedia* pMedia = GetMLMedia();
	if (!pMedia)
	{
		return;
	}

	if ( m_bAlwaysPlaying)
	{
		return;
	}

	rm::rmISoundManager* m_pSoundManager = NULL;
	if ( m_pRenderManager)
		m_pSoundManager = m_pRenderManager->GetSoundManager();
	ATLASSERT( m_pSoundManager);
	if ( m_pSoundManager == NULL)
		return;	

	int iPrevMode = m_iMode;
	m_iMode = iMode;
	if ( iPrevMode == DIS_PLAY && iPrevMode != m_iMode)
	{
		// открываем звук, если запрос был сделан и он еще не открыт.
		if ( m_bLoadNeed)
		{
			m_bLoadNeed = FALSE;
			OpenSound( m_wsSrc.c_str());
		}
	}

	mlIContinuous* pCont = pMedia->GetIContinuous();
	//m_bPlayAllow = (iMode == 0) ? FALSE : TRUE;
	if ( iMode == DIS_PLAY)
	{
		if ( m_iSoundId == -1)	
		{
			if ( m_bLoading || m_bOpening)
			{
				m_playingMutex.lock();
				m_bPlayingByLoading = FALSE;
				m_playingMutex.unlock();
			}
			return;
		}
		m_playingMutex.lock();
		m_bPlayingByLoading = FALSE;		
		if ( m_pSoundManager->IsPlaying( m_iSoundId))
		{
			//m_playingMutex.unlock();
			//m_pSoundManager->GetPosition( m_iSoundId, m_dwPos);
			//GetMLMedia()->GetIContinuous()->SetCurPos( m_dwPos);
			//m_playingMutex.lock();
			m_pSoundManager->StopSound( m_iSoundId);
			m_bPlaying = FALSE;
			m_bPlayed = FALSE;
			m_iStopReason = STOP_BY_USER;
			m_playingMutex.unlock();
		}
		else
		{
			if ( m_iStopReason == STOP_BY_DISTANCE || m_iStopReason == STOP_BY_DISALLOW)
				m_iStopReason = STOP_BY_USER;
			m_bPlaying = FALSE;
			m_bPlayed = FALSE;
			m_playingMutex.unlock();
		}
	}
	else if ( pCont->GetPlaying())
	{
		//pCont->Play();
		if ( m_iSoundId != -1)
		{
			m_playingMutex.lock();
			m_bPlayed = FALSE;
			m_bPlaying = TRUE;
			m_iStopReason = PLAY;
			if ( m_bPlayingEnabled)
			{
				m_pSoundManager->PlaySound( m_iSoundId, -1, GetMLMedia()->GetIContinuous()->GetLoop());
			}
			m_playingMutex.unlock();
			//PosChanged();
		}
		else if ( m_bLoading || m_bOpening)
		{
			m_playingMutex.lock();
			m_bPlayingByLoading = TRUE;
			m_playingMutex.unlock();
		}
	}
}

void nrmAudio::SetAudioVolume( float fVolume)
{
	rm::rmISoundManager* m_pSoundManager = NULL;
	if ( m_pRenderManager)
		m_pSoundManager = m_pRenderManager->GetSoundManager();
	ATLASSERT( m_pSoundManager);
	if ( m_pSoundManager == NULL)
		return;		

	if ( m_iScriptVolume >= 0)
	{
		return;
	}	

	if ( m_iSoundId == -1)
		return;

	m_fVolume = fVolume;

	if ( (( m_iEmitterType & SND_POINT) != 0) || (( m_iEmitterType & SND_SPLINE) != 0))
	{
		UpdateParams();
	}
	else
	{
		m_pSoundManager->SetVolume( m_iSoundId, fVolume);
	}
}

void nrmAudio::UpdateParams()
{
	rm::rmISoundManager* m_pSoundManager = NULL;
	if ( m_pRenderManager)
		m_pSoundManager = m_pRenderManager->GetSoundManager();
	ATLASSERT( m_pSoundManager);
	if ( m_pSoundManager == NULL)
		return;	

	nrm3DSoundManager *p3DSoundManager = m_pRenderManager->Get3DSoundManager();

	ATLASSERT( p3DSoundManager);
	if ( p3DSoundManager == NULL)
		return;	

	if ( m_iSoundId == -1)
		return;

	mlMedia *pMedia = this->GetMLMedia();	

	if ( pMedia == NULL)
		return;

	ATLASSERT( pMedia->GetIContinuous());	

	if ( pMedia->GetIContinuous() == NULL)
		return;

	m_playingMutex.lock();
	if ( !m_bPlayingEnabled || !pMedia->GetIContinuous()->GetPlaying() || !m_bPlaying || m_bPlayed)
	{
		m_playingMutex.unlock();
		return;
	}
	m_playingMutex.unlock();

	if ( m_sScriptLocation.compare( L"") != 0)
	{
		USES_CONVERSION;
		char *asLocation = W2A(m_sScriptLocation.c_str());
		//rm_trace("nrmAudio::UpdateParams - m_sScriptLocation '%s'\n", asLocation);
		//CComString s;
		//s.Format( "nrmAudio::UpdateParams - m_sScriptLocation '%s'", asLocation);
		//g->lw.WriteLn( s.GetBuffer());
		float aX = 0, aY = 0, aZ = 0;
		char *aLocation = NULL;//MP_NEW_ARR( char, MAX_PATH);
		m_pRenderManager->GetContext()->mpMapMan->GetAvatarPosition( aX, aY, aZ, &aLocation);			
		if ( aLocation == NULL)
		{
			//s.Format( "nrmAudio::UpdateParams - aLocation 'NULL'");
			//g->lw.WriteLn( s.GetBuffer());
			m_pSoundManager->SetVolume( m_iSoundId, 0.0f);
			//m_playingMutex.lock();
			//if ( m_pSoundManager->IsPlaying( m_iSoundId))
			//{
			//	m_pSoundManager->StopSound( m_iSoundId);
			//	m_iStopReason = STOP_BY_DISTANCE;
			//}
			//m_playingMutex.unlock();
			return;
		}
		//s.Format( "nrmAudio::UpdateParams - aLocation '%s'", aLocation);
		//g->lw.WriteLn( s.GetBuffer());
		wchar_t *awLocation = A2W(aLocation);			
		if ( m_sScriptLocation.compare( awLocation) != 0)
		{
			m_pSoundManager->SetVolume( m_iSoundId, 0.0f);
			//m_playingMutex.lock();
			//if ( m_pSoundManager->IsPlaying( m_iSoundId))
			//{
			//	m_pSoundManager->StopSound( m_iSoundId);
			//	m_iStopReason = STOP_BY_DISTANCE;
			//}
			//m_playingMutex.unlock();			
			return;
		}

		//m_playingMutex.lock();
		//if ( !m_pSoundManager->IsPlaying( m_iSoundId))
		//{
		//	m_pSoundManager->PlaySound( m_iSoundId, -1, GetMLMedia()->GetIContinuous()->GetLoop());
		//	m_iStopReason = PLAY;
		//}
		//m_playingMutex.unlock();		
	}

	if ( ( m_iEmitterType & SND_POINT) != 0)
	{
		//ATLASSERT( m_p3DObj);
		if ( !m_p3DObj)
		{
			return;
		}

		CVector3D dir = CVector3D( 0.0f, 1.0f, 0.0f);
		CVector3D av_dir = CVector3D( 1.0f, 0.0f, 0.0f);
		ml3DPosition pos = m_p3DObj->getAbsolutePosition();
		ml3DRotation rot = m_p3DObj->getAbsoluteRotation();
		CQuaternion qrot = CQuaternion( rot.x, rot.y, rot.z, rot.a);
		dir *= qrot;
		CVector3D absPos = CVector3D( pos.x, pos.y, pos.z);

		if ( IsFactorDist())
		{
			C3DSoundFactor t3DSoundFactor;
			if ( p3DSoundManager->calc( GetFactorDist(), GetMinDist(), absPos, dir, &t3DSoundFactor))
			{					
				t3DSoundFactor._attenutedCoef *= m_fVolume;
				m_pSoundManager->Set3DSoundParams( m_iSoundId, t3DSoundFactor._attenutedCoef,
				t3DSoundFactor._leftAttenutedCoef, t3DSoundFactor._rightAttenutedCoef);
				m_playingMutex.lock();
				if ( !m_pSoundManager->IsPlaying( m_iSoundId) && m_bPlaying && !m_bPlayed)
				{
					if ( m_iStopReason == STOP_BY_DISTANCE)
					{
						// проигрывание звука было остановлено уходом пользователем от источника звука
						m_pSoundManager->PlaySound( m_iSoundId, -1, GetMLMedia()->GetIContinuous()->GetLoop());
						m_iStopReason = PLAY;
					}
				}
				m_playingMutex.unlock();
			}
			else
			{
				m_pSoundManager->SetVolume( m_iSoundId, 0.0f);
				m_playingMutex.lock();
				if ( m_pSoundManager->IsPlaying( m_iSoundId))
				{
					m_pSoundManager->StopSound( m_iSoundId);
					m_iStopReason = STOP_BY_DISTANCE;
				}
				m_playingMutex.unlock();
			}
		}
		else
		{
			ml3DPosition vp_pos;
			ml3DPosition vp_dir;
			ml3DPosition cam_dir;
			//					
			if ( p3DSoundManager->calc( absPos, dir, vp_pos, vp_dir, cam_dir))
			{
				m_pSoundManager->Set3DSoundParams( m_iSoundId, vp_pos, vp_dir, cam_dir);
				m_playingMutex.lock();
				if ( !m_pSoundManager->IsPlaying( m_iSoundId) && m_bPlaying && !m_bPlayed)
				{
					if ( m_iStopReason == STOP_BY_DISTANCE)
					{
						// проигрывание звука было остановлено уходом пользователем от источника звука
						m_pSoundManager->PlaySound( m_iSoundId, -1, GetMLMedia()->GetIContinuous()->GetLoop());
						m_iStopReason = PLAY;
					}
				}
				m_playingMutex.unlock();
			}
			else
			{
				m_pSoundManager->SetVolume( m_iSoundId, 0.0f);
				m_playingMutex.lock();
				if ( m_pSoundManager->IsPlaying( m_iSoundId))
				{
					m_pSoundManager->StopSound( m_iSoundId);
					m_iStopReason = STOP_BY_DISTANCE;
				}
				m_playingMutex.unlock();
			}
		}

		return;
	}

	if ( ( m_iEmitterType & SND_SPLINE) != 0)
	{
		if ( m_pSplinePoints == NULL)
		{
			return;
		}

		ATLASSERT( m_p3DObj);
		if ( !m_p3DObj)
		{
			return;
		}

		C3DSoundFactor t3DSoundFactor;
		if ( p3DSoundManager->calc( GetFactorDist(), GetMinDist(), m_pSplinePoints, &t3DSoundFactor))
		{				
			t3DSoundFactor._attenutedCoef *= m_fVolume;
			m_pSoundManager->Set3DSoundParams( m_iSoundId, t3DSoundFactor._attenutedCoef,
									t3DSoundFactor._leftAttenutedCoef, t3DSoundFactor._rightAttenutedCoef);
			m_playingMutex.lock();
			if ( !m_pSoundManager->IsPlaying( m_iSoundId) && m_bPlaying && !m_bPlayed)
			{
				if ( m_iStopReason == STOP_BY_DISTANCE)
				{
					// проигрывание звука было остановлено уходом пользователем от источника звука
					m_pSoundManager->PlaySound( m_iSoundId, -1, GetMLMedia()->GetIContinuous()->GetLoop());
					m_iStopReason = PLAY;
				}
			}
			m_playingMutex.unlock();
		}
		else
		{
			m_pSoundManager->SetVolume( m_iSoundId, 0.0f);
			m_playingMutex.lock();
			if ( m_pSoundManager->IsPlaying( m_iSoundId))
			{
				m_pSoundManager->StopSound( m_iSoundId);
				m_iStopReason = STOP_BY_DISTANCE;
			}
			m_playingMutex.unlock();
		}		
		return;
	}
	
	/*if ( ( m_iEmitterType & SND_BACK_MUSIC) != 0)
	{
		float fVolume = 0.0f;
		m_pSoundManager->GetVolume( m_iSoundId, fVolume);
		if ( m_fVolume != fVolume)
			m_pSoundManager->SetVolume( m_iSoundId, m_fVolume);
	}*/

	// если звук не 3D, то надо запустить его на воспроизведение (если проигрывание не идет)
	float fVolume = 0.0f;
	m_pSoundManager->GetVolume( m_iSoundId, fVolume);
	if ( m_fVolume != fVolume)
	{
		m_pSoundManager->SetVolume( m_iSoundId, m_fVolume);
	}
}

int nrmAudio::GetEmitterType()
{
	return m_iEmitterType;
}

void nrmAudio::SetPlayingEnabled( bool bPlayingEnabled)
{
	rm::rmISoundManager* m_pSoundManager = NULL;
	if ( m_pRenderManager)
		m_pSoundManager = m_pRenderManager->GetSoundManager();
	ATLASSERT( m_pSoundManager);
	if ( m_pSoundManager == NULL)
		return;	
	
	m_playingMutex.lock();
	m_bPlayingEnabled = bPlayingEnabled;
	if ( !m_bPlayingEnabled)
	{
		if ( m_pSoundManager->IsPlaying( m_iSoundId))
		{
			m_pSoundManager->StopSound( m_iSoundId);
			m_iStopReason = STOP_BY_DISALLOW;
		}
	}
	else
	{
		mlMedia *pMedia = this->GetMLMedia();

		if ( pMedia == NULL)
			return;

		ATLASSERT( pMedia->GetIContinuous());
		if ( pMedia->GetIContinuous()->GetPlaying() && m_bPlaying && !m_bPlayed && !m_pSoundManager->IsPlaying( m_iSoundId))
		{
			m_pSoundManager->PlaySound( m_iSoundId, -1, GetMLMedia()->GetIContinuous()->GetLoop());
			m_iStopReason = PLAY;
		}
	}
	m_playingMutex.unlock();
}
