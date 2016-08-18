// nrmAudio.h: interface for the nrmAudio class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_nrmAudio_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
#define AFX_nrmAudio_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../res/res.h"
#include "nrmanager.h"
#include "nrmObject.h"
#include "iasynchresourcehandler.h"
#include "Mutex.h"
#include "../sm/sm.h"

#define SND_BUILD_H_STREAMING 1

class nrm3DObject;
class CBSpline;
class CSplinePoints;

class nrmAudio :	
					public nrmObject,
					public moIContinuous,
					public moISpeech,
					public IAsynchResourceHandler,
					public sm::smISoundNotify,
					public moIAudible
{
	friend class nrmCharacter;
	friend class nrm3DObject;
public:
	nrmAudio(mlMedia* apMLMedia);
	virtual ~nrmAudio();

// реализация moMedia
public:
	moILoadable* GetILoadable(){ return this; };
	moIContinuous* GetIContinuous(){ return this; };
	moISpeech* GetISpeech(){ return this; };
	moIAudible* GetIAudible(){ return this; };
	void PropIsSet();

// реализация  moILoadable
public:
	bool SrcChanged(); // загрузка нового ресурса
	bool Reload(){return false;} // перегрузка ресурса по текущему src

// реализация  moIAudible
public:
	bool EmitterTypeChanged(); // изменения типа звука
	bool SplineSrcChanged(); // загрузка нового ресурса сплайна
	bool VolumeChanged(); // 
	bool LocationChanged(); // установка локации для звука

// реализация  moIContinuous
public:
	void update(DWORD delta);
	// Свойства
	void PlayingChanged();	// Начать/Закончить проигрывание звука
	void FrameChanged();	// переставить звук на новое место
	void PosChanged();	// переставить звук на новое место (ms)
	long GetDuration();	// запрашивает значение _duration (продолжительность в ms)
	//
	void setCheckPoints(long* /*apfCheckPoints*/, int /*aiLength*/){}
	long getPassedCheckPoint(){ return 0; }
	unsigned long GetCurPos();

// реализация  moISpeech
public:
	long GetCurrentPos(){ return (long) GetCurPos();};

// реализация  sm::smISoundNotify
public:
	void OnLoaded( int iError);
	void OnStopped( int iReason);

// реализация  res::IAsynchResourceHandler
public:
	void OnAsynchResourceLoaded(IAsynchResource* asynch);
	void OnAsynchResourceError(IAsynchResource* asynch);
	void OnAsynchResourceLoadedPersent(IAsynchResource* asynch, unsigned char percent);

protected:
	BOOL OpenSound( const wchar_t* pwcSrc);
	void onOpened();
	void PauseLoading( IAsynchResource* asynch);

public:
	BOOL IsLoading();
	BOOL IsLoaded();

public:
	enum{ ON_LOADED = 101, ON_STOPPED, ON_DESTROY};

	void set_p3dobj( nrm3DObject *ap3DObj, ml3DPosition aPos3D);
	nrm3DObject *get_p3dobj();
	double GetFactorDist();
	BOOL IsFactorDist();
	double GetMinDist();
	BOOL IsMinDist();
	void SetAudioMode( int iMode);
	void SetAudioVolume( float fVolume);
	void UpdateParams();
	int GetEmitterType();
	void SetPlayingEnabled( bool bPlayingEnabled);

protected:
	// идентификатор звука
	int				m_iSoundId;

	// тип звука
	int				m_iEmitterType;

	// текущая позицияи в проигрываемом звуке (в мс)
	DWORD			m_dwPos;

	// указывает на то, что ресурс загружен ResLoader и открыт SoundMan
	BOOL			m_bLoaded;	

	// указывает на то, что ресурс грузится ResLoader
	BOOL			m_bLoading;

	// указывает на то, что звук открывается SoundMan
	BOOL			m_bOpening;

	// указывает на то, что звук проигрывается во время загрузки ресурса
	BOOL			m_bPlayingByLoading;
#if SND_BUILD_H_STREAMING

	// тип звука, указывает на возможность проигрывание звука во время загрузки звука
	BOOL			m_bStreaming;

	// кол-во байтов загруженного ресурса
	DWORD			m_dwLoadedBytes;

	// длительность звука
	DWORD			m_dwDuration;

	// размер звука
	DWORD			m_dwSize;	

	// указывает на процесс удаления
	BOOL			m_bRemoving;

	// критическая секция на удаление звука
	CMutex			m_removingMutex;

	// критическая секция на загрузку звука
	CMutex			m_loadingUpdateMutex;
#endif
	// указатель на 3D-объект
	nrm3DObject		*m_p3DObj;

	// позиция звука в 3D мире
	ml3DPosition	pos3D;

	// время начала загрузки ресурса
	DWORD			m_dwStartLoadTime;

	// объект асинхронной загрузки ресурса
	IAsynchResource*	m_pAsyncResource;

	// коэффициент падения тона звука (в см)
	double			m_dFactorDist;

	// расстояние, на котором тон звука не уменьшается при удаление пользователя от звука
	double			m_dMinDist;

	// режим: воспроизводить звук или отключить воспроизведение звука
	enum{ DIS_PLAY, ENABLE_PLAY};
	int				m_iMode;

	// всегда воспроизводить звук независимо от режима m_iMode (настроек академии)
	BOOL			m_bAlwaysPlaying;

	// имя ресурса
	std::wstring	m_wsSrc; 

	// ресурс необходимо загрузить
	BOOL			m_bLoadNeed; 

	// уровень громкости звука (тон)
	float			m_fVolume;

	// уровень громкости звука (тон), заданная скриптом
	// значенпре < 0 означает, что уровень громкости звука от скрипта не задан (или сброшен)
	int				m_iScriptVolume;

	// необходимо обновить данные о загрузке ресурса в SoundMan
	BOOL			m_bNeedUpdate;	

	std::wstring	m_sScriptLocation;

#ifdef DEBUG
	int destroy_counter;
#endif
	
	// объект сплайн
	CBSpline *m_pSpline;
	// объект с списоком точек сплайна
	CSplinePoints *m_pSplinePoints;

	// причина остановки звука
	enum{ PLAY, STOP_BY_USER, STOP_BY_ENDFILE, STOP_BY_DISTANCE, STOP_BY_DISALLOW};
	int		m_iStopReason;

	// звук проигрывается
	BOOL	m_bPlaying;
	// критическая секция на изменения статуса звука: проигрывание или остановка
	CMutex	m_playingMutex;
	// звук проигран
	BOOL	m_bPlayed;
	// разрешение на проигрывание звука
	BOOL	m_bPlayingEnabled;
	// список событий, который надо обработать update
	//MP_VECTOR<TAudioEvent*> m_audioEvents;
	//CMutex	m_eventMutex;

public:
	void HandleOnLoaded( int iError);
	void HandleOnStopped( int iReason);
	void HandleOnDestroy();
	void Destroy();

private:
	//CEvent	evEventsUpdated;
};

#endif // !defined(AFX_nrmAudio_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
